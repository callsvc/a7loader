#pragma once
#include <stdbool.h>
#include <app.h>

struct IPA {
    const char *name;

    struct VfsBase *appfs;
    bool zipped;
};

struct IPA *IpaOpen(const struct App *app, const char *filename);
void IpaClose(struct IPA *ipa);