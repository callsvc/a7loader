#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <type.h>
#include <unistd.h>
#include <bits/fcntl-linux.h>
#include <virt_io/file.h>

size_t FileGetSize(struct VfsBase *vfs) {
    const struct File *file = (struct File *)vfs;
    const size_t cursor = fseek(file->handler, 0, SEEK_CUR);
    const size_t result = fseek(file->handler, 0, SEEK_END);
    fseek(file->handler, cursor, SEEK_SET);
    return result;
}

void FileWrite(struct VfsBase *vfs, const void *data, const size_t offset, const size_t size) {
    struct File *file = (struct File *)vfs;

    VfsLock(&file->vfs);
    assert(strchr(file->vfs.mode, 'w') != NULL);

    if (fseek(file->handler, 0, SEEK_CUR) != offset)
        fseek(file->handler, offset, SEEK_SET);
    fwrite(data, size, 1, file->handler);

    VfsUnlock(&file->vfs);
}
void FileRead(struct VfsBase *vfs, void *data, const size_t offset, const size_t size) {
    struct File *file = (struct File *)vfs;

    VfsLock(&file->vfs);
    assert(strchr(file->vfs.mode, 'r') != NULL);

    if (fseek(file->handler, 0, SEEK_CUR) != offset)
        fseek(file->handler, offset, SEEK_SET);
    fread(data, size, 1, file->handler);

    VfsLock(&file->vfs);
}

void FileClose(struct File *file) {
    if (VfsUpdate(&file->vfs, 0))
        Kill("File is still in use");

    fclose(file->handler);
    VfsFinish(&file->vfs);
    Free(file);
}

void touch(const char *pathname) {
    close(open(pathname, O_WRONLY | O_CREAT | O_EXCL, 0644));
}

struct File * FileOpen(const char *pathname, const char *mode) {
    struct File *file = Malloc(sizeof(struct File));
    VfsInit(&file->vfs, pathname, mode);

    if (strchr(mode, 'w'))
        if (access(pathname, R_OK))
            touch(pathname);
    if ((file->handler = fopen(pathname, mode)) != NULL)
        VfsUpdate(&file->vfs, 1);

    VFS_WRITE(file, FileWrite);
    VFS_READ(file, FileRead);
    VFS_GET_SIZE(file, FileGetSize);

    return file;
}


