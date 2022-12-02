#ifndef OUTPUT_DATA_H
#define OUTPUT_DATA_H

#include"dictionary_data.h"

// A struct interface for writing the output
typedef struct {
    //Open the output compacted data  file
    bool (*open)(struct output_writer_t*, const char* file_path, char** error_message);
    //Open the input file
    bool (*write_entry)(struct output_writer_t*, compact_record_t*, char** error_message);
    //Close the output compacted data  file
    bool (*close)(struct output_writer_t*, char** error_message);
    apr_file_t* the_file;
    FILE* file;
} output_writer_t;

output_writer_t* output_writer_t_new();
void output_writer_t_init(output_writer_t* writer);

static bool output_writer_t_open(output_writer_t*, const char* file_path, char** error_message);
static bool output_writer_t_write_entry(output_writer_t*, compact_record_t*, char** error_message);
static bool output_writer_t_close(output_writer_t*, char** error_message);

#endif // !OUTPUT_DATA_H


