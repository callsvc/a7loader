#pragma once

enum VectorType {
    VecOfBytes,
    VecOfStrings,
    VecOfPointers
};

struct Vector {
    enum VectorType type;
    size_t size;
    size_t capacity;
    char *data;
};

struct Vector *VectorBacked(void *data, size_t size);
struct Vector *VectorCreate(enum VectorType type);

void *VectorGetBack(const struct Vector *vec);
void *VectorGetFront(const struct Vector *vec);

void *VectorGet(const struct Vector *vec, size_t index);
size_t VectorGetSize(const struct Vector *vec);

void VectorSetSize(struct Vector *vec, size_t size);
void VectorResize(struct Vector *vec, size_t size);
void VectorEmplace(struct Vector *vec, const void *data);

void VectorDestroy(struct Vector *vec);
