#ifndef CORE_CONTAINER_H
#define CORE_CONTAINER_H

#include <stdlib.h>

#include "types.h"
#include "error.h"

template<typename T>
struct Container {
    T *_raw_data;

    Container(u32 count = 4);

    _FORCE_INLINE_ _NO_DISCARD_ T* raw() const { return _raw_data; }
    _FORCE_INLINE_ T* _get_value(u32 at) const { return _raw_data + at; }
    _FORCE_INLINE_ u32* _len() const { return reinterpret_cast<u32 *>(_raw_data) - 1; }
    _FORCE_INLINE_ u32* _capacity() const { return reinterpret_cast<u32 *>(_raw_data) - 2; }
    _FORCE_INLINE_ void _realloc();
    _FORCE_INLINE_ void _realloc_for(u32 reach_capacity);

    _FORCE_INLINE_ void dealloc();
    _FORCE_INLINE_ u32 len() const { return *_len(); }
    _FORCE_INLINE_ T& get_value(u32 at) const { return *reinterpret_cast<T *>(_get_value(at)); }

    _FORCE_INLINE_ void push_back(const T &value);
    _FORCE_INLINE_ void append(const Container<T> &other);
    _NO_DISCARD_ _FORCE_INLINE_ T pop_back();
};

template<typename T>
void Container<T>::_realloc() {
    u32 current_len = *(_len());
    u32 current_capacity = *(_capacity());
    const u32 alloc = current_capacity * 2;
    const u32 meta_alloc = sizeof(u32) * 2;
    u32 *new_base = (u32 *)
        realloc((void *)(reinterpret_cast<u32 *>(_raw_data) - 2), meta_alloc + alloc);
    DEV_ASSERT(new_base != NULL, "container realloc has failed!");
    _raw_data = reinterpret_cast<T *>(new_base + 2);
    *(_len()) = current_len;
    *(_capacity()) = alloc;
}

template<typename T>
void Container<T>::_realloc_for(u32 reach_capacity) {
    u32 current_capacity = *(_capacity());
    // check if capacity is met already
    if (reach_capacity <= current_capacity)
        return;

    u32 current_len = *(_len());
    const u32 alloc = reach_capacity;
    const u32 meta_alloc = sizeof(u32) * 2;
    u32 *new_base = (u32 *)
        realloc((void *)(reinterpret_cast<u32 *>(_raw_data) - 2), meta_alloc + alloc);
    DEV_ASSERT(new_base != NULL, "container realloc has failed!");
    _raw_data = reinterpret_cast<T *>(new_base + 2);
    *(_len()) = current_len;
    *(_capacity()) = alloc;
}

template<typename T>
void Container<T>::push_back(const T &value) {
    const u32 current_len = *_len();
    const u32 current_capacity = *_capacity();
    if (current_len * sizeof(T) >= current_capacity) {
        _realloc();
    }
    *_len() += 1;
    _raw_data[*_len()] = value;
}

template<typename T>
T Container<T>::pop_back() {
    static T default_value;
    DEV_ASSERT(current_len > 0, "can't pop_back on an empty container!");
    const T result = *_get_value();
    *(_len()) -= 1;
    return result;
}

template<typename T>
void Container<T>::append(const Container<T> &other) {
    u32 current_capacity = *(_capacity());
    _realloc_for(current_capacity + *(other._capacity()));
    memcpy(_raw_data + *(_len()), other._raw_data, sizeof(T) * other.len());
}

template<typename T>
void Container<T>::dealloc() {
    void *base = (void *)(reinterpret_cast<u32 *>(_raw_data) - 2);
    free(base);
}

template <typename T>
Container<T>::Container(u32 count) {
    const u32 alloc = count * sizeof(T);
    const u32 meta_alloc = sizeof(u32) * 2;
    T *raw_data = (T *)malloc(meta_alloc + alloc);
    DEV_ASSERT(raw_data != null, "this must be able to allocate on init");
    _raw_data = raw_data;
    *(_len()) = 0;
    *(_capacity()) = alloc;
}

#endif // CORE_CONTAINER_H
