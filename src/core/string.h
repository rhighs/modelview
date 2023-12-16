#ifndef CORE_STRING_H
#define CORE_STRING_H

#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "vec.h"
#include "container.h"

#define _STRING_CHAR_TYPE char
#define _STRING_GET_LEN_FUNC strlen

struct String {
    Container<_STRING_CHAR_TYPE> _c_data;

    String() { _c_data = Container<_STRING_CHAR_TYPE>(4); }
    ~String() { _c_data.dealloc(); }

    _FORCE_INLINE_ _NO_DISCARD_
    String operator+(const String &other) const { 
        String result;
        result.append(*this); result.append(other);
        return result;
    };

    void operator+=(const String &other) { _c_data.append(other._c_data); }
    void operator+=(_STRING_CHAR_TYPE value) { _c_data.push_back(value); }
    _STRING_CHAR_TYPE operator[](u32 at) const { return _c_data.get_value(at); };

    _FORCE_INLINE_ void push_back(_STRING_CHAR_TYPE value) { _c_data.push_back(value); };

    _NO_DISCARD_ _STRING_CHAR_TYPE* raw() const;
    _NO_DISCARD_ Vec<String> split(_STRING_CHAR_TYPE token) const;
    _NO_DISCARD_ static String from(const _STRING_CHAR_TYPE *c_string);

    _FORCE_INLINE_ u32 len() const { return _c_data.len(); }
    _FORCE_INLINE_ void append(const String &other) { _c_data.append(other._c_data); };

    _STRING_CHAR_TYPE* begin() const { return _c_data.raw(); }
    _STRING_CHAR_TYPE* end() const { return _c_data.raw() + _c_data.len(); }
};

#endif // CORE_STRING_H
