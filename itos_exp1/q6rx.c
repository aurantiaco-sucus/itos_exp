#include "common.h"

int main() {
    const int SIZE = 1024;
    const char *name = "/my_shared_memory";
    char message[SIZE];

    // open the shared memory object
    int shm_fd = shm_open(name, O_RDONLY, 0666);
    if (shm_fd == -1) {
        perror("Error opening shared memory");
        exit(1);
    }

    // map the shared memory object into the current address space
    void *ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Error mapping shared memory");
        exit(1);
    }

    printf("Press any key to receive the message...");
    getchar();

    // read from the shared memory object
    memcpy(message, ptr, strlen(ptr)+1);

    // clean up
    munmap(ptr, SIZE);
    close(shm_fd);

    printf("%s\n", message);
    return 0;
}