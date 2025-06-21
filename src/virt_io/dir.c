#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include <vector.h>
#include <type.h>
#include <paths.h>
#include <virt_io/file.h>
#include <virt_io/dir.h>


struct Vector * ListAllFiles(struct VfsBase *vfs, bool recursive) {
    (void)recursive;
    const struct Dir *directory = (struct Dir*)vfs;
    const struct dirent *entry = NULL;

    struct Vector *files = VectorCreate(VecOfStrings);
    while ((entry = readdir(directory->dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        VectorEmplace(files, entry->d_name);
    }
    return files;
}

bool Mkdirs(const char* paths) {
    if (mkdir(paths, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        return false;
    return true;
}

void CreateDirectories(const char *filename) {
    char *pathdirs;
    if (strstr(filename, getenv("PWD")))
        pathdirs = strdup(filename + strlen(getenv("PWD")) + 1);
    else pathdirs = strdup(filename);

    *strrchr(pathdirs, '/') = '\0';
    char *pathbuild = NULL;

    char *tok;
    for (char *rel = pathdirs; (rel = strtok_r(rel, "/", &tok)); ) {
        char *path = pathbuild ? MergePaths(2, pathbuild, rel) : strdup(rel);
        Free(pathbuild);
        pathbuild = path;

        Mkdirs(pathbuild);
        rel = NULL;
    }
    Free(pathbuild);
    Free(pathdirs);
}

struct VfsBase * DirOpenFile(struct VfsBase *vfs, const char *pathname, const char *mode) {
    const struct Dir *directory = (struct Dir*)vfs;
    for (size_t count = 0; count < VectorGetSize(directory->cachedFiles); count++) {
        struct VfsBase *file = VectorGet(directory->cachedFiles, count);
        if (strcmp(VfsGetName(file), pathname))
            return file;
    }

    const char *pathfile = pathname;
    if (!strstr(VfsGetPath(vfs), pathname))
        pathfile = MergePaths(2, VfsGetPath(vfs), pathfile);
    if (strchr(mode, 'w'))
        CreateDirectories(pathfile);

    struct File *file = FileOpen(pathfile, mode);
    if (!file) {
        VfsUpdate((struct VfsBase*)file, 1);
        VectorEmplace(directory->cachedFiles, file);
    }
    if (pathfile != pathname)
        Free((void*)pathfile);

    return (struct VfsBase*)file;
}
void DirCloseFile(struct VfsBase *vfs, struct VfsBase *file) {
    const struct Dir *directory = (struct Dir*)vfs;
    (void)directory;
    // TODO: Remove from "cached files"
    struct File *handler = (struct File*)file;
    FileClose(handler);
}

struct Dir *DirOpen(const char *path) {
    struct Dir *directory = Malloc(sizeof(struct Dir));
    VfsInit(&directory->vfs, path, NULL);
    VfsUpdate(&directory->vfs, 1);

    directory->cachedFiles = VectorCreate(VecOfPointers);
    if (access(path, F_OK))
        if (!Mkdirs(path))
            Kill("Can't create directory");
    directory->dir = opendir(path);
    VFS_LIST_ALL_FILES(directory, ListAllFiles);
    VFS_OPEN_FILE(directory, DirOpenFile);
    VFS_CLOSE_FILE(directory, DirCloseFile);

    return directory;
}

void DirClose(struct Dir *dir) {
    for (size_t count = 0; count < VectorGetSize(dir->cachedFiles); count++) {
        struct VfsBase *file = VectorGet(dir->cachedFiles, count);
        VfsDirCloseFile((struct VfsBase*)dir, file);
    }
    VectorDestroy(dir->cachedFiles);
    VfsUpdate(&dir->vfs, 0);
    VfsFinish(&dir->vfs);

    closedir(dir->dir);

    Free(dir);
}