#include <string.h>

#include <type.h>
#include <virt_io/type.h>

void VfsInit(struct VfsBase * vfs, const char *path, const char *mode) {
    pthread_mutex_init(&vfs->mutex, NULL);
    vfs->path = strdup(path);
    if (mode != NULL)
        vfs->mode = strdup(mode);

    vfs->refcount = 0;
}

void VfsFinish(struct VfsBase * vfs) {
    Free(vfs->path);
    Free(vfs->mode);

    pthread_mutex_destroy(&vfs->mutex);
}

bool VfsUpdate(struct VfsBase *vfs, const int status) {
    pthread_mutex_lock(&vfs->mutex);

    if (status)
        vfs->refcount++;
    else if (vfs->refcount != 0)
        vfs->refcount--;

    const bool result = vfs->refcount != 0;

    pthread_mutex_unlock(&vfs->mutex);
    return result;
}

void VfsLock(struct VfsBase *vfs) { VfsUpdate(vfs, 1); }
void VfsUnlock(struct VfsBase *vfs) { VfsUpdate(vfs, 0); }

size_t VfsGetSize(struct VfsBase *vfs) {
    return vfs->GetSize(vfs);
}

struct VfsBase * VfsDirOpenFile(struct VfsBase *dir, const char *pathname, const char *mode) {
    return dir->DirOpenFile(dir, pathname, mode);
}

void VfsDirCloseFile(struct VfsBase *dir, struct VfsBase *vfs) {
    dir->DirCloseFile(dir, vfs);
}
const char* VfsGetPath(const struct VfsBase *vfs) {
    return vfs->path;
}
const char * VfsGetName(const struct VfsBase *vfs) {
    if (strchr(vfs->path, '/') != NULL)
        return strrchr(vfs->path, '/') + 1;
    return VfsGetPath(vfs);
}

void VfsWrite(struct VfsBase *vfs, const void *data, const size_t offset, const size_t size) {
    vfs->Write(vfs, data, offset, size);
}

void VfsRead(struct VfsBase *vfs, void *data, const size_t offset, const size_t size) {
    vfs->Read(vfs, data, offset, size);
}

struct Vector * VfsListAllFiles(struct VfsBase *vfs, bool recursive) {
    return vfs->ListAllFiles(vfs, recursive);
}
