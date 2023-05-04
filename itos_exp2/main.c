#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define COUNT_RESOURCE 10
#define COUNT_PROCESS 10
#define INITIAL_CAP 20
#define DEMAND_CAP 20

int available[COUNT_RESOURCE];
int requirement[COUNT_PROCESS][COUNT_RESOURCE];
int allocated[COUNT_PROCESS][COUNT_RESOURCE];
int pending[COUNT_PROCESS];
int pending_count = COUNT_PROCESS;

bool can_satisfy(int pi) {
    for (int i = 0; i < COUNT_RESOURCE; ++i) {
        if (allocated[pi][i] + available[i] < requirement[pi][i]) {
            return false;
        }
    }
    return true;
}

void execute_unchecked(int pi) {
    printf("---EXECUTION OF P%d---\n", pi);
    for (int i = 0; i < COUNT_RESOURCE; ++i) {
        printf("\tR%d", i);
    }
    printf("\nBEFORE");
    for (int i = 0; i < COUNT_RESOURCE; ++i) {
        printf("\t%d", available[i]);
    }
    printf("\nAFTER");
    for (int i = 0; i < COUNT_RESOURCE; ++i) {
        available[i] += allocated[pi][i];
    }
    for (int i = 0; i < COUNT_RESOURCE; ++i) {
        printf("\t%d", available[i]);
    }
    printf("\n");
    --pending_count;
    if (pending_count != 0) {
        pending[pi] = pending_count;
    }
}

int main() {
    // initialize pending list
    for (int i = 0; i < COUNT_PROCESS; ++i) {
        pending[i] = i;
    }

    // randomize test data
    srand48((long) time(NULL));
    for (int i = 0; i < COUNT_RESOURCE; ++i) {
        available[i] = (int) (drand48() * INITIAL_CAP);
    }
    for (int i = 0; i < COUNT_PROCESS * COUNT_RESOURCE; ++i) {
        *(((int *) requirement) + i) = (int) (drand48() * DEMAND_CAP);
    }
    for (int i = 0; i < COUNT_PROCESS * COUNT_RESOURCE; ++i) {
        *(((int *) allocated) + i) = (int) (drand48() * *((int *) requirement + i));
    }

    // summary
    printf("--- INITIALIZATION ---\n\tFREE");
    for (int i = 0; i < COUNT_PROCESS; ++i) {
        printf("\tP%d", i);
    }
    printf("\n");
    for (int i = 0; i < COUNT_RESOURCE; ++i) {
        printf("R%d\t%d\t", i, available[i]);
        for (int j = 0; j < COUNT_PROCESS; ++j) {
            printf("%d/%d\t", allocated[j][i], requirement[j][i]);
        }
        printf("\n");
    }

    bool mutated = false;
    while (pending_count > 0) {
        for (int i = 0; i < pending_count; ++i) {
            if (can_satisfy(pending[i])) {
                execute_unchecked(pending[i]);
                mutated = true;
                break;
            }
        }
        if (!mutated) {
            printf("Current state is unsafe!\n");
            break;
        }
    }
}