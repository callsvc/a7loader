#pragma once

#include <dirent.h>
#include <virt_io/type.h>

struct Dir {
    struct VfsBase vfs;
    DIR *dir;
    struct Vector *cachedFiles;
};

struct Dir *DirOpen(const char *path);

void DirClose(struct Dir *dir);