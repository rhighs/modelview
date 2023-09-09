#ifndef ARRAY_H
#define ARRAY_H

#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <sys/types.h>

#include "types.h"

template<typename T>
struct Array {
    T operator[](u32 index);

    T *data;
    u32 capacity;
    u32 len;
};

template <typename T>
T Array<T>::operator[](u32 index) {
    assert(index < len && "Array out of bounds access");
    return data[index];
}

template<typename T>
u32 array_len(Array<T> *arr) {
    return arr->len;
}

template<typename T>
u32 array_capacity(Array<T> *arr) {
    return arr->capacity;
}

template<typename T>
void array_init(Array<T> *arr, u32 count) {
    const u32 capacity = sizeof(T) * count;
    arr->data = (T *)malloc(capacity);
    memset(arr->data, 0, capacity);

    arr->capacity = capacity;
    arr->len = 0;
}

template<typename T>
void array_init_with(Array<T> *arr, T value, u32 count) {
    const u32 capacity = sizeof(T) * count;
    arr->data = (T *)malloc(capacity);
    memset(arr->data, 0, capacity);
    for (u32 i=0; i<count; i++)
        arr->data[i] = value;

    arr->capacity = capacity;
    arr->len = 0;
}

template<typename T>
void array_realloc(Array<T> *arr) {
    const u32 new_capacity = (arr->len+arr->len/2) * sizeof(T);
    T *base = (T *)realloc((void *)(arr->data), new_capacity);
    if (base == NULL) {
        fprintf(stderr, "[%s:%s:%d] Failed array realloc, not enough memory", __FILE_NAME__, __FUNCTION__, __LINE__);
    }
    arr->data = base;
    arr->capacity = new_capacity;
}

template<typename T>
void array_push(Array<T> *arr, T value) {
    if (arr->len * sizeof(T) >= arr->capacity) {
        array_realloc(arr);
    }
    arr->len++;
    arr->data[arr->len-1] = value;
}

template<typename T>
T array_pop(Array<T> *arr) {
    const T value = arr->data[arr->len-1];
    arr->len--;
    return value;
}

template<typename T>
void array_reset(Array<T> *arr) {
    arr->len = 0;
}

template<typename T>
void array_free(Array<T> *arr) {
    arr->len = 0;
    arr->capacity = 0;
    free(arr->data);
}

template<typename T>
u32 array_count(Array<T> *arr, T countme) {
    u32 result = 0;
    for (u32 i=0; i<arr->len; i++)
        if (arr->data[i] == countme) result++;
    return result;
}

template<typename T>
Array<T> array_zip(Array<T> a, Array<T> b) {
    Array<T> result;
    u32 minl = a.len > b.len ? b.len : a.len;
    array_init(&result, minl * 2);
    for (u32 i=0; i<minl; i++) {
        array_push(&result, array_pop(&a));
        array_push(&result, array_pop(&b));
    }
    return result;
}

#endif

