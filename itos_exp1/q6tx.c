#include "common.h"

int main() {
    const int SIZE = 1024;
    const char *name = "/my_shared_memory";
    char message[] = "Hello, world!";

    // create a shared memory object
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error creating shared memory");
        exit(1);
    }

    // configure the size of the shared memory object
    ftruncate(shm_fd, SIZE);

    // map the shared memory object into the current address space
    void *ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Error mapping shared memory");
        exit(1);
    }

    // write to the shared memory object
    memcpy(ptr, message, strlen(message)+1);

    // clean up
    munmap(ptr, SIZE);
    close(shm_fd);
    return 0;
}