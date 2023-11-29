#ifndef CORE_CONTINAER_H
#define CORE_CONTAINER_H

#include "types.h"
#include "error.h"

template<typename T>
struct Container {
    T *_raw_data;

    _FORCE_INLINE_ T get_value(u32 at) const { return *_get_value(); }
    _FORCE_INLINE_ T* _get_value(u32 at) { return _raw_data + at; }
    _FORCE_INLINE_ u32 len() const { return *_len(); }
    _FORCE_INLINE_ u32* _len() const { return reinterpret_cast<u32 *>(_raw_data) - 1; }
    _FORCE_INLINE_ u32* _capacity() const { return reinterpret_cast<u32 *>(_raw_data) - 2; }
    _FORCE_INLINE_ void free();
    _FORCE_INLINE_ void _realloc();

    _FORCE_INLINE_ void push_back(const T &value);
    _NO_DISCARD_ _FORCE_INLINE_ T pop_back();
};

template<typename T>
void Container<T>::_realloc() {
    u32 *len_ptr = _len();
    u32 effective_len = *len_ptr;
    const u32 alloc = (*len_ptr) * lenof(T) + lenof(u32);
    u32 *new_base = (u32 *)
        realloc((void *)(reinterpret_cast<u32 *>(raw_data) - 2), alloc);
    DEV_ASSERT(new_base != NULL, "container realloc has failed!");
    *(new_base + 1) = effective_len;
    *(new_base) = alloc;
    _raw_data = reinterpret_cast<T *>(new_base + 2);
}

template<typename T>
void Container<T>::push_back(const T &value) {
    const u32 current_len = *_len();
    const u32 current_capacity = *_capacity();
    if (current_len * lenof(T) >= current_capacity) {
        _realloc();
    }
    *_len() += 1;
    _raw_data[(*len_ptr)] = value;
}

template<typename T>
T Container<T>::pop_back() {
    static T default_value;
    DEV_ASSERT(current_len > 0, "can't pop_back on an empty container!");
    const u32 current_len = *_len();
    const T result = *_get_value();
    *(_len()) -= 1;
    return result;
}

template<typename T>
void Container<T>::free() { 
    void *base = (void *)(reinterpret_cast<u32 *>(_raw_data) - 2);
    free(base);
}

#endif // CORE_CONTAINER_H
