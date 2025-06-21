#pragma once
#include <stdint.h>
#include <virt_io/type.h>

// https://medium.com/@karaiskc/understanding-apples-binary-property-list-format-281e6da00dbd
enum PListObjectType {
    PListNull,
    PListBoolean,
    PListInteger,
    PListString,
    PListStringUnicode,
    PListDictionary,
    PListFill
};
struct PListObject {
    enum PListObjectType type;
    union {
        bool boolean;
        int64_t integer;
        char string[1 << 10];

        char keysfmt[];
    };
};

struct PropertyList {
    struct Vector *offsets;
    struct Vector *objects;
};

struct PropertyList * PListParser(struct VfsBase *file);
const char* PListGetText(const struct PropertyList * plist, const char *keyname);
void PListDestroy(struct PropertyList *plist);