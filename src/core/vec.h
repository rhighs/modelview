#ifndef CORE_VEC_H
#define CORE_VEC_H

#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>

#include "types.h"
#include "error.h"
#include "container.h"

#define ARRAY_RAW_LEN(__ARR) (sizeof(__ARR)/sizeof(__ARR[0]))

template<typename T>
struct Vec {
    Container<T> _c_data;

    T& operator[](u32 index);
    T operator[](u32 index) const;

    _FORCE_INLINE_ Vec() {}
    _FORCE_INLINE_ Vec(const T &value, u32 count);
    _FORCE_INLINE_ Vec(u32 count) : _c_data(count) {}
    _FORCE_INLINE_ ~Vec() {}
    inline void operator=(const Vec &from) {
        _c_data._raw_data = from._c_data._raw_data;
    }

    // returns a new Vec<T> with length set 0, and capacity set to sizeof(T) * count.
    // the allocated memory is initialised to init_v
    _NO_DISCARD_ static Vec<T> with_capacity(const T &init_v, u32 count);

    // returns a new Vec<T> with length set 0, and capacity set to sizeof(T) * count.
    // the allocated memory is initialised to 0
    _NO_DISCARD_ static Vec<T> with_capacity(u32 count);

    _FORCE_INLINE_ u32 len() const { return _c_data.len(); }
    void push_back(const T &value) { return _c_data.push_back(value); }
    _FORCE_INLINE_ void append(const Vec<T> &other) { _c_data.append(other._c_data); }
    T pop_back() { return _c_data.pop_back(); }

    _FORCE_INLINE_ _NO_DISCARD_ Vec<T> inline_zip(const Vec<T> &other);

    _NO_DISCARD_
    static Vec<T> from(T *data, u32 count);

    T* begin() const { return _c_data.raw(); }
    T* end() const { return _c_data.raw() + _c_data.len(); }

    _FORCE_INLINE_ void dealloc() { _c_data.dealloc(); }
};

template<typename T>
Vec<T> Vec<T>::with_capacity(u32 count) {
    Vec<T> result(count);
    result._c_data.clear();
    return result;
}

template<typename T>
Vec<T> Vec<T>::with_capacity(const T &init_v, u32 count) {
    Vec<T> result(count);
    result._c_data.reset(init_v);
    return result;
}

template<typename T>
Vec<T> Vec<T>::from(T *data, u32 count) {
    Vec<T> result(count);
    T *base = result._c_data.raw();
    memcpy(base, data, count * sizeof(T));

    *(result._c_data._len()) = count; 
    return result;
}

template<typename T>
T& Vec<T>::operator[](u32 index) {
    DEV_ASSERT(index < _c_data.len(), "index cannot be out of array bounds");
    return (_c_data.get_value(index));
}

template<typename T>
T Vec<T>::operator[](u32 index) const {
    const u32 current_len = _c_data.len();
    DEV_ASSERT(index < current_len, "index cannot be out of array bounds");
    return (_c_data.get_value(index));
}

template<typename T>
Vec<T> Vec<T>::inline_zip(const Vec<T> &other) {
    const u32 minlen = len() > other.len() ? other.len() : len();
    Vec<T> result(minlen * 2);

    u32 this_rlen = _c_data.len();
    u32 other_rlen =  other._c_data.len();
    for (u32 i=0; i<minlen; i++) {
        result.push_back(_c_data.pop_back());
        result.push_back(other.pop_back());
    }

    *(_c_data._len()) = this_rlen;
    *(other._c_data._len()) = other_rlen;
    return result;
}

template<typename T>
Vec<T>::Vec(const T &value, u32 count) {
    _c_data = Container<T>(count);
    T *base = _c_data.raw();
    DEV_ASSERT(base != nullptr, "container base must be initialized to perform an init operation");

    // set the length alrady to be used later,
    // this is only really used to perform container specific fills
    u32 *len_ptr = _c_data._len();
    *len_ptr = count; 

    for (u32 k=0; k<_c_data.len(); k++)
        memcpy(base++, &value, sizeof(T));
}

#endif // CORE_VEC_H

