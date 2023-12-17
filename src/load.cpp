#include "load.h"

#include "core/string.h"
#include "core/error.h"

static
bool is_win_abs(const String& path) {
    String win_abs_prefix = String::from(":\\");
    String substr = path.substr(1, path.len()-1);
    bool is_abs = substr.starts_with(win_abs_prefix);
    return is_abs;
}

static
bool is_unix_abs(const String &path) {
    String unix_abs_prefix = String::from("/");
    bool is_abs = path.starts_with(unix_abs_prefix);
    return is_abs;
}

static
String replace_slashes(const String& path) {
    String result;
    for (const char token : path) {
        if (token == FS_FROM_PATH_SEP_TOKEN) {
            result += FS_INTO_PATH_SEP_TOKEN;
        } else {
            result += token;
        }
    }
    return result;
}

String Loader::resolve_filepath(const char *filepath) {
    String filepath_str = String::from(filepath);
#ifdef _WIN32
    if (is_unix_abs(filepath_str)) {
        IO_LOG(stderr, "cannot use unix absolute path notation on a win32 system...");
    }
#else
    if (is_win_abs(filepath_str)) {
        IO_LOG("cannot use win32 absolute path notation on a unix system...");
        exit(-1);
    }
#endif
    String result = replace_slashes(filepath_str);
    IO_LOG(stdout, "made a new path %s", result.raw());
    return result;
}
