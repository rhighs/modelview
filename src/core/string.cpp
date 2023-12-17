#include "string.h"
#include "vec.h"

#include <stdio.h>
#include <memory.h>
#include <assert.h>

String String::from(const _STRING_CHAR_TYPE *c_string) {
    u32 c_len = _STRING_GET_LEN_FUNC(c_string);
    String result = String::with_capacity(c_len);
    memcpy(result._c_data._raw_data, c_string, c_len * sizeof(_STRING_CHAR_TYPE));
    *(result._c_data._len()) = c_len;
    return result;
}

String String::from(const _STRING_CHAR_TYPE* c_string, u32 len) {
    String result = String::with_capacity(len);
    memcpy(result._c_data._raw_data, c_string, len * sizeof(_STRING_CHAR_TYPE));
    *(result._c_data._len()) = len;
    return result;
}

String String::strip(const _STRING_CHAR_TYPE strip_ch) const {
    _STRING_CHAR_TYPE *start = _c_data.raw();
    while (*start == strip_ch) start++;
    _STRING_CHAR_TYPE *end = _c_data.raw() + len();
    while (*end == strip_ch) end--;
    if (start >= end)
        return String();
    return String::from(start, (u32)(end-start));
}

Vec<String> String::lines() const {
#ifdef _WIN32
    return this->split_str(String::from("\r\n"));
#else
    return this->split('\n');
#endif
}

_NO_DISCARD_ _FORCE_INLINE_
bool String::operator==(const String& other) const {
    _STRING_CHAR_TYPE *s1 = this->raw();
    _STRING_CHAR_TYPE *s2 = other.raw();
    return !strcmp(s1, s2);
}

_NO_DISCARD_
Vec<String> String::split_str(const String& other) const {
    Vec<String> result;
    String current;

    for (u32 i=0; i<(this->len()-other.len()); i++) {
        String window = this->substr(i, other.len());
        if (window == other) {
            result.push_back(current);
            current = String();
            i += other.len() - 1;
            continue;
        } else {
            const _STRING_CHAR_TYPE current_char = (*this)[i];
            current.push_back(current_char);
        }
    }

    return result;
}

_NO_DISCARD_
String String::substr(u32 from, u32 len) const {
    if (from >= this->len() || from + len > this->len())
        return String();

    _STRING_CHAR_TYPE *data = (_STRING_CHAR_TYPE *)malloc(sizeof(_STRING_CHAR_TYPE) * len);
    memccpy(data, this->raw() + from, len + 1, sizeof(_STRING_CHAR_TYPE));
    data[len] = '\0';
    String result = String::from(data);
    return result;
}

_NO_DISCARD_
Vec<String> String::split(_STRING_CHAR_TYPE token) const {
    Vec<String> result;
    String current;
    for (_STRING_CHAR_TYPE c : *this) {
        if (c != token) {
            current.push_back(c);
        } else {
            result.push_back(current);
            current = String();
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
