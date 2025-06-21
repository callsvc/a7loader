
#include <app.h>

#include <type.h>
#include <paths.h>
#include <vector.h>
#include <hle/collection.h>
#include <hle/content.h>
#include <virt_io/dir.h>


int main(void) {
    struct App *app = AppInit();

    GetAllIpas(app);
    char *logosdirname = MergePaths(2, app->root, "logos");
    struct Dir *logos = DirOpen(logosdirname);
    for (size_t count = 0; count < GetIpasCount(app); count++) {
        char *output = MergePaths(2, GetIpaName(app, count), "icon.jpeg");
        struct VfsBase *outfile = VfsDirOpenFile((struct VfsBase*)logos, output, "w");

        struct Vector *icon = GetIpaIcon(app, count);
        VfsWrite(outfile, VectorFront(icon), 0, VectorGetSize(icon));

        VfsDirCloseFile((struct VfsBase*)logos, outfile);
        VectorDestroy(icon);
        Free(output);
    }

    Free(logosdirname);
    DirClose(logos);
    AppFinish(app);
    return 0;
}
