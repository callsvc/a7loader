#pragma once
#include <pthread.h>
#include <stdbool.h>

struct VfsBase {
    char *path;
    char *mode;
    int refcount;

    pthread_mutex_t mutex;

    void (*Write)(struct VfsBase *, const void *, size_t, size_t);
    void (*Read)(struct VfsBase *, void *, size_t, size_t);
    struct Vector * (*ListAllFiles)(struct VfsBase *, bool);
    size_t (*GetSize)(struct VfsBase *);

    void (*DirCloseFile)(struct VfsBase *, struct VfsBase*);
    struct VfsBase * (*DirOpenFile)(struct VfsBase *, const char *, const char *);
};

void VfsInit(struct VfsBase * vfs, const char *path, const char *mode);
void VfsFinish(struct VfsBase *vfs);
bool VfsUpdate(struct VfsBase *vfs, int status);

void VfsLock(struct VfsBase *vfs);
void VfsUnlock(struct VfsBase *vfs);

size_t VfsGetSize(struct VfsBase *vfs);

struct VfsBase * VfsDirOpenFile(struct VfsBase *dir, const char *pathname, const char *mode);
void VfsDirCloseFile(struct VfsBase *dir, struct VfsBase *vfs);

const char* VfsGetPath(const struct VfsBase *vfs);
const char* VfsGetName(const struct VfsBase *vfs);

#define VFS_WRITE(obj, func)\
    obj->vfs.Write = func
#define VFS_READ(obj, func)\
    obj->vfs.Read = func
#define VFS_CLOSE_FILE(obj, func)\
    obj->vfs.DirCloseFile = func

#define VFS_GET_SIZE(obj, func)\
    obj->vfs.GetSize = func

#define VFS_LIST_ALL_FILES(obj, func)\
    obj->vfs.ListAllFiles = func
#define VFS_OPEN_FILE(obj, func)\
    obj->vfs.DirOpenFile = func


void VfsWrite(struct VfsBase *vfs, const void *data, size_t offset, size_t size);
void VfsRead(struct VfsBase *vfs, void *data, size_t offset, size_t size);

struct Vector * VfsListAllFiles(struct VfsBase *vfs, bool recursive);

