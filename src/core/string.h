#ifndef CORE_STRING_H
#define CORE_STRING_H

#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "tuple.h"
#include "vec.h"
#include "container.h"

#define _STRING_CHAR_TYPE char
#define _STRING_GET_LEN_FUNC strlen
// assume all types end in '\0'
#define _STRING_CHAR_TYPE_TERM '\0'

struct String {
    b8 _is_literal = FALSE;
    Container<_STRING_CHAR_TYPE> _c_data;

    template<u32 N>
    constexpr String(const _STRING_CHAR_TYPE (&string)[N]) {
        _c_data._raw_data = (_STRING_CHAR_TYPE*)string;
        _is_literal = TRUE;
        // *(_c_data._len()) = N;
    }

    _FORCE_INLINE_ String(String &&s) {
        this->_c_data._raw_data = s._c_data._raw_data;
        s._c_data._raw_data = nullptr;
    }
    _FORCE_INLINE_ void operator=(String &&s) {
        this->_c_data = s._c_data;
        s._c_data._raw_data = nullptr;
    }

    _FORCE_INLINE_ String(const String &s) { this->_c_data = s._c_data.clone(); }
    _FORCE_INLINE_ void operator=(const String &s) { this->_c_data = s._c_data.clone(); }

    _FORCE_INLINE_ String(u32 count = 4) : _c_data(count) {}
    _FORCE_INLINE_ ~String() { _c_data.dealloc(); }

    _FORCE_INLINE_ _NO_DISCARD_
    String operator+(const String &other) const { 
        String result;
        result.append(*this); result.append(other);
        return result;
    };

    _FORCE_INLINE_ void _own_literal() { DEV_ASSERT(this->_is_literal, "must be literal in order to own it!"); (*this) = String::copy_from(this->raw()); }

    _NO_DISCARD_ String replace(const String& str_a, const String& str_b) const;
    _NO_DISCARD_ bool contains(const String& str) const;

    _FORCE_INLINE_ _NO_DISCARD_ bool operator==(const String &other) const;
    _FORCE_INLINE_ void operator+=(const String &other) { _c_data.append(other._c_data); }
    _FORCE_INLINE_ void operator+=(_STRING_CHAR_TYPE value) { _c_data.push_back(value); }
    _FORCE_INLINE_ _STRING_CHAR_TYPE operator[](u32 at) const { return _c_data.get_value(at); };

    _FORCE_INLINE_ void push_back(_STRING_CHAR_TYPE value) { if (_is_literal) _own_literal(); _c_data.push_back(value); };
    _FORCE_INLINE_ void append(const String& other) { if (_is_literal) _own_literal(); _c_data.append(other._c_data); }
    _FORCE_INLINE_ void clear() { if (_is_literal) return; _c_data.reset(_STRING_CHAR_TYPE_TERM); }

    _NO_DISCARD_ _STRING_CHAR_TYPE* raw() const;
    _NO_DISCARD_ Vec<String> split(_STRING_CHAR_TYPE token) const;
    _NO_DISCARD_ Vec<String> split_str(const String &other) const;
    _NO_DISCARD_ String substr(u32 from, u32 len) const;

    _NO_DISCARD_ static String copy_from(const _STRING_CHAR_TYPE* c_string);
    _NO_DISCARD_ static String copy_from(const _STRING_CHAR_TYPE* c_string, u32 len);

    _NO_DISCARD_ bool starts_with(const String& other) const;
    _NO_DISCARD_ bool starts_with(const _STRING_CHAR_TYPE *c_string) const;
    _FORCE_INLINE_ u32 len() const { return _is_literal ? strlen(this->_c_data._raw_data) : _c_data.len(); }
    _NO_DISCARD_ String strip(const _STRING_CHAR_TYPE strip_ch = ' ') const;

    _NO_DISCARD_ Pair<f32, b8> to_f32() const;

    Vec<String> lines() const;

    _STRING_CHAR_TYPE* begin() const { return _c_data.raw(); }
    _STRING_CHAR_TYPE* end() const { return _c_data.raw() + _c_data.len(); }

    _NO_DISCARD_ static
    String with_capacity(u32 count) {
        String result(count);
        result._c_data.clear();
        return result;
    }
};

#endif // CORE_STRING_H
