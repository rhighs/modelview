#include "string.h"
#include "container.h"
#include "types.h"
#include "vec.h"

#include <cstring>
#include <stdio.h>
#include <memory.h>
#include <assert.h>

_NO_DISCARD_
bool String::starts_with(const String &other) const {
    String substr = this->substr(0, other.len());
    return substr == other;
}

_NO_DISCARD_
bool String::starts_with(const _STRING_CHAR_TYPE *c_string) const {
    u32 len = strlen(c_string);
    if (len > this->len()) return false;

    for (u32 k=0; k<len; k++)
        if (c_string[k] != (*this)[k]) return false;

    return true;
}

_NO_DISCARD_
String String::copy_from(const _STRING_CHAR_TYPE *c_string) {
    u32 c_len = _STRING_GET_LEN_FUNC(c_string);
    if (c_len > 0) {
        String result(c_len);
        strcpy(result._c_data._raw_data, c_string);
        *(result._c_data._len()) = c_len;
        return result;
    }
    return String();
}

String String::copy_from(const _STRING_CHAR_TYPE *c_string, u32 len) {
    String result;
    u32 c_len = _STRING_GET_LEN_FUNC(c_string);
    if (c_len > 0) {
        result._c_data = Container<_STRING_CHAR_TYPE>(c_len + 1);
        memcpy(result._c_data._raw_data, c_string, len * sizeof(_STRING_CHAR_TYPE));
        *(result._c_data._len()) = c_len;
    }
    return result;
}

String String::strip(const _STRING_CHAR_TYPE strip_ch) const {
    _STRING_CHAR_TYPE *start = _c_data.raw();
    while (*start == strip_ch) start++;
    _STRING_CHAR_TYPE *end = _c_data.raw() + len();
    while (*end == strip_ch) end--;
    String result;
    if (start >= end) {
        result = String();
    } else {
        u32 len = ((u32)(end-start));
        result = String::with_capacity(len);
        memcpy(result._c_data._raw_data, start, len * sizeof(_STRING_CHAR_TYPE));
        *(result._c_data._len()) = len;
    }
    return result;
}

Vec<String> String::lines() const {
    if (this->contains(String("\r\n"))) {
        return this->split_str(String("\r\n"));
    } else {
        return this->split('\n');
    }
}

_NO_DISCARD_
b8 String::operator==(const String& other) const {
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
    DEV_ASSERT(len > 0 && from <= this->len() && from + len < this->len(), "invalid substr params: out bounds");

    _STRING_CHAR_TYPE *data = (_STRING_CHAR_TYPE *)malloc(sizeof(_STRING_CHAR_TYPE) * len);
    memcpy(data, this->_c_data._raw_data + from, sizeof(_STRING_CHAR_TYPE) * len);
    String result(len);
    memcpy(result._c_data._raw_data, data, len * sizeof(_STRING_CHAR_TYPE));
    *(result._c_data._len()) = len;
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
            current.clear();
        }
    }
    result.push_back(current);
    return result;
}

// sets a null character at the end of the internal container
_STRING_CHAR_TYPE* String::raw() const {
    /* FIXME: this could happen, inore it for now
    {
        u32 potential_max_len = *(_c_data._capacity()) * sizeof(_STRING_CHAR_TYPE);
        len() == potential_max_len
    }
    */
    if (_is_literal == FALSE) {
        _STRING_CHAR_TYPE* end = _c_data.raw() + *(_c_data._len());
        *(end) = _STRING_CHAR_TYPE_TERM;
    }
   return _c_data.raw();
}

_NO_DISCARD_
Pair<f32, b8> String::to_f32() const {
    DEV_ASSERT(sizeof(_STRING_CHAR_TYPE) == 1, "string must be made of C characters");

    char *str = this->raw();
    f32 result = strtof(str, NULL);
    if (result == 0.0f)
        return { 0.0f, false };

    return { result, true };
}

_NO_DISCARD_
String String::replace(const String &str_a, const String &str_b) const {
    String result;
    u32 i=0;
    String window;
    for (;i<this->len()-str_a.len(); i++) {
        window = this->substr(i, str_a.len());
        if (window == str_a) {
            result += str_b;
            i += str_a.len();
            continue;
        } else {
            const _STRING_CHAR_TYPE current_char = (*this)[i];
            result += current_char;
        }
    }
    u32 current_len = this->len();
    result += this->substr(i, current_len - i);
    return result;
}

_NO_DISCARD_
b8 String::contains(const String &str) const {
    for (u32 i=0; i<(this->len()-str.len()); i++) {
        String window = this->substr(i, str.len());
        if (window == str)
            return TRUE;
    }
    return FALSE;
}

