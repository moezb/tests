#include <stdio.h>
#include <apr-1.0/apr_pools.h>
#include <apr-1.0/apr.h>

#include "common.h"
#include "dictionary_data.h"
#include "input_data.h"
#include "output_data.h"
#include <apr-1.0/apr_pools.h>
#include <apr-1.0/apr_strings.h>
// Our global pool 
apr_pool_t* the_pool = NULL;



int main(char** argv, int argc) {

    apr_initialize();
    atexit(apr_terminate);    

    input_reader_t reader; 
    output_writer_t writer; 
    mm_dictionary_file_t dictionary_file; 

    char* error_message = NULL;

    const char* input_file_path = "/home/moez/input_file.json";
    const char* out_dict_path = "/home/moez/key_dict.dat";
    const char* out_data_path = "/home/moez/compacted_output.dat.txt";

    apr_pool_create(&the_pool, NULL);

    input_reader_t_init(&reader);
    output_writer_t_init(&writer);
    mm_dictionary_file_t_init(&dictionary_file);
    apr_pool_t* line_processing_mem_pool = NULL;
    bool success = false;

    do {
        if (!reader.open(&reader, input_file_path, &error_message)) break;
        if (!dictionary_file.create_and_open(&dictionary_file, out_dict_path, &error_message)) break;
        if (!writer.open(&writer, out_data_path, &error_message)) break;
        do {
            record_t record; record_t_init(&record);
            bool eof = false;
            if (line_processing_mem_pool != NULL)   apr_pool_destroy(line_processing_mem_pool);
            apr_pool_create(&line_processing_mem_pool, the_pool);
            if (!reader.read_record(line_processing_mem_pool,&reader, &record, &eof, &error_message)) break;
            compact_record_t compact_record;
            compact_record_t_init(&compact_record);
            compact_record.resize(line_processing_mem_pool,&compact_record,record.count);
            bool line_processing_success = true;
            for (int i = 0; i < record.count && line_processing_success; i++) {
                compact_key_value_t compact_key_value;
                compact_key_value.key = hash_key(&(record.key_value_list[i].key));
                if (!tlv_encode_buffer(line_processing_mem_pool,&record.key_value_list[i].key, &compact_key_value.value, &error_message)) {
                    line_processing_success = false;
                    break;
                }
                if (!dictionary_file.emplace(&dictionary_file, &compact_key_value, &error_message)) {
                    line_processing_success = false;
                    break;
                }
                if (!compact_record.set(line_processing_mem_pool,i,&compact_record, &compact_key_value, &error_message)) {
                    line_processing_success = false;
                    break;
                }
            }
            if (!line_processing_success) break;
            if (!writer.write_entry(&writer, &compact_record, &error_message)) break;
            apr_status_t rv = apr_file_flush(writer.the_file);
            rv= apr_file_flush(dictionary_file.the_file);
            if (rv != APR_SUCCESS) {
                char arp_error[256]; 
                apr_strerror(rv, arp_error, sizeof(arp_error));  
                printf(apr_psprintf(the_pool, "file flush error %s\n",arp_error));
                break;
            }
            success = true;
        } while (true);

        //trying close files any way
        if (reader.the_file && reader.close(&reader, &error_message)) break;
        if (dictionary_file.the_mmap && dictionary_file.close(&dictionary_file, &error_message)) break;
        if (writer.the_file && writer.close(&writer, &error_message)) break;

    } while (false);

    if (success) success = (error_message == NULL);
    if (!success) printf(error_message);
    if (line_processing_mem_pool != NULL) apr_pool_destroy(line_processing_mem_pool);
    apr_pool_destroy(the_pool);
    exit( success?0:-1);
}
