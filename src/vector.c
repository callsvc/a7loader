
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>
#include <type.h>

size_t SizeofType(const struct Vector *vec, const void *data) {
    if (data != NULL)
        if (vec->type == VecOfStrings && isprint(*(char*)data))
            return strlen(data);
    if (vec->type == VecOfBytes)
        return 1;
    if (vec->type == VecOfPointers)
        return sizeof(void *);

    assert(!"String is not supported");
}

size_t GetTotalOfStrings(const struct Vector *vec) {
    size_t count = 0;
    for (const char* result = vec->data;
            result && isprint(*result); count++)
        result += strlen(result) + 1;
    return count;
}

void* GetIter(const struct Vector *vec, const size_t index) {
    if (vec->type != VecOfStrings)
        return (unsigned char*)vec->data + SizeofType(vec, NULL) * index;

    char *result = vec->data;
    for (int count = 0; count < index; count++)
        if ((result = strchr(result, '\0')) == NULL)
            return NULL;
    return result;
}
void* VectorGetBack(const struct Vector *vec) {
    return GetIter(vec, vec->size);
}
void* VectorGetFront(const struct Vector *vec) {
    return GetIter(vec, 0);
}

struct Vector * VectorBacked(void *data, const size_t size) {
    struct Vector *vector = Malloc(sizeof(struct Vector));
    vector->data = data;
    vector->size = size;
    vector->type = VecOfBytes;
    vector->capacity = -1;

    return vector;
}
struct Vector *VectorCreate(const enum VectorType type) {
    struct Vector *vector = Malloc(sizeof(struct Vector));
    vector->type = type;
    return vector;
}

void * VectorGet(const struct Vector *vec, const size_t index) {
    if (index > VectorGetSize(vec))
        return NULL;
    if (vec->type == VecOfPointers)
        return *(void**)GetIter(vec, index);
    return GetIter(vec, index);
}
size_t VectorGetSize(const struct Vector *vec) {
    return vec->type == VecOfStrings ? GetTotalOfStrings(vec) : vec->size / SizeofType(vec, NULL);
}
void VectorSetSize(struct Vector *vec, const size_t size) {
    if (size > vec->capacity)
        VectorResize(vec, size);
    vec->size = size;
}
void VectorResize(struct Vector *vec, size_t size) {
    size = ALIGN_TO(size, 8);
    void *result = Malloc(size);

    if (!vec->data) {
        vec->data = result;
    } else if (vec->size) {
        memcpy(result, vec->data, vec->size);
        Free(vec->data);
        vec->data = result;
    }
    vec->capacity = size;
}

void VectorEmplace(struct Vector *vec, const void *data) {
    if (!vec->capacity)
        VectorResize(vec, SizeofType(vec, data));
    if (vec->size >= vec->capacity)
        VectorResize(vec, vec->capacity * 2 + SizeofType(vec, data));

    switch (vec->type) {
        case VecOfStrings:
        case VecOfBytes:
            memcpy(VectorGetBack(vec), data, strlen(data)); break;
        case VecOfPointers:
            memcpy(VectorGetBack(vec), &data, sizeof(void*)); break;
    }
    vec->size += SizeofType(vec, data);
}

void VectorDestroy(struct Vector *vec) {
    if (vec->capacity != -1)
        Free(vec->data);
    Free(vec);
}
