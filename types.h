#ifndef TYPES_H
#define TYPES_H

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
