#include "dictionary_data.h"

#include <assert.h>
#include <apr-1.0/apr_file_io.h>
#include <apr-1.0/apr_mmap.h>
#include <apr-1.0/apr_pools.h>
#include <apr-1.0/apr_strings.h>

#include "input_data.h"


extern  apr_pool_t* the_pool;

compact_key_value_t* compact_key_value_t_new(apr_pool_t* pool) {
    assert(pool != NULL);
    compact_key_value_t* key_value = apr_pcalloc(pool, sizeof(compact_key_value_t));    
    return key_value;
}

compact_record_t* compact_record_t_new(apr_pool_t* pool) {
    assert(pool != NULL);
    compact_record_t* record = apr_pcalloc(pool, sizeof(compact_record_t));
    compact_record_t_init(record);
    return record;
}
void compact_record_t_init(compact_record_t* record) {
    assert(record != NULL);
    record->count = 0;
    record->compact_key_value_list= NULL;
    record->set = compact_record_t_set;
    record->resize = compact_record_t_resize;
}
bool compact_record_t_set(apr_pool_t* pool, unsigned int index,compact_record_t* record, compact_key_value_t* compact_key_value, char** error_message) {
    assert(record != NULL);
    assert(compact_key_value != NULL);
    if (index>=record->count) {
        *error_message = apr_pstrdup(the_pool, "Error in compact_record_t_set:index overflow\n");
        return false;
    }
    record->compact_key_value_list[index]= compact_key_value;
    return true;
}

void compact_record_t_resize(apr_pool_t* pool, compact_record_t* record, unsigned int size) {
    assert(pool != NULL);
    assert(record != NULL);
    record->compact_key_value_list= apr_palloc(pool, size* sizeof(compact_key_value_t));
    record->count = size;
}

mm_dictionary_file_t* mm_dictionary_file_t_new(apr_pool_t* pool) {
    assert(pool != NULL);
    mm_dictionary_file_t* dictionary = apr_pcalloc(pool, sizeof(mm_dictionary_file_t));
    mm_dictionary_file_t_init(dictionary);
    return dictionary;
}
void mm_dictionary_file_t_init(mm_dictionary_file_t* dictionary) {
    assert(dictionary != NULL);
    dictionary->the_mmap=NULL;
    dictionary->the_file=NULL;
    dictionary->create_and_open = mm_dictionary_file_t_create_and_open;
    dictionary->close = mm_dictionary_file_t_close;
    dictionary->emplace = mm_dictionary_file_t_emplace;

}
bool mm_dictionary_file_t_create_and_open(mm_dictionary_file_t* dictionary, const char* file_path, char** error_message) {
    apr_status_t rv = APR_SUCCESS;
    
    do {
        if (dictionary == NULL)  break;

        if (dictionary->the_file != NULL)  break;

        if (dictionary->the_mmap != NULL)  break;
        
       IF_ERROR_BREAK(rv,apr_file_open(&dictionary->the_file, file_path, APR_FOPEN_CREATE | APR_FOPEN_TRUNCATE | APR_FOPEN_READ | APR_FOPEN_WRITE | APR_FOPEN_SPARSE,APR_FPROT_OS_DEFAULT, the_pool));        

       IF_ERROR_BREAK(rv,apr_mmap_create(&dictionary->the_mmap, dictionary->the_file, 0, mmf_hash_map_size, APR_MMAP_WRITE, the_pool));

       return true;

    } while (false);
    
    FORMAT_ERROR(rv, error_message);
    return false;
}
bool mm_dictionary_file_t_close(mm_dictionary_file_t* dictionary, char** error_message) {
    apr_status_t rv = APR_SUCCESS;
    do {
        if (dictionary == NULL || dictionary->the_file == NULL) break;
        IF_ERROR_BREAK(rv, apr_file_close(dictionary->the_file));

        if (dictionary->the_mmap != NULL)
            IF_ERROR_BREAK(rv, apr_mmap_delete(dictionary->the_mmap));

        dictionary->the_file = NULL;
        dictionary->the_mmap = NULL;

        return true;

    } while (false);

    FORMAT_ERROR(rv, error_message);
    return false;
}
bool mm_dictionary_file_t_emplace(mm_dictionary_file_t* dictionary, compact_key_value_t* dictionary_entry, char** error_message) {
    apr_status_t rv = APR_SUCCESS;
    do {
        if (dictionary == NULL) break;
        if (dictionary->the_mmap == NULL) break;
        if (dictionary->the_mmap == NULL) break;
        if (dictionary_entry == NULL) break;
        const apr_off_t offset = (dictionary_entry->key * mmf_entry_size)+1;
        struct {
            unsigned int key; 
            unsigned char value[mmf_entry_size]; 
        } compact_key_value;
        
        //get the entry offset address
        IF_ERROR_BREAK(rv, apr_mmap_offset(&compact_key_value, dictionary->the_mmap, offset));
        //write the key disgest
        compact_key_value.key = dictionary_entry->key;
        //write the key tlv compacted size fixed value
        memcpy(compact_key_value.value, dictionary_entry->value.data, mm_compacted_key_size);
        return true;
    } while (false);

    FORMAT_ERROR(rv, error_message);
    return false;
}