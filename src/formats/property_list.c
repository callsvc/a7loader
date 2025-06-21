
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <type.h>
#include <vector.h>
#include <formats/property_list.h>

struct PropertyListHeader {
    uint8_t magic[8];
};

uint8_t GetVersion(const struct PropertyListHeader *property) {
    if (memcmp(property->magic, "bplist", 6))
        Kill("Magic not recognized");
    const char *version = (char*)property->magic + 6;
    if (!memcmp(version, "00", 2))
        return 0;
    if (!memcmp(version, "15", 2))
        return 15;
    if (!memcmp(version, "16", 2))
        return 16;
    return -1;
}

enum PListObjectType GetType(const uint8_t value) {
    switch (value >> 4) {
        case 0:
            if ((value & 0xF) == 0xF)
                return PListFill;
            if ((value & 0xF) == 0 || (value & 0xF) == 1)
                return PListBoolean;
            break;
        case 1: return PListInteger;
        case 5: return PListString;
        case 6: return PListStringUnicode;
        case 0xD: return PListDictionary;
        default:
    }
    return PListNull;
}

void GetOffsets(const struct PropertyList *plist, const uint8_t *begin, const int8_t offsize, const size_t size) {
    for (const uint8_t *poff = begin; poff != begin + size; poff += offsize) {
        uint32_t value = 0;
        if (offsize == 2)
            value = __builtin_bswap16(*(uint16_t*)poff);
        else if (offsize == 4)
            value = __builtin_bswap32(*(uint32_t*)poff);

        VectorEmplace(plist->offsets, &value);
    }
}

uint64_t __DecodeInteger(uint8_t *data, const size_t size) {
    if (size == 1)
        return *data;
    if (size == 2)
        return __builtin_bswap16(*(uint16_t*)data);
    if (size == 4)
        return __builtin_bswap32(*(uint32_t*)data);
    if (size == 8)
        return __builtin_bswap64(*(uint64_t*)data);
    return 0;
}
uint64_t DecodeInteger(uint8_t *data, size_t *size) {
    *size = pow(2, *data++ & 0xF);
    return __DecodeInteger(data, *size);
}

void GetObjects(const struct PropertyList *plist, const uint8_t *begin, const int refsize, const size_t size) {
    struct PListObject object;

    for (uint32_t count = 0; count < VectorGetSize(plist->offsets); count++) {
        memset(&object, 0, sizeof(object));
        size_t bytes = 0;

        uint32_t objoffset = 0;
        memcpy(&objoffset, VectorGet(plist->offsets, count), VectorSizeofType(plist->offsets));

        assert(objoffset < size);
        uint8_t *values = (uint8_t*)begin + objoffset;
        object.type = GetType(*values);

        uint32_t length = *values & 0xF;
        if (length == 0xF)
            length = DecodeInteger(++values, &bytes);

        if (object.type == PListInteger) {
            object.integer = DecodeInteger(values, &bytes);
        } else if (object.type == PListString) {
            strncpy(object.string, (char*)++values + bytes, length);
        } else if (object.type == PListDictionary) {
            uint8_t *_keys = ++values + bytes;
            uint8_t *_values = values + bytes + length * refsize;

            strncpy(object.keysfmt, "?", 1);
            for (int pair = 0; pair < length; pair++)
                sprintf(strstr(object.keysfmt, "?"), "%u:%u|?",
                    (uint32_t)__DecodeInteger(_keys + pair * refsize, refsize),
                    (uint32_t)__DecodeInteger(_values + pair * refsize, refsize));
        }

        VectorEmplace(plist->objects, &object);
    }
}

bool FindPairBykey(const struct PropertyList * plist, struct PListObject** pair, const char *keyname) {
    for (uint32_t objects = 0; objects < VectorGetSize(plist->objects); objects++) {
        const struct PListObject* parent = VectorGet(plist->objects, objects);
        if (parent->type != PListDictionary)
            continue;

        for (const char *result = parent->keysfmt; *result != '?'; ) {
            uint32_t key = 0, value = 0;
            sscanf(result, "%u:%u|", &key, &value);
            result = strchr(result, '|') + 1;

            pair[0] = VectorGet(plist->objects, key);
            if (pair[0]->type == PListString)
                if (!strcmp(pair[0]->string, keyname))
                    if ((pair[1] = VectorGet(plist->objects, value)))
                        return true;
        }
    }
    return false;
}

const char* PListGetText(const struct PropertyList *plist, const char *keyname) {
    struct PListObject * pairs[2];
    if (FindPairBykey(plist, pairs, keyname))
        if (pairs[0] && pairs[1])
            if (pairs[1]->type == PListString)
                return pairs[1]->string;
    return NULL;
}

struct PropertyList * PListParser(struct VfsBase *file) {
    struct PropertyList * plist = Malloc(sizeof(struct PropertyList));
    plist->objects = VectorCreateType(sizeof(struct PListObject));

    struct Vector *content = VfsReadContent(file);
    if (VectorGetSize(content) < sizeof(struct PropertyListHeader))
        Kill("Plist header is malformed");

    uint8_t *begin = VectorFront(content);

    const struct PropertyListHeader *property = (struct PropertyListHeader*)begin;
    if (GetVersion(property))
        Kill("Only version 0 is accepted by now");

    uint8_t *trailer = begin + VectorGetSize(content) - 32;

    const size_t offsets = __builtin_bswap64(*(uint64_t*)(trailer + 8));
    plist->offsets = VectorCreateType(trailer[6]);

    GetOffsets(plist, begin + __builtin_bswap64(*(uint64_t*)(trailer + 24)), trailer[6], offsets);
    GetObjects(plist, begin, trailer[7], VectorGetSize(content));

    VectorDestroy(content);
    return plist;
}

void PListDestroy(struct PropertyList *plist) {
    VectorDestroy(plist->objects);
    VectorDestroy(plist->offsets);
    Free(plist);
}
