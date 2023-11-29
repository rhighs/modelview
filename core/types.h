#ifndef TYPES_H
#define TYPES_H

// Should always inline no matter what.
#ifndef _ALWAYS_INLINE_
#if defined(__GNUC__)
#define _ALWAYS_INLINE_ __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#define _ALWAYS_INLINE_ __forceinline
#else
#define _ALWAYS_INLINE_ inline
#endif
#endif

// Should always inline, except in dev builds because it makes debugging harder.
#ifndef _FORCE_INLINE_
#ifdef DEV_ENABLED
#define _FORCE_INLINE_ inline
#else
#define _FORCE_INLINE_ _ALWAYS_INLINE_
#endif
#endif

#ifndef _NO_DISCARD_
#if defined(_MSC_VER)
#define _NO_DISCARD_ _Check_return_
#elif defined(__GNUC__)
#define _NO_DISCARD_ [[nodiscard]]
#else
#define _NO_DISCARD_ __attribute__((warn_unused_result))
#endif
#endif

typedef float                   f32;
typedef double                  f64;
typedef long double             f128;
typedef unsigned char           u8;
typedef unsigned short int      u16;
typedef unsigned int            u32;
typedef unsigned long long int  u64;
typedef char                    i8;
typedef signed short int        i16;
typedef int                     i32;
typedef long long int           i64;

#ifdef __cplusplus
typedef bool    b8;
#define TRUE    true
#define FALSE   false
#else
typedef unsigned char           b8;
#define TRUE    ((b8)0x1)
#define FALSE   ((b8)0x0)
#endif

#endif // TYPES_H
