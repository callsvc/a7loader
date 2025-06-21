
#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>
#include <type.h>

size_t VectorSizeofType(const struct Vector *vec) {
    if (vec->type == VecType)
        return vec->objsize;

    if (vec->type == VecOfBytes)
        return 1;
    if (vec->type == VecOfPointers)
        return sizeof(void *);

    assert(!"String is not supported");
}
size_t SizeofOrStrlen(const struct Vector *vec, const void *str) {
    return vec->type == VecOfStrings ? strlen(str) + 1 : VectorSizeofType(vec);
}

size_t GetTotalOfStrings(const struct Vector *vec) {
    size_t count = 0;
    for (const char* result = vec->data; result && isprint(*result); count++)
        result += strlen(result) + 1;
    return count;
}

void* GetIter(const struct Vector *vec, const size_t index) {
    if (vec->type != VecOfStrings) {
        if (index > vec->size / VectorSizeofType(vec))
            return NULL;
        return (uint8_t*)vec->data + VectorSizeofType(vec) * index;
    }

    char *last = NULL;
    char *result = vec->data;
    for (int count = 0; count < index && result < vec->data + vec->capacity; count++) {
        if ((last = strchr(result, '\0')) == NULL)
            break;
        if (last == result)
            break;
        result = ++last;
    }
    return result;
}

struct Vector * VectorCreateType(const size_t typesize) {
    struct Vector *vector = Malloc(sizeof(struct Vector));
    vector->objsize = typesize;
    return vector;
}

void* VectorBack(const struct Vector *vec) {
    return GetIter(vec, vec->type != VecOfStrings ? vec->size / VectorSizeofType(vec) : vec->size);
}
void* VectorFront(const struct Vector *vec) {
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
    return vec->type == VecOfStrings ? GetTotalOfStrings(vec) : vec->size / VectorSizeofType(vec);
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
    const size_t bytes = SizeofOrStrlen(vec, data);
    if (!vec->capacity)
        VectorResize(vec, bytes);
    else if (vec->size + bytes >= vec->capacity)
        VectorResize(vec, vec->capacity * 2 + bytes);

    void *place = VectorBack(vec);

    switch (vec->type) {
        case VecOfStrings:
            memcpy(place, data, strlen(data)); break;
        case VecOfBytes:
        case VecType:
            memcpy(place, data, bytes); break;
        default:
            memcpy(place, &data, bytes); break;
    }
    vec->size += bytes;
}

void VectorDestroy(struct Vector *vec) {
    if (vec->capacity != -1)
        Free(vec->data);
    Free(vec);
}
