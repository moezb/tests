#ifndef INPUT_DATA_H
#define INPUT_DATA_H
 
#include "common.h"

// A struct representing the original json record (a json document)
// This struct stores the result of a decoding a line of our original input file
typedef struct {
    //key-value pairs count
    unsigned int count; 
    //array of key-value pairs
    key_value_t* key_value_list;
    // A function to append a key-value pair to the array member
    void (*record_t_append)(apr_pool_t* ,struct record_t*, key_value_t*, char** error_message);
} record_t;

record_t* record_t_new(apr_pool_t*);
void record_t_init(record_t* record);
static bool record_t_append(apr_pool_t* pool, record_t* record, key_value_t*, char** error_message);

// A struct interface for reading and processing the input file
typedef struct {
    //Open the input file
    bool (*open)(struct input_reader_t* reader, const char* file_path, char** error_message);
    //Close the input file
    bool (*close)(struct input_reader_t* reader, char** error_message);
    //read a json record and advance
    //return true if a read was performed correctly, false otherwise, eof will indicate wether the failure
    //was caused by an eof
    bool (*read_record)(apr_pool_t*pool,struct input_reader_t* reader, record_t* output_record, bool* eof, char** error_message);
    //set to true if the last  read_record call triggered an error, false otherwise
    bool has_error;
    apr_file_t* the_file;
} input_reader_t;

input_reader_t* input_reader_t_new(apr_pool_t*);
void input_reader_t_init(input_reader_t* reader);
bool input_reader_t_open(input_reader_t* reader, const char* file_path, char** error_message);
static bool input_reader_t_close(input_reader_t* reader, char** error_message);
static bool input_reader_t_read_record(apr_pool_t* pool,input_reader_t* reader, record_t* output_record, bool* eof, char** error_message);


// read a line from the input stream file and return it in the output argument buffer
bool read_line(apr_pool_t* pool,apr_file_t* file, string_buffer_t* buffer, char** error_message);

// initialize \param output_record from the a string (a line read previously from input file)
bool reader_decode_line(apr_pool_t* pool, string_buffer_t* line, record_t* output_record, char** error_message);

#endif // INPUT_DATA_H 
