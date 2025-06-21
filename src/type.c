#include <stdlib.h>
#include <stdio.h>
#include <type.h>

void Kill(const char *err) {
    fprintf(stderr, "%s\n", err);
    exit(1);
}

void * Malloc(const size_t size) {
    if (!size)
        return NULL;
    void *result = calloc(size, 1);
    if (!result)
        Kill("Can't allocate");

    return result;
}

void Free(void *pointer) {
    free(pointer);
}
