#include <stdarg.h>
#include <paths.h>
#include <string.h>

#include <linux/limits.h>
#include <type.h>

char * MergePaths(const int depth, ...) {
    va_list args;
    va_start(args, depth);

    char *result = Malloc(PATH_MAX);
    size_t position = 0;

    for (int i = 0; i < depth; i++) {
        if (position)
            result[position - 1] = '/';
        const char *path = va_arg(args, char *);
        const size_t len = strlen(path);
        memcpy(&result[position], path, len);
        position += len + 1;
    }

    result[position] = '\0';
    va_end(args);
    return result;
}
