#include "common.h"

pid_t pid1, pid2;

void handle_signal(int signal) {
    if (signal != SIGINT) {
        return;
    }
    kill(pid1, SIGINT);
    kill(pid2, SIGINT);
}

void handle_child1_signal(int signal) {
    if (signal != SIGINT) {
        return;
    }
    printf("Child process1 is killed by parent!\n");
    exit(0);
}

void handle_child2_signal(int signal) {
    if (signal != SIGINT) {
        return;
    }
    printf("Child process2 is killed by parent!\n");
    exit(0);
}

int main() {
    // Register signal handler for parent process
    signal(SIGINT, handle_signal);

    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(1);
    } else if (pid1 == 0) {
        // Child process 1
        signal(SIGINT, handle_child1_signal);
        while (1) sleep(1);
    }

    pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(1);
    } else if (pid2 == 0) {
        // Child process 2
        signal(SIGINT, handle_child2_signal);
        while (1) sleep(1);
    }

    // Parent process
    int status;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
    printf("Parent process is killed!\n");
    return 0;
}