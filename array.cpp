#include "assert.h"
#include "stdlib.h"
#include "memory.h"
#include "stdio.h"
#include "array.h"

template <typename T>
T* Array<T>::operator[](u32 index) {
    assert(index < __len);
    return __base[index];
}

template<typename T>
u32 len(Array<T> *arr) {
    return arr->__len;
}

template<typename T>
u32 capacity(Array<T> *arr) {
    return arr->__capacity;
}

template<typename T>
void init(Array<T> *arr, u32 count) {
    const u32 capacity = sizeof(T) * count;
    arr->__base = (T *)malloc(capacity);
    memset(arr->__base, 0, capacity);

    arr->__capacity = capacity;
    arr->__len = 0;
}

template<typename T>
void realloc(Array<T> *arr) {
    const u32 new_capacity = (arr->__len+arr->__len/2) * sizeof(T);
    T *base = (T *)realloc(arr->__base, new_capacity);
    if (base == NULL) {
        fprintf(stderr, "[%s:%s:%d] Failed array realloc, not enough memory", __FILE_NAME__, __FUNCTION__, __LINE__);
    }
    arr->__base = base;
    arr->__capacity = new_capacity;
}

template<typename T>
void push(Array<T> *arr, T value) {
    if (arr->__len * sizeof(T) >= arr->__capacity) {
        realloc(arr);
    }
    arr->__len++;
    (*arr)[arr->__len-1] = value;
}

template<typename T>
T pop(Array<T> *arr) {
    const T value = arr->__base[arr->__len-1];
    arr->__len--;
    return value;
}
