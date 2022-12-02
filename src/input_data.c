#include "input_data.h"
#include <assert.h>
#include <apr-1.0/apr.h>
#include <apr-1.0/apr_file_info.h>
#include <apr-1.0/apr_file_io.h>
#include <apr-1.0/apr_pools.h>
#include <apr-1.0/apr_strings.h>

extern  apr_pool_t* the_pool;

//FAKED
bool reader_decode_line(apr_pool_t* pool,string_buffer_t* line, record_t* output_record, char** error_message) {
    //Fake json parse
    output_record->count = 3;
    output_record->key_value_list= apr_palloc(pool, 3*sizeof(key_value_t));
    for (int i = 0; i < output_record->count; i++) {
        key_value_t* key_value = &output_record->key_value_list[i];
        generate_fake_buffer_string(pool, &key_value->key,-1);
        generate_fake_buffer_string(pool, &key_value->value, -1);
    }
    return true;
}

bool read_line(apr_pool_t* pool,apr_file_t* file, string_buffer_t* line_buffer, char** error_message) {
    assert(file != NULL);
    assert(line_buffer != NULL);
    assert(error_message!=NULL);
    apr_status_t rv = APR_SUCCESS;
    do {
        line_buffer->data = apr_pstrdup(pool, "");
        line_buffer->length = 1;

        while((apr_file_eof(file) != APR_EOF)) {
            char tmp_buffer[line_buffer_size];
            memset(tmp_buffer, NULL, line_buffer_size);
            rv = apr_file_gets(tmp_buffer, line_buffer_size, file);
            const char last = tmp_buffer[line_buffer_size -2];
            
            if (last=='\n') { //we're done reading a single line
                line_buffer->data = apr_pstrcat(pool, line_buffer->data, tmp_buffer,NULL);
                line_buffer->length = strlen(line_buffer->data)+1;
                break;
            }
            char* state;
            char* next = apr_strtok(tmp_buffer, "\n",&state);
            if (next==NULL) {
                line_buffer->data = apr_pstrcat(pool, line_buffer->data, tmp_buffer, NULL);
                line_buffer->length = strlen(line_buffer->data) + 1;
            }
            else {//we're done reading a single line
                line_buffer->data = apr_pstrcat(pool, line_buffer->data,next, NULL);
                line_buffer->length = strlen(line_buffer->data) + 1;
                break;
            }
        }        
        if (rv != APR_SUCCESS)  break;
        return true;
    } while (false);
    FORMAT_ERROR(rv, error_message);
    return false;
}

record_t* record_t_new(apr_pool_t* pool) {
    assert(pool != NULL);
    record_t* record = apr_pcalloc(pool,sizeof(record_t));
    record_t_init(record);
    return record;
}
void record_t_init(record_t* record) {
    assert(record != NULL);
    record->record_t_append = record_t_append;
}
bool record_t_append(apr_pool_t* pool, record_t* record, key_value_t* key_value, char** error_message) {
    *error_message = apr_pstrdup(pool, "record_t_append Not Implemented");
    return false;
}

input_reader_t* input_reader_t_new(apr_pool_t* pool){
    assert(pool != NULL);
    input_reader_t* reader= apr_pcalloc(pool, sizeof(input_reader_t));
    input_reader_t_init(reader);
    return reader;
}
void input_reader_t_init(input_reader_t* reader){
    assert(reader != NULL);
    reader->has_error = false;
    reader->the_file = NULL;    
    reader->open = input_reader_t_open;
    reader->close = input_reader_t_close;
    reader->read_record = input_reader_t_read_record;
}
bool input_reader_t_open(input_reader_t* reader, const char* file_path, char** error_message){
    apr_status_t rv = APR_SUCCESS;
    do {
       if (reader == NULL)  break;
        if (file_path == NULL) break;
        IF_ERROR_BREAK(rv, apr_file_open(&reader->the_file, file_path, APR_FOPEN_READ | APR_FOPEN_BUFFERED, APR_FPROT_OS_DEFAULT, the_pool));
        return true;
    } while (false);    
    FORMAT_ERROR(rv,error_message);
    return false;
}
bool input_reader_t_close(input_reader_t* reader, char** error_message){
    apr_status_t rv = APR_SUCCESS;
    do {
        if (reader == NULL || reader->the_file == NULL) break;
        IF_ERROR_BREAK(rv, apr_file_close(reader->the_file));
        reader->the_file = NULL;
        return true;
    } while (false);
    FORMAT_ERROR(rv, error_message);
    return false;
}
bool input_reader_t_read_record(apr_pool_t* pool, input_reader_t* reader, record_t* output_record, bool* eof, char** error_message) {
    assert(pool != NULL);
    do {
        char* buf = NULL;
        if (reader == NULL || reader->the_file == NULL) break;
        if (output_record == NULL) break;
        if (eof == NULL) break;
        *eof = false;
         
        if (apr_file_eof(reader->the_file) == APR_EOF) {
            *eof = true;
            return false;
        }        
        string_buffer_t line;
        if (!read_line(pool,reader->the_file, &line, error_message)) break;
        if (!reader_decode_line(pool,&line, output_record, error_message)) break;
        return true;
    } while (false);
    *error_message = apr_psprintf(the_pool, "Error in %s\n", __FUNCTION__);
    return false;
}