#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <stddef.h>
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ALIGN_TO(x, align) (((x) + ((align) - 1)) & ~((align) - 1))

void * Malloc(size_t size);
void Free(void *pointer);
void Kill(const char *err);