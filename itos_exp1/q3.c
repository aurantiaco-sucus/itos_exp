#include "common.h"

int main() {
    pid_t pid;
    int fd;
    char buf[20];

    fd = open("q3f.txt", O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        //lock
        if (lockf(fd, F_LOCK, 0) == -1) {
            perror("lockf");
            exit(EXIT_FAILURE);
        }

        sprintf(buf, "Child process\n");
        write(fd, buf, strlen(buf));

        //unlock
        if (lockf(fd, F_ULOCK, 0) == -1) {
            perror("lockf");
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    }

    //lock
    if (lockf(fd, F_LOCK, 0) == -1) {
        perror("lockf");
        exit(EXIT_FAILURE);
    }

    sprintf(buf, "Parent process\n");
    write(fd, buf, strlen(buf));

    // unlock
    if (lockf(fd, F_ULOCK, 0) == -1) {
        perror("lockf");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}