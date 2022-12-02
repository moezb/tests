#ifndef DICTIONARY_DATA_H
#define DICTIONARY_DATA_H

#include "common.h"

// A struct representing a 'compressed' (integral key -  string value) pair
// This struct is basically an entry in our hash map dictionary
typedef struct {
    unsigned int key; // the hash (digest) of the json key
    string_buffer_t value; // the LTV representation of the key value
    // this is a buffer of size : mmf_hash_map_size
} compact_key_value_t;

compact_key_value_t* compact_key_value_t_new(apr_pool_t* pool);

// A struct representing the compacted json record in the output data file
typedef struct {
    //key-value pairs count
    unsigned int count;
    // array of key-value pairs
    compact_key_value_t** compact_key_value_list;
    //resize the compact_key_value_list
    void (*resize)(apr_pool_t*, struct compact_record_t* ,unsigned int size);
    // A function to set  key-value pair at the index i of the array member
    bool (*set)(apr_pool_t*, unsigned int index, struct compact_record_t*, compact_key_value_t*, char** error_message); 
} compact_record_t;

compact_record_t* compact_record_t_new(apr_pool_t* pool);
void compact_record_t_init(compact_record_t*);
void compact_record_t_resize(apr_pool_t*, compact_record_t*, unsigned int);
bool compact_record_t_set(apr_pool_t*, unsigned int ,compact_record_t*, compact_key_value_t*, char** );



/* A struct to hold and manage a memory mapped sparse file that is going to be used as
 * our hash map dictionary :

+-----------------+---------------------------------------------+
| hash key UINT   | TLV compressed key(50 char max)             |
+---------------------------------------------------------------+
| 0               |                                             |
+------------------------------------------------------------ - +
|                 |                                             |
+------------------------------------------------------------ - +
| 1               |                                             |
+------------------------------------------------------------ - +
.                                                               .
.                                                               .
|                 |                                             |
+ ------------------------------------------------------------- +
| 4294967294      |                                             |
+-------------------------------------------------------------- +
| 4294967295      |                                             |
+-----------------+-------------------------------------------- +

*/

typedef struct {
    // create and open the memory mapped file (we will use  sparse file to save disk space)
    bool (*create_and_open)(struct mm_dictionary_file_t*, const char* file_path, char** error_message);
    //close (and save) the memory mapped file 
    bool (*close)(struct mm_dictionary_file_t*, char** error_message);
    //Insert a new hah and its corresponding key value to our dictionary.
    //Note that insertion is straightforward and fast as it is just an assignment to a know memory location.
    //We don't have to perform search nor sorting nor mem move operations.
    bool (*emplace)(struct mm_dictionary_file_t*, compact_key_value_t* dictionary_entry, char** error_message);
    apr_mmap_t* the_mmap;
    apr_file_t* the_file;

} mm_dictionary_file_t;

mm_dictionary_file_t* mm_dictionary_file_t_new();
void mm_dictionary_file_t_init(mm_dictionary_file_t*);
static bool mm_dictionary_file_t_create_and_open(mm_dictionary_file_t*, const char* file_path, char** error_message);
static bool mm_dictionary_file_t_close(mm_dictionary_file_t*, char** error_message);
static bool mm_dictionary_file_t_emplace(mm_dictionary_file_t*, compact_key_value_t* dictionary_entry, char** error_message);

#endif // DICTIONARY_DATA_H

