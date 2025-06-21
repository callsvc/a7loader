#pragma once

#include <stdio.h>
#include <virt_io/type.h>

struct File {
    struct VfsBase vfs;
    FILE *handler;
};


struct File * FileOpen(const char *pathname, const char *mode);
void FileClose(struct File *file);