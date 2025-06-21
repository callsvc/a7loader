#include <assert.h>
#include <type.h>
#include <virt_io/zipped_file.h>

void ZippedFileRead(struct VfsBase *vfs, void *data, const size_t offset, const size_t size) {
    const struct ZippedFile *zip = (struct ZippedFile *)vfs;
    if (!zip_file_is_seekable(zip->file)) {
        if (offset)
            while (zip_ftell(zip->file) != offset)
                zip_fread(zip->file, data, 1);
    } else {
        zip_fseek(zip->file, offset, SEEK_SET);
    }
    assert(zip_fread(zip->file, data, size) == size);
}

size_t ZippedFileGetSize(struct VfsBase *vfs) {
    const struct ZippedFile *zip = (struct ZippedFile *)vfs;
    return zip->uncsize;
}

struct ZippedFile * ZippedFileOpen(zip_t * za, const size_t index) {
    struct ZippedFile *zip = Malloc(sizeof(struct ZippedFile));
    VfsUpdate(&zip->vfs, 1);

    zip_stat_t fst;
    zip_stat_index(za, index, 0, &fst);

    zip->uncsize = fst.size;
    VfsInit(&zip->vfs, fst.name, "r");

    VFS_READ(zip, ZippedFileRead);
    VFS_GET_SIZE(zip, ZippedFileGetSize);

    zip->file = zip_fopen_index(za, index, 0);
    return zip;
}
void ZippedFileClose(struct ZippedFile * zip) {
    VfsUpdate(&zip->vfs, 0);
    VfsFinish(&zip->vfs);

    zip_fclose(zip->file);
    Free(zip);
}
