#include <stdio.h>
#include <stdbool.h>

// Define the maximum number of resources and processes
#define MAX_RESOURCES 10
#define MAX_PROCESSES 10

// Matrix to store the current allocation of each process
int allocation[MAX_PROCESSES][MAX_RESOURCES];

// Matrix to store the current available resources
int available[MAX_RESOURCES];

// Matrix to store the maximum demand of each process
int maximum[MAX_PROCESSES][MAX_RESOURCES];

// Matrix to keep track of which processes are finished
bool finished[MAX_PROCESSES];

// Function to check if a request is valid and safe
bool isRequestValid(int process_id, int* request, int num_resources) {
    // Check if the request exceeds the maximum demand of the process
    for (int i = 0; i < num_resources; i++) {
        if (request[i] > maximum[process_id][i] - allocation[process_id][i]) {
            return false;
        }
    }

    // Check if the request can be satisfied with the available resources
    for (int i = 0; i < num_resources; i++) {
        if (request[i] > available[i]) {
            return false;
        }
    }

    // Temporarily simulate the allocation of the requested resources
    for (int i = 0; i < num_resources; i++) {
        allocation[process_id][i] += request[i];
        available[i] -= request[i];
    }

    // Check if the system is still in a safe state after the allocation
    bool safe = true;

    while (safe) {
        safe = false;

        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (!finished[i]) {
                bool can_run = true;

                for (int j = 0; j < num_resources; j++) {
                    if (maximum[i][j] - allocation[i][j] > available[j]) {
                        can_run = false;
                        break;
                    }
                }

                if (can_run) {
                    safe = true;
                    finished[i] = true;

                    // Release the resources allocated to the process
                    for (int j = 0; j < num_resources; j++) {
                        available[j] += allocation[i][j];
                    }
                }
            }
        }
    }

    // Restore the original allocation and available matrices
    for (int i = 0; i < num_resources; i++) {
        allocation[process_id][i] -= request[i];
        available[i] += request[i];
    }

    return safe;
}

// Function to allocate resources to a process
bool allocateResources(int process_id, int* request, int num_resources) {
    if (isRequestValid(process_id, request, num_resources)) {
        // Allocate the requested resources to the process
        for (int i = 0; i < num_resources; i++) {
            allocation[process_id][i] += request[i];
            available[i] -= request[i];
        }

        return true;
    } else {
        return false;
    }
}

int main() {
    // Initialize the allocation, maximum, and available matrices
    // For example, if there are two resources R1 and R2 and three processes P1, P2, and P3:
    // allocation[0][0] = 0 means that process P1 currently has 0 units of resource R1
    // maximum[1][1] = 5 means that process P2 needs at most 5 units of resource R2
    // available[0] = 10 means that there are currently 10 units of resource R1 available in the system
    // Note that all matrices should be initialized before any allocation is made
    // to ensure that the system is in a safe state
    // Also note that MAX_RESOURCES and MAX_PROCESSES should be set according to the actual number of resources and processes in the system

    // Example usage:
    int request[2] = {3, 4};
    int process_id = 0;
    int num_resources = 2;

    if (allocateResources(process_id, request, num_resources)) {
        printf("Request granted\n");
    } else {
        printf("Request denied\n");
    }

    return 0;
}