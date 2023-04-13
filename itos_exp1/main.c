#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/file.h>

int q1() {
    pid_t pid = fork();
    if (pid == 0) {
        // child
        printf("Child: My PID is %d\n", getpid());
    } else if (pid > 0) {
        // parent
        printf("Parent: Child's PID is %d and mine is %d\n", pid, getpid());
    } else {
        // fork failed
        printf("Fork failed.");
    }
    return 0;
}

int q2() {
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

int q3() {
    FILE *file = fopen("data.txt", "a");
    pid_t pid = fork();
    if (pid == -1) {
        // Fork failed
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // Child process
        flock(fileno(file), LOCK_EX);
        fprintf(file, "Child process writing to file...\n");
        flock(fileno(file), LOCK_UN);
        exit(0);
    } else {
        // Parent process
        flock(fileno(file), LOCK_EX);
        fprintf(file, "Parent process writing to file...\n");
        flock(fileno(file), LOCK_UN);
    }
    fclose(file);
    return 0;
}

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

int q4() {
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

int main() {
    return q4();
}