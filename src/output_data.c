#include "output_data.h"

#include <assert.h>
#include <apr-1.0/apr_file_io.h>
#include <apr-1.0/apr_pools.h>
#include <apr-1.0/apr_strings.h>

#include "dictionary_data.h"

extern  apr_pool_t* the_pool;

#define DEBUG_OUTPUT_TEXT  1

output_writer_t* output_writer_t_new() {
    output_writer_t* writer = apr_pcalloc(the_pool, sizeof(output_writer_t));
    output_writer_t_init(writer);
    return writer;
}
void output_writer_t_init(output_writer_t* writer) {
    assert(writer != NULL);
    writer->the_file = NULL;    
    writer->open = output_writer_t_open;
    writer->close = output_writer_t_close;
    writer->write_entry = output_writer_t_write_entry;
}

bool output_writer_t_open(output_writer_t* writer, const char* file_path, char** error_message) {
    apr_status_t rv = APR_SUCCESS;
    do {
        if (writer == NULL)  break;
        if (file_path == NULL) break;
        IF_ERROR_BREAK(rv, apr_file_open(&writer->the_file, file_path, APR_FOPEN_CREATE| APR_FOPEN_TRUNCATE| APR_FOPEN_WRITE| APR_FOPEN_BUFFERED, APR_FPROT_OS_DEFAULT, the_pool));
        return true;
    } while (false);

    FORMAT_ERROR(rv, error_message);
    return false;
}
bool output_writer_t_close(output_writer_t* writer, char** error_message) {
    apr_status_t rv = APR_SUCCESS;
    do {
        if (writer == NULL || writer->the_file == NULL) break;
        IF_ERROR_BREAK(rv, apr_file_close(writer->the_file));
        writer->the_file = NULL;
        return true;
    } while (false);
    FORMAT_ERROR(rv, error_message);
    return false;
}
bool output_writer_t_write_entry(output_writer_t* writer, compact_record_t* record, char** error_message) {
    apr_status_t rv = APR_SUCCESS;
    do {
        if (writer == NULL || writer->the_file == NULL) break;
        if (writer == NULL) break;
        if (record == NULL) break;
        //empty record
        if (record->compact_key_value_list== NULL) return true;
        if (record->count == 0) return true;
        bool write_success = true;
        for (int i=0;i < record->count && write_success;i++) {
            const compact_key_value_t* compact_key_value = record->compact_key_value_list[i];
#if DEBUG_OUTPUT_TEXT
            const char* line = apr_psprintf(the_pool, "%d:\"%s\",", compact_key_value->key, compact_key_value->value.data);
            apr_size_t size = strlen(line)*sizeof(char);
            apr_size_t osize;
            if (rv = apr_file_write_full(writer->the_file ,&line, size,&osize), rv != APR_SUCCESS) {
                write_success = false; break;
            }
            apr_file_flush(writer->the_file);
#else
            apr_size_t size = sizeof(unsigned int);
            if (rv = apr_file_write(writer->the_file, &compact_key_value->key, &size), rv != APR_SUCCESS) {
                write_success = false; break;
            }
            size = sizeof(unsigned char) * compact_key_value->value.length;
            if (rv = apr_file_write(writer->the_file, &compact_key_value->value.data, &size), rv != APR_SUCCESS) {
                write_success = false; break;
            }
#endif
        } 

        if (!write_success)  break;
        return true;
        
    } while (false);
    FORMAT_ERROR(rv, error_message);
    return false;
}