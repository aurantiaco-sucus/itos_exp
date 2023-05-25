#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define TRACK_COUNT 1000
#define REQUEST_COUNT 1000

#define IS_VERBOSE false
#define ON_VERBOSE if (IS_VERBOSE)

int prophecy[REQUEST_COUNT];
int sequence[REQUEST_COUNT];
int init_track = 0;
int cur_track = 0;

void populate_prophecy() {
    srand48(time(NULL));
    for (int i = 0; i < REQUEST_COUNT; i++) {
        double rnd = drand48();
        prophecy[i] = (int) (rnd * TRACK_COUNT);
    }
    double rnd = drand48();
    init_track = (int) (rnd * TRACK_COUNT);
}

void populate_sequence() {
    for (int i = 0; i < REQUEST_COUNT; i++) {
        sequence[i] = prophecy[i];
    }
    cur_track = init_track;
}

#define ELEM_PER_LINE 10

void print_prophecy() {
    ON_VERBOSE printf("[Prophecy]\n");
    int cur_elem = 0;
    for (int i = 0; i < REQUEST_COUNT; i++) {
        ON_VERBOSE printf("%d\t", sequence[i]);
        cur_elem = (cur_elem + 1) % ELEM_PER_LINE;
        if (cur_elem == 0) {
            ON_VERBOSE printf("\n");
        }
    }
}

void fcfs() {
    ON_VERBOSE printf("[FCFS]\nTRACK\tDELTA\tTOTAL\n");
    int total_movement = 0;
    for (int i = 0; i < REQUEST_COUNT; i++) {
        int track = sequence[i];
        int movement = abs(track - cur_track);
        total_movement += movement;
        cur_track = track;
        ON_VERBOSE printf("%d\t%d\t%d\n", track, movement, total_movement);
    }
    printf("FCFS TOTAL: %d\n", total_movement);
}

int compare_req(const void *a, const void *b) {
    int req_a = *(int *) a;
    int req_b = *(int *) b;
    return req_a - req_b;
}

void sstf() {
    qsort(sequence, REQUEST_COUNT, sizeof(int), compare_req);
    ON_VERBOSE printf("[SSTF]\nTRACK\tDELTA\tTOTAL\n");
    int total_movement = 0;
    for (int i = 0; i < REQUEST_COUNT; i++) {
        int track = sequence[i];
        int movement = abs(track - cur_track);
        total_movement += movement;
        cur_track = track;
        ON_VERBOSE printf("%d\t%d\t%d\n", track, movement, total_movement);
    }
    printf("SSTF TOTAL: %d\n", total_movement);
}

int left_array[REQUEST_COUNT];
int left_count = 0;
int right_array[REQUEST_COUNT];
int right_count = 0;

void scan() {
    qsort(sequence, REQUEST_COUNT, sizeof(int), compare_req);
    for (int i = 0; i < REQUEST_COUNT; i++) {
        int req = sequence[i];
        if (req < cur_track) {
            left_array[left_count++] = req;
        } else {
            right_array[right_count++] = req;
        }
    }
    ON_VERBOSE printf("[SCAN]\nTRACK\tDELTA\tTOTAL\n");
    int total_movement = 0;
    bool forward = true;
    while (left_count > 0 || right_count > 0) {
        int track;
        if (forward) {
            if (right_count > 0) {
                track = right_array[--right_count];
            } else {
                forward = false;
                track = left_array[--left_count];
            }
        } else {
            if (left_count > 0) {
                track = left_array[--left_count];
            } else {
                forward = true;
                track = right_array[--right_count];
            }
        }
        int movement = abs(track - cur_track);
        total_movement += movement;
        cur_track = track;
        ON_VERBOSE printf("%d\t%d\t%d\n", track, movement, total_movement);
    }
    printf("SCAN TOTAL: %d\n", total_movement);
}

int main() {
    populate_prophecy();
    print_prophecy();

    populate_sequence();
    fcfs();

    populate_sequence();
    sstf();

    populate_sequence();
    scan();
    return 0;
}
