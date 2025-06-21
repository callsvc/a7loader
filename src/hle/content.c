#include <string.h>
#include <hle/content.h>

#include <loader/ipa.h>
#include <vector.h>

#include <type.h>
#include <buffers.h>
#include <virt_io/type.h>

char * GetLogoPath(const struct IPA *ipa) {
    (void)ipa;
    return strdup("iTunesArtwork");
}

struct Vector * GetIpaIcon(const struct App *app, const size_t index) {
    const struct IPA *ipa = VectorGet(app->ipalist, index);
    char *logo = GetLogoPath(ipa);
    struct VfsBase *file = VfsDirOpenFile(ipa->appfs, logo, "r");

    const size_t size = VfsGetSize(file);
    void *buffer = AllocateBuffer(size);
    VfsRead(file, buffer, 0, size);

    struct Vector *result = VectorCreate(VecOfBytes);
    VectorSetSize(result, size);
    memcpy(VectorGetFront(result), buffer, size);

    Free(logo);
    VfsDirCloseFile(ipa->appfs, file);
    return result;
}

size_t GetIpasCount(const struct App *app) {
    return VectorGetSize(app->ipalist);
}

const char * GetIpaName(const struct App *app, const size_t index) {
    if (GetIpasCount(app) < index)
        return NULL;
    return VfsGetName(VectorGet(app->ipalist, index));
}
