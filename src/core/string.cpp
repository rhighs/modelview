#include "string.h"
#include "vec.h"

#include <stdio.h>
#include <memory.h>
#include <assert.h>

//[CONTAINER_ACCESS]
String String::from(const _STRING_CHAR_TYPE *c_string) {
    String result;
    u32 c_len = _STRING_GET_LEN_FUNC(c_string);

    result._c_data._realloc_for(c_len * sizeof(_STRING_CHAR_TYPE));

    _STRING_CHAR_TYPE *base = (result._c_data.raw());
    for (u32 k=0; k<c_len; k++)
        *(base++) = c_string[k];

    *(result._c_data._len()) = c_len;

    return result;
}

Vec<String> String::split(_STRING_CHAR_TYPE token) const {
    Vec<String> result;
    String current;
    for (_STRING_CHAR_TYPE c : *this) {
        if (c != token) {
            current.push_back(c);
        } else {
            result.push_back(current);
            current = String::from("");
        }
    }
    result.push_back(current);
    return result;
}

// assume all types end in '\0'
#define _STRING_CHAR_TYPE_TERM '\0'

// sets a null character at the end of the internal container
_STRING_CHAR_TYPE* String::raw() const {
    /* FIXME: this could happen, inore it for now
    {
        u32 potential_max_len = *(_c_data._capacity()) * sizeof(_STRING_CHAR_TYPE);
        len() == potential_max_len
    }
    */
   _STRING_CHAR_TYPE *end = _c_data.raw() + *(_c_data._len());
   *(end + 1) = _STRING_CHAR_TYPE_TERM;
   return _c_data.raw();
}
