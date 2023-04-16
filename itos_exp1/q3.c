#include "common.h"

int main() {
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