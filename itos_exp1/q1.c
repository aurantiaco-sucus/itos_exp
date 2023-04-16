#include "common.h"

int main() {
    pid_t pid = fork(); // Pure ~*magic*~
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