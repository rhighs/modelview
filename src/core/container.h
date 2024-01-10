#ifndef CORE_CONTAINER_H
#define CORE_CONTAINER_H

#include <stdlib.h>

#include "types.h"
#include "error.h"


#define CONTAINER_DEFAULT_ALLOC 4

template<typename T>
struct Container {
    u32 _default_alloc_size = CONTAINER_DEFAULT_ALLOC;
    T *_raw_data = NULL;

    Container(u32 count = 4) { _default_alloc_size = count; if (_default_alloc_size > 0) _alloc(_default_alloc_size); };
    Container(T *raw_data) : _raw_data(raw_data) {};
    void alloc() { _alloc(_default_alloc_size); };
    void _alloc(u32 count);

    _FORCE_INLINE_ _NO_DISCARD_ T* raw() const { return _raw_data; }
    _FORCE_INLINE_ T* _get_value(u32 at) const { return _raw_data + at; }
    _FORCE_INLINE_ u32* _len() const { return reinterpret_cast<u32 *>(_raw_data) - 1; }
    _FORCE_INLINE_ u32* _capacity() const { return reinterpret_cast<u32 *>(_raw_data) - 2; }
    void _realloc();
    void _realloc_for(u32 reach_capacity);

    _NO_DISCARD_ Container<T> clone() const;

    void dealloc();
    _FORCE_INLINE_ _NO_DISCARD_ u32 len() const { return _raw_data != NULL ? *_len() : 0; }
    _FORCE_INLINE_ _NO_DISCARD_ T& get_value(u32 at) const { return *reinterpret_cast<T *>(_get_value(at)); }

    _FORCE_INLINE_ void push_back(const T &value);
    _FORCE_INLINE_ _NO_DISCARD_ T pop_back();
    void append(const T *data, u32 len);

    u32 __get_min_realloc_size(u32 realloc_factor = 2) const;

    // sets all allocated memory to 0 from ((u32 *) raw_data) - 2.
    // capacity will be kept, but len will be reset to 0
    void clear();

    // same as clear, but sets memory to reset_value
    void reset(const T& reset_value);
};

template<typename T>
void Container<T>::clear() {
    if (_raw_data == NULL)
        return;

    const u32 current_capacity = *(_capacity());
    if (!current_capacity) return;
    const u32 alloc_capacity = current_capacity + sizeof(u32) * 2;

    u32* base_ptr = reinterpret_cast<u32*>(_raw_data) - 2;
    u8* base_bytes_ptr = reinterpret_cast<u8*>(base_ptr);
    memset(base_bytes_ptr, 0, alloc_capacity);
    *(_capacity()) = current_capacity;
}

template<typename T>
void Container<T>::reset(const T& value) {
    const u32 current_capacity = *(_capacity());
    if (!current_capacity) return;

    u32* base_ptr = reinterpret_cast<u32*>(_raw_data) - 2;
    u8* base_bytes_ptr = reinterpret_cast<u8*>(base_ptr);
    const u8* t_value_data = reinterpret_cast<const u8*>(&value);
    u32 t_value_size = sizeof(T);
    for (u32 i = 0; i < current_capacity; i+=t_value_size) {
        for (u32 k = 0; k < t_value_size; k++)
            base_bytes_ptr[i + k] = t_value_data[k];
    }
    *(_capacity()) = current_capacity;
}

template<typename T>
u32 Container<T>::__get_min_realloc_size(u32 realloc_factor) const {
    const u32 current_capacity = *(_capacity());
    u32 realloc_size = current_capacity * realloc_factor;
    if (realloc_size < sizeof(T) * 2) {
        realloc_size = sizeof(T) * 2;
    }
    return realloc_size;
}

template<typename T>
void Container<T>::_realloc() {
    if (_raw_data == NULL) {
        _alloc(_default_alloc_size); 
    }

    u32 current_len = *(_len());
    u32 alloc = __get_min_realloc_size();
    const u32 meta_alloc = sizeof(u32) * 2;
    u32 *current_base = reinterpret_cast<u32*>(_raw_data) - 2;
    if ((current_base = (u32 *)realloc((void*)current_base, meta_alloc + alloc)) == NULL) {
        IO_LOG(stderr, "realloc has failed", NULL);
        exit(1);
    }

    T *raw_data = reinterpret_cast<T *>(reinterpret_cast<u32*>(current_base) + 2);
    _raw_data = raw_data;
    *(_len()) = current_len;
    *(_capacity()) = alloc;
}

template<typename T>
void Container<T>::_realloc_for(u32 reach_capacity) {
    if (_raw_data == NULL) {
        _alloc(reach_capacity / sizeof(T)); 
    }

    u32 current_capacity = *(_capacity());
    // check if capacity is met already
    if (reach_capacity <= current_capacity)
        return;

    u32 current_len = *(_len());
    const u32 alloc = reach_capacity;
    const u32 meta_alloc = sizeof(u32) * 2;
    u32* current_base = reinterpret_cast<u32*>(_raw_data) - 2;
    if ((current_base = (u32 *)realloc((void*)current_base, meta_alloc + alloc)) == NULL) {
        IO_LOG(stderr, "realloc has failed", NULL);
        exit(1);
    }

    T *raw_data = reinterpret_cast<T *>(reinterpret_cast<u32*>(current_base) + 2);
    _raw_data = raw_data;
    *(_len()) = current_len;
    *(_capacity()) = alloc;
}

template<typename T>
void Container<T>::push_back(const T &value) {
    const u32 current_len = *_len();
    const u32 current_capacity = *_capacity();
    const u32 t_value_size = sizeof(T);
    if (current_len * t_value_size >= current_capacity) {
        _realloc();
    }
    *_get_value(current_len) = value;
    *_len() = current_len + 1;
}

template<typename T>
T Container<T>::pop_back() {
    static T default_value;
    const u32 current_len = *(_len());
    DEV_ASSERT(current_len > 0, "can't pop_back on an empty container!");
    const T result = *_get_value();
    *(_len()) = current_len - 1;
    if (current_len == 1) {
        dealloc();
    }
    return result;
}

template<typename T>
void Container<T>::append(const T *data, u32 len) {
    if (len == 0) return;

    u32 current_len = *(_len());
    u32 copy_n_bytes = len * sizeof(T);
    u32 current_capacity = *(_capacity());
    u32 realloc_for_bytes = current_capacity + copy_n_bytes;
    _realloc_for(realloc_for_bytes);

    // u8 *copy_dst_ptr = &(((u8 *)_raw_data)[current_len]);
    u8 *copy_dst_ptr = reinterpret_cast<u8 *>(_raw_data + current_len);
    u8 *copy_src_ptr = (u8 *)(data);
    memcpy(copy_dst_ptr, copy_src_ptr, copy_n_bytes);
    *(_len()) = current_len + len;
}

template<typename T>
void Container<T>::dealloc() {
    if (this->_raw_data != NULL) {
        u8* base = reinterpret_cast<u8 *>(reinterpret_cast<u32 *>(_raw_data) - 2);
        free(base);
        _raw_data = NULL;
    }
}

template <typename T>
void Container<T>::_alloc(u32 count) {
    const u32 alloc = count * sizeof(T);
    const u32 meta_alloc = sizeof(u32) * 2;
    void *malloc_result;
    if ((malloc_result = malloc(meta_alloc + alloc)) == NULL) {
        IO_LOG(stderr, "malloc failed, exiting...", NULL);
        exit(1);
    }

    u32* base_ptr = reinterpret_cast<u32*>(malloc_result);
    // make space for meta_data ensured by "meta_alloc"
    T* raw_data = reinterpret_cast<T*>(base_ptr + 2);
    _raw_data = raw_data;
    *(_len()) = 0;
    *(_capacity()) = alloc;
}

template <typename T>
Container<T> Container<T>::clone() const {
    if (_raw_data == NULL || *(_capacity()) == 0)
        return Container<T>();

    u32 current_capacity = *(_capacity());
    u32 meta_alloc = sizeof(u32) * 2;

    void *malloc_result = malloc(current_capacity + meta_alloc);
    u32* base_ptr = reinterpret_cast<u32*>(malloc_result);
    T *copy_raw_data = reinterpret_cast<T *>(base_ptr + 2);
    memcpy(copy_raw_data, _raw_data, current_capacity);
    Container<T> result(copy_raw_data);
    *(result._len()) = *(_len());
    *(result._capacity()) = current_capacity;
    return result;
}

#endif // CORE_CONTAINER_H
