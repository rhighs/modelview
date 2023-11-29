#ifndef ARRAY_H
#define ARRAY_H

#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>

#include "types.h"
#include "error.h"
#include "container.h"

#define RAW_ARRAY_LEN(__ARR) (sizeof(__ARR)/sizeof(__ARR[0]))

template<typename T>
struct Array {
    Container<T> _c_data;

    T& operator[](u32 index);

    ~Array() { _c_data.free() }

    _FORCE_INLINE_ u32 len() const { return _c_data.len(); }
    void push_back(const T &value) { return _c_data.push_back(value); }
    T pop_back() { return _c_data.pop_back(); }
}

template<typename T>
Array<T>::operator[](u32 index) {
    ASSERT(index < _c_data.len(), "index cannot be out of array bounds");
    return (_c_data.get_value(index));
}

template<typename T>
struct Array_ {
    T *_raw_data;
    u32 _capacity;
    u32 _len;

    T& operator[](u32 index);
    ~Array_();

    Array_(u32 count) { array_init(this, count); }
    Array_(const T &value, u32 count) { array_fill_with(this, value, count); }

    _FORCE_INLINE_ u32 len() { return len; }
    _FORCE_INLINE_ u32 capacity() { return this->capacity; }
    _FORCE_INLINE_ void fill();
    _FORCE_INLINE_ void from(T *raw_data, u32 count);
};

template<typename T>
_FORCE_INLINE_ void Array_<T>::fill() {
    for (u32 i=0; i<arr->_len; i++)
        this->_raw_data[i] = value;
}

template<typename T>
_FORCE_INLINE_ void Array_<T>::from(T *raw_data, u32 count) {
}

template <typename T>
T& Array_<T>::operator[](u32 index) {
    assert(index < len && "Array_ out of bounds access");
    return data[index];
}

/*
===================== CLIKE INTERFACES =====================
*/

template<typename T>
u32 array_len(Array_<T> *arr) {
    return arr->len;
}

template<typename T>
u32 array_capacity(Array_<T> *arr) {
    return arr->capacity;
}

template<typename T>
void array_init(Array_<T> *arr, u32 count) {
    const u32 capacity = sizeof(T) * count;
    arr->data = (T *)malloc(capacity);
    memset(arr->data, 0, capacity);

    arr->capacity = capacity;
    arr->len = 0;
}

template<typename T>
void array_init_with(Array_<T> *arr, T value, u32 count) {
    const u32 capacity = sizeof(T) * count;
    arr->data = (T *)malloc(capacity);
    memset(arr->data, 0, capacity);
    for (u32 i=0; i<count; i++)
        arr->data[i] = value;

    arr->capacity = capacity;
    arr->len = 0;
}

template<typename T>
void array_fill_with(Array_<T> *arr, T value, u32 count) {
    const u32 needed = sizeof(T) * count;
    if (arr->capacity < needed) {
        array_realloc(arr);
    }

    memset(arr->data, 0, arr->capacity);
    for (u32 i=0; i<count; i++)
        arr->data[i] = value;
    arr->len = count;
}

template<typename T>
void array_fill(Array_<T> *arr, T value) {
    for (u32 i=0; i<arr->len; i++)
        arr->data[i] = value;
}

template<typename T>
void array_realloc(Array_<T> *arr) {
    const u32 new_capacity = (arr->len+arr->len/2) * sizeof(T);
    T *base = (T *)realloc((void *)(arr->data), new_capacity);
    if (base == NULL) {
        fprintf(stderr, "[%s:%s:%d] Failed array realloc, not enough memory", __FILE_NAME__, __FUNCTION__, __LINE__);
    }
    arr->data = base;
    arr->capacity = new_capacity;
}

template<typename T>
void array_realloc_for(Array_<T> *arr, u32 capacity) {
    if (arr->capacity >= capacity) {
        return;
    }
    T *base = (T *)realloc((void *)(arr->data), capacity);
    if (base == NULL) {
        fprintf(stderr, "[%s:%s:%d] Failed array realloc, not enough memory", __FILE_NAME__, __FUNCTION__, __LINE__);
    }
    arr->data = base;
    arr->capacity = capacity;
}

template<typename T>
void array_push(Array_<T> *arr, T value) {
    if (arr->len * sizeof(T) >= arr->capacity) {
        array_realloc(arr);
    }
    arr->len++;
    arr->data[arr->len-1] = value;
}

template<typename T>
T array_pop(Array_<T> *arr) {
    const T value = arr->data[arr->len-1];
    arr->len--;
    return value;
}

template<typename T>
void array_reset(Array_<T> *arr) {
    arr->len = 0;
}

template<typename T>
void array_free(Array_<T> *arr) {
    arr->len = 0;
    arr->capacity = 0;
    free(arr->data);
}

template<typename T>
u32 array_count(Array_<T> *arr, T countme) {
    u32 result = 0;
    for (u32 i=0; i<arr->len; i++)
        if (arr->data[i] == countme) result++;
    return result;
}

template<typename T>
Array_<T> array_zip(Array_<T> a, Array_<T> b) {
    Array_<T> result;
    u32 minl = a.len > b.len ? b.len : a.len;
    array_init(&result, minl * 2);
    for (u32 i=0; i<minl; i++) {
        array_push(&result, array_pop(&a));
        array_push(&result, array_pop(&b));
    }
    return result;
}

template<typename T>
Array_<T> array_from_copy(T *data, u32 len) {
    assert(len > 0 && "Wym copy from length = 0 ??");
    Array_<T> result;
    result.len = len;
    const u32 capacity = sizeof(T) * len;
    result.data = (T *)malloc(capacity);
    result.capacity = capacity;
    memcpy(result.data, data, sizeof(data[0]) * len);
    return result;
}

template<typename T>
Array_<T> array_from(T *data, u32 len) {
    Array_<T> result;
    result.len = len;
    const u32 capacity = sizeof(T) * len;
    result.data = data;
    result.capacity = capacity;
    return result;
}

template<typename T>
void array_append(Array_<T> *dst, Array_<T> src) {
    if (src.len == 0) {
        return;
    }

    array_realloc_for(dst, dst->capacity + (src.len * sizeof(T)));
    memcpy((u8 *)(dst->data + dst->len), (u8 *)src.data, sizeof(T) * src.len);
}

template<typename T>
void array_print(Array_<T> *arr) {};
#endif

