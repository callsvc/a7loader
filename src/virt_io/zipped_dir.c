#include <type.h>

#include <virt_io/zipped_file.h>
#include <virt_io/zipped_dir.h>

struct VfsBase * ZippedDirOpenFile(struct VfsBase *dir, const char *pathname, const char *mode) {
    const struct ZippedDir *folder = (struct ZippedDir *)dir;
    zip_int64_t tagfile;
    if (folder->zip)
        if ((tagfile = zip_name_locate(folder->zip, pathname, 0)) != -1)
            return (struct VfsBase*)ZippedFileOpen(folder->zip, tagfile);
    return NULL;
}
// ReSharper disable once CppParameterMayBeConstPtrOrRef
void ZippedDirCloseFile(struct VfsBase *dir, struct VfsBase *vfs) {
    (void)dir;
    ZippedFileClose((struct ZippedFile*)vfs);
}

struct ZippedDir * ZippedDirOpen(const char *pathname) {
    struct ZippedDir *folder = Malloc(sizeof(struct ZippedDir));
    VfsInit(&folder->vfs, pathname, "r");
    VfsUpdate(&folder->vfs, 1);

    folder->zip = zip_open(pathname, ZIP_RDONLY, 0);
    VFS_OPEN_FILE(folder, ZippedDirOpenFile);
    VFS_CLOSE_FILE(folder, ZippedDirCloseFile);

    return folder;
}
void ZippedDirClose(struct ZippedDir *zip) {
    VfsUpdate(&zip->vfs, 0);
    zip_close(zip->zip);

    VfsFinish(&zip->vfs);
    Free(zip);
}