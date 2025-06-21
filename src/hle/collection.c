#include <hle/collection.h>

#include <paths.h>
#include <type.h>
#include <vector.h>
#include <loader/ipa.h>
#include <virt_io/dir.h>

void GetAllIpas(struct App *app) {
    char *folder = MergePaths(2, app->root, "ipalist");
    struct Dir *games = DirOpen(folder);

    struct Vector *files = VfsListAllFiles((struct VfsBase*)games, true);
    app->ipalist = VectorCreate(VecOfPointers);

    for (size_t count = 0; count < VectorGetSize(files); count++) {
        char *ipafile = MergePaths(2, folder, VectorGet(files, count));
        const struct IPA *ipa = IpaOpen(app, ipafile);
        if (ipa)
            VectorEmplace(app->ipalist, ipa);
        else Kill("Can't handle file");
        Free(ipafile);
    }

    VectorDestroy(files);
    Free(folder);
    DirClose(games);
}

void FinishIpas(const struct App *app) {
    VectorDestroy(app->ipalist);
}
