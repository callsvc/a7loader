#include <stdio.h>
#include <string.h>
#include <regex.h>

#include <loader/bundle.h>
#include <loader/ipa.h>

#include <vector.h>
#include <virt_io/type.h>

#include <type.h>

char * Strdupn(const char *begin, const size_t size) {
    char *result = Malloc(size);
    snprintf(result, size, "%s", begin);
    return result;
}

const char * GetBundleName(const struct IPA *ipa) {
    char *result = NULL;
    struct Vector *files = VfsListAllFiles(ipa->appfs, true);

    const char *pattern = "[^/]+\\.app";
    regex_t regex;
    regmatch_t match;
    regcomp(&regex, pattern, REG_EXTENDED);

    for (int count = 0; count < VectorGetSize(files) && !result; count++) {
        if (!regexec(&regex, VectorGet(files, count), 1, &match, 0))
            result = Strdupn(VectorGet(files, count) + match.rm_so, match.rm_eo - match.rm_so + 1);
    }
    VectorDestroy(files);
    return result;
}
