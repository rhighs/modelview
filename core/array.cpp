#include "array.h"

#include <stdio.h>
#include "types.h"

template<typename T>
Array<T>::~Array() { free(this->data); }

#define DEF_ARR_T_SPEC(TYPE, FMT)\
template<>\
void array_print(Array<TYPE> *arr) {\
    fprintf(stdout, "[\n");\
    u32 j = 0;\
    for (u32 i=0; i<arr->len; i++) {\
        if (j == 0) fprintf(stdout, "   ");\
        fprintf(stdout, " " FMT ", ", (*arr)[i]);\
        j++;\
        if (j == 10) { j = 0; fprintf(stdout, "\n"); }\
    }\
    fprintf(stdout, "]\n");\
}\

DEF_ARR_T_SPEC(f32,     "%f")
DEF_ARR_T_SPEC(f64,     "%f")
DEF_ARR_T_SPEC(u8,      "%d")
DEF_ARR_T_SPEC(u16,     "%d")
DEF_ARR_T_SPEC(u32,     "%d")
DEF_ARR_T_SPEC(u64,     "%lld")

