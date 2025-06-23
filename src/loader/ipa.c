#include <stdlib.h>

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

void SetIpaName(struct IPA *ipamold) {
    ipamold->name = GetBundleName(ipamold);
    if (ipamold->name)
        Free((char*)ipamold->name);
    const char *bundle = PListGetText(ipamold->infoplist, "CFBundleName");
    ipamold->name = Malloc(snprintf(NULL, 0, "%s.app", bundle) + 1);
    sprintf((char*)ipamold->name, "%s.app", bundle);
}

struct IPA *IpaOpen(const struct App *app, const char *filename) {
    struct IPA *ipamold = Malloc(sizeof(struct IPA));

    if (app->installDir) {
        InstallIpa(app->installDir, filename);
        ipamold->appfs = (struct VfsBase*)DirOpen(app->installDir);
    } else {
        ipamold->appfs = (struct VfsBase*)ZippedDirOpen(filename);
        ipamold->zipped = true;
    }

    const char *infoname = MergePaths(3, "Payload", ipamold->name, "Info.plist");
    struct VfsBase *plistfile = VfsDirOpenFile(ipamold->appfs, infoname, "r");

    if (plistfile) {
        ipamold->infoplist = PListParser(plistfile);
        VfsDirCloseFile(ipamold->appfs, plistfile);
    }

    Free((void*)infoname);

    SetIpaName(ipamold);

    return ipamold;
}
void IpaClose(struct IPA *ipamold) {
    free((void*)ipamold->name);
    if (!ipamold->zipped)
        DirClose((struct Dir*)ipamold->appfs);
    else
        ZippedDirClose((struct ZippedDir*)ipamold->appfs);

    PListDestroy(ipamold->infoplist);
    free(ipamold);
}