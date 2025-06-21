#include <unistd.h>
#include <linux/limits.h>

#include <loader/ipa.h>
#include <vector.h>
#include <type.h>
#include <app.h>


struct App* AppInit() {
    struct App* app = Malloc(sizeof(struct App));
    app->root = Malloc(PATH_MAX);
    if (getcwd(app->root, PATH_MAX) == NULL)
        Kill("Killed");

    return app;
}

void AppFinish(struct App *app) {
    if (app->root)
        Free(app->root);
    for (int index = 0; index < VectorGetSize(app->ipalist); index++) {
        IpaClose(VectorGet(app->ipalist, index));
    }
    VectorDestroy(app->ipalist);
    Free(app);
}
