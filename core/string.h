#ifndef STRING_H
#define STRING_H

#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "array.h"

struct String {
    u32 len;
    u32 capacity;
    char *data;

    ~String();

    char operator[](u32 at);
};

String string_new();
void string_push(String *str, char value);
String string_from(const char *c_str);
String string_concat(String a, String b);
b8 string_equal(String a, String b);
Array<String> string_split(String str, const char delim);
String string_strip(String str);
String string_strip_free(String str);
char* string_c(String *str);

void string_reset(String *str);
void string_free(String *str);

#endif // STRING_H
