#include "common.h"

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // child process
        char *args[] = {"/usr/bin/uname", "-a", NULL};
        int ret = execv(args[0], args);
        if (ret == -1) {
            perror("execv");
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {
        // parent process
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            printf("Child process exited with status %d\n", WEXITSTATUS(status));
        }
    } else {
        // fork failed
        perror("fork");
        exit(EXIT_FAILURE);
    }

    return 0;
}