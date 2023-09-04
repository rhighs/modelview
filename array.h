#ifndef ARRAY_H
#define ARRAY_H

#include "types.h"

template<typename T>
struct Array {
    T* operator[](u32 index);

    T *__base;
    u32 __capacity;
    u32 __len;
};

template<typename T>
u32 len(Array<T> *arr);

template<typename T>
u32 capacity(Array<T> *arr);

template<typename T>
void init(Array<T> *arr, u32 count);

template<typename T>
void realloc(Array<T> *arr);

template<typename T>
void push(Array<T> *arr, T value);

template<typename T>
T pop(Array<T> *arr);

#endif

