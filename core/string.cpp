#include "string.h"
#include "array.h"

#include <stdio.h>
#include <memory.h>
#include <assert.h>

// operators and overloads
// ---------------------------------------------------
String::~String() {
    free(this->data);
}

char String::operator[](u32 at) {
    assert(at < len && "Array out of bounds access");
    return data[at];
}
// ---------------------------------------------------

String string_new() { return string_from(""); }

String string_from(const char *c_str) {
    String result;
    const u32 len = strlen(c_str);
    const u32 cap = len > 0 ? len * sizeof(char) : 4;
    char *data = (char *)malloc(cap);

    if (len > 0) {
        memcpy(data, c_str, len);
    }

    result.len = len;
    result.capacity = cap;
    result.data = data;

    return result;
}

void __string_realloc(String *str) {
    u32 new_capacity = (str->len+str->len/2) * sizeof(char);
    new_capacity = new_capacity > 0 ? new_capacity : 4;
    char *data = (char *)realloc((void *)(str->data), new_capacity);
    if (data == NULL) {
        fprintf(stderr, "[%s:%s:%d] Failed string realloc, not enough memory", __FILE_NAME__, __FUNCTION__, __LINE__);
        exit(1);
    } else {
        str->data = data;
        str->capacity = new_capacity;
    }
}

void string_push(String *str, char c) {
    if (str->len * sizeof(char) >= str->capacity) {
        __string_realloc(str);
    }
    str->data[str->len] = c;
    str->len++;
}

String string_concat(String a, String b) {
    String result;
    const u32 capacity = a.capacity + b.capacity;
    char *data = (char *)malloc(capacity);
    const u32 len = a.len + b.len;

    if (len > 0) {
        memcpy(data,         a.data, a.len);
        memcpy(data + a.len, b.data, b.len);
    }

    result.len = len;
    result.capacity = capacity;
    result.data = data;

    return result;
}

Array<String> string_split(String str, const char delim) {
    Array<String> result;
    array_init(&result, 4);

    String current = string_from("");
    for (u32 i=0; i<str.len; i++) {
        if (str[i] != delim) {
            string_push(&current, str[i]);
        } else {
            array_push(&result, current);
            current = string_from("");
        }
    }
    array_push(&result, current);

    return result;
}

char* string_c(String *str) {
    string_push(str, '\0');
    str->len--;
    return str->data;
}

b8 string_equal(String a, String b) {
    if (a.len != b.len) return FALSE;

    for (u32 i=0; i<a.len; i++) {
        if (a[i] != b[i]) return FALSE;
    }

    return TRUE;
}

String string_strip(String str) {
    if (str.len == 0) return string_from("");
    String result;

    u32 from = 0;
    for (u32 i=0; i<str.len && str[i] == ' '; i++) from++;
    u32 take = str.len - from;
    for (i32 i=str.len-1; i>=0 && str[i] == ' '; i--) take--;

    if (take == 0 || from >= str.len) {
        return string_from("");
    }

    result.len = take;
    result.capacity = take * sizeof(char);
    result.data = (char *)malloc(sizeof(char) * result.capacity);
    memcpy(result.data, str.data + from, take);

    return result;
}

String string_strip_free(String str) {
    const auto result = string_strip(str);
    string_free(&str);
    return result;
}

void string_reset(String *str) {
    str->len = 0;
    str->capacity = 0;
}

void string_free(String *str) {
    str->len = 0;
    str->capacity = 0;
    free(str->data);
}

