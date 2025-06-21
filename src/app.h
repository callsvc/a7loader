#pragma once

struct App {
    char *root;

    char *installDir;
    struct Vector *ipalist;
};

struct App* AppInit();
void AppFinish(struct App* app);