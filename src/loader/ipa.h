#pragma once
#include <stdbool.h>
#include <app.h>
#include <formats/property_list.h>

struct IPA {
    const char *name;

    struct VfsBase *appfs;
    struct PropertyList *infoplist;
    bool zipped;
};

struct IPA *IpaOpen(const struct App *app, const char *filename);
void IpaClose(struct IPA *ipamold);