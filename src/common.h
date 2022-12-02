#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdbool.h>
#include <apr-1.0/apr_file_io.h>
#include <apr-1.0/apr_mmap.h>


//an enum for json simple types
typedef enum {
    json_string=0,    
    json_bool,
    json_number,
    json_null
} json_simple_types_enum;

static const apr_size_t file_buffer_size = 1024*2*2; // A buffer size constant for buffered file streams
static const apr_size_t line_buffer_size = 1024*2;   // a buffer to read  a line 
static const apr_size_t max_key_length = 50;

// Size of the memory mapped file :
// Will will use a dictionary of  UINT_MAX  entries  which will cover all possible values of the key has as the latter
// is unsigned integer. Each entry will contain 2 elements with the following sizes:
//- The hash_key:  size = sizeof(uint)
//- The json key value : size = max_key_length * sizeof(char) , in this memory space we will store a tlv value of the json key (a string).
static const apr_uint64_t  mm_compacted_key_size = max_key_length * sizeof(unsigned char);
static const apr_uint64_t  mmf_entry_size = sizeof(unsigned int) + mm_compacted_key_size;
static const apr_uint64_t  mmf_hash_map_size = (UINT_MAX) * mmf_entry_size;

// A storage facility struct to store a string buffer (ending with \0) or binary 
typedef struct  {
    unsigned char* data;
    apr_size_t length;
    json_simple_types_enum data_type;//string by default
    
} string_buffer_t;

void string_buffer_t_init(string_buffer_t str_buffer);


// A struct for storing a parsed key-value pair. As we are sure that all json value are simple values (not objects nor arrays),
// we will just store values as string (as read from the original input file).
typedef struct {
    string_buffer_t key;    //JSON key
    string_buffer_t value;  // This is the string representation of the json value as read from the file
} key_value_t;

/////////////////////////////
// common global functions
/////////////////////////////

// Encode a binary buffer into a binary tlv buffer of size 'max_key_length'

// NULL is return in case of error
bool tlv_encode_buffer(apr_pool_t* pool,const string_buffer_t* source, string_buffer_t* destination, char** error_message);

// A function to hash a string using APR default hashing function
unsigned int hash_key(const string_buffer_t* string_buffer);

//generate random 'text' string buffer , if size is negative or zero a random size will be used
//max random size is 50 for convenience
void generate_fake_buffer_string(apr_pool_t* pool, string_buffer_t* buffer,int size);


#define IF_ERROR_BREAK(rv,statement)  if(rv = statement,rv != APR_SUCCESS) break

#define FORMAT_ERROR(rv,error_message)  if(rv!= APR_SUCCESS) { \
            char arp_error[256]; \
            apr_strerror(rv, arp_error, sizeof(arp_error)); \
            *error_message = apr_psprintf(the_pool, "Error in %s: %s\n", __FUNCTION__, arp_error); \
            } \
            else \
                *error_message = apr_psprintf(the_pool, "Error in %s\n", __FUNCTION__)


#endif // TEST_COMMON_H
