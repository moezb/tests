#include "common.h"

#include <assert.h>
#include <time.h>
#include <apr-1.0/apr_hash.h>
#include <apr-1.0/apr_pools.h>
#include <stdlib.h>
#include <apr-1.0/apr_strings.h>

extern  apr_pool_t* the_pool;
//faked
bool tlv_encode_buffer(apr_pool_t* pool,const string_buffer_t* source, string_buffer_t* destination, char** error_message) {
    //generate smaller then 50
    assert(source != NULL);
    assert(source->length > 0);
    generate_fake_buffer_string(pool,destination, source->length==1?1:source->length/2);
   return true;
}

unsigned int hash_key(const string_buffer_t* string_buffer) {
    assert(string_buffer != NULL);
    assert(string_buffer->data != NULL);
    return apr_hashfunc_default((const char*)string_buffer->data,&string_buffer->length);
}


void generate_fake_buffer_string(apr_pool_t* pool,string_buffer_t* buffer,int size) {
    assert(buffer != NULL);
    const char* abcd = "0123456789abcdefghijklmnopqrstuvwxyz.-";
    srand(time(NULL));
    //max random size is 50
    if (size > 0)
        buffer->length = size;
    else
        buffer->length = 1 + (rand() % 50);

    buffer->data = apr_pcalloc(pool, buffer->length);
    srand(time(NULL));
    for(int i=0;i<buffer->length-1;i++) {
        buffer->data[i] = abcd[rand() % 39] ;//39 is the strlen of abcd
    }
    buffer->data_type = json_string;
}