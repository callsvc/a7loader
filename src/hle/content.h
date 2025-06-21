#pragma once
#include <app.h>

struct Vector * GetIpaIcon(const struct App *app, size_t index);
size_t GetIpasCount(const struct App *app);
const char * GetIpaName(const struct App *app, size_t index);
