#pragma once


#include <zip.h>
#include <virt_io/type.h>

struct ZippedDir {
    struct VfsBase vfs;
    zip_t *zip;
};

struct ZippedDir * ZippedDirOpen(const char *pathname);
void ZippedDirClose(struct ZippedDir *zip);


