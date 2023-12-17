#ifndef LOAD_H
#define LOAD_H

#include "core/string.h"

#ifdef _WIN32
#define FS_FROM_PATH_SEP_TOKEN '/'
#define FS_INTO_PATH_SEP_TOKEN '\\'
#else
#define FS_FROM_PATH_SEP_TOKEN '\\'
#define FS_INTO_PATH_SEP_TOKEN '/'
#endif

namespace Loader {

String resolve_filepath(const char *filepath);

}; // namespace Loader


#endif // LOAD_H