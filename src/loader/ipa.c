#include <stdlib.h>
#include <string.h>

#include <zip.h>

#include <type.h>
#include <paths.h>
#include <buffers.h>
#include <virt_io/file.h>
#include <virt_io/dir.h>

#include <virt_io/zipped_dir.h>
#include <loader/bundle.h>
#include <loader/ipa.h>


void ExtractEntry(zip_file_t *entry, const char *target) {
    const size_t BufferSize = 2 * 1024 * 1024;
    char *buffer = AllocateBuffer(BufferSize);

    struct File *file = FileOpen(target, "w");

    size_t count = 0;
    for (size_t offset = 0; (count = zip_fread(entry, buffer, BufferSize)) != -1; ) {
        VfsWrite((struct VfsBase*)file, buffer, offset, count);
        offset += count;
    }

    FileClose(file);
    DeallocateBuffer(buffer);
}

void InstallIpa(char *install, const char *filename) {
    int err;
    zip_t *zip;
    zip_error_t what;

    if ((zip = zip_open(filename, ZIP_RDONLY, &err)) == NULL) {
        switch (err) {
            case ZIP_ER_NOZIP:
                Kill("Not a zip file");
            default:
                zip_error_init_with_code(&what, err);
                Kill(zip_error_strerror(&what));
        }
    }

    for (size_t count = 0; count != zip_get_num_entries(zip, 0); count++) {
        zip_file_t *entry = zip_fopen_index(zip, count, 0);

        const char *file = zip_get_name(zip, count, 0);
        char *target = MergePaths(2, install, file);
        ExtractEntry(entry, target);

        Free(target);
        zip_fclose(entry);
    }
    zip_close(zip);

}

struct IPA *IpaOpen(const struct App *app, const char *filename) {
    struct IPA *handler = Malloc(sizeof(struct IPA));

    if (app->installDir) {
        InstallIpa(app->installDir, filename);
        handler->appfs = (struct VfsBase*)DirOpen(app->installDir);
    } else {
        handler->appfs = (struct VfsBase*)ZippedDirOpen(filename);
        handler->zipped = true;
    }
    handler->name = GetBundleName(handler);

    return handler;
}
void IpaClose(struct IPA *ipa) {
    free((void*)ipa->name);
    if (!ipa->zipped)
        DirClose((struct Dir*)ipa->appfs);
    else
        ZippedDirClose((struct ZippedDir*)ipa->appfs);
    free(ipa);
}