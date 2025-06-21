#include <assert.h>
#include <pthread.h>
#include <stdbool.h>

#include <type.h>
#include <buffers.h>
#include <stdlib.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char *livebuffer = NULL;
const size_t LiveBufferMinSize = 1 * 1024 * 1024;
bool allocated = false;

void CleanBuffer() {
    Free(livebuffer);
}

char * AllocateBuffer(const size_t size) {
    pthread_mutex_lock(&mutex);
    if (!livebuffer) {
        livebuffer = Malloc(MAX(LiveBufferMinSize, size));
        atexit(CleanBuffer);
    }
    pthread_mutex_unlock(&mutex);

    if (allocated)
        Kill("Already in use");
    allocated = true;
    return livebuffer;
}

void DeallocateBuffer(const void *buffer) {
    pthread_mutex_lock(&mutex);
    assert(buffer == livebuffer && allocated);
    allocated = false;
    pthread_mutex_unlock(&mutex);
}
