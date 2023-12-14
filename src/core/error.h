#ifndef CORE_ERROR
#define CORE_ERROR

#include <stdio.h>

#define IO_LOG(STREAM, FMT, ...)\
        do{fprintf(STREAM, "[%s:%s:%d]: " FMT "\n", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);}while(0)

#ifdef DEV_ENABLED
#define DEV_ASSERT(COND, MSG)                                               \
    if (!(COND)) {                                                          \
        IO_LOG(stderr, "assertion = %s failed, message = %s", COND, MSG);   \
    } else ((void)(0))                                                 
#endif
#else
#define DEV_ASSERT(COND, MSG) ((void)(0))
#endif
