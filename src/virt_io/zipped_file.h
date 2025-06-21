#pragma once

#include <zip.h>
#include <virt_io/type.h>

struct ZippedFile {
    struct VfsBase vfs;
    zip_file_t *file;
    size_t uncsize;
};

struct ZippedFile * ZippedFileOpen(zip_t * za, size_t index);
void ZippedFileClose(struct ZippedFile * zip);