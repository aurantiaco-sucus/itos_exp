#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "lcpr.h"

#define loop while(true)

// --- Default Configuration ---
//const int ins_count = 320;
//const int ins_per_page = 10;
//const int mem_size_min = 4;
//const int mem_size_max = 32;

#define ins_count 320
#define ins_per_page 10
#define mem_size_min 4
#define mem_size_max 32

int prophecy[ins_count];
int mem_size;
int *mem_pages;

void init_prophecy() {
#define tick_tock index += 1; if (index == ins_count) break;
    int index = 0;
    loop {
        prophecy[index] = lcpr_rand_int_bounded(ins_count);
        tick_tock
        prophecy[index] = prophecy[index - 1] + 1;
        tick_tock
        prophecy[index] = lcpr_rand_int_bounded(prophecy[index - 1]);
        tick_tock
        prophecy[index] = prophecy[index - 1] + 1;
        tick_tock
        prophecy[index] = prophecy[index - 1] + lcpr_rand_int_bounded(ins_count - prophecy[index - 1]);
        tick_tock
    }
}

void init_memory(int size) {
    mem_size = size;
    mem_pages = malloc(sizeof(int) * mem_size);
    for (int i = 0; i < mem_size; ++i) {
        mem_pages[i] = -1;
    }
}

#define page_of_ins(addr) (addr / ins_per_page)
#define foreach_memory(index) for (int index = 0; index < mem_size; ++index)
#define foreach_prophecy(index) for (int index = 0; index < ins_count; ++index)

bool impl_page_in_memory(int page) {
    for (int i = 0; i < mem_size; ++i) {
        if (mem_pages[i] == page) return true;
    }
    return false;
}
#define page_in_memory(n) impl_page_in_memory(n)

#define simulate(func) \
    init_memory(mem_size); \
    func(result); \
    free(mem_pages)

void fifo(FILE *result) {
    int pf_count = 0;
    int head = 0;
    foreach_prophecy(pc) {
        int page = page_of_ins(prophecy[pc]);
        if (!page_in_memory(page)) {
            // Page fault
            pf_count += 1;
            mem_pages[head] = page;
            head = (head + 1) % mem_size;
        } else {
            // Page hit
        }
    }
    fprintf(result, "fifo,%d,%d,%d\n", mem_size, ins_count - pf_count, ins_count);
}

#define foreach_recency(index) for (int index = 0; index < mem_size; ++index)
void lru(FILE *result) {
    int recency[mem_size];
    int pf_count = 0;
    foreach_recency(i) {
        recency[i] = -1;
    }
    foreach_prophecy(pc) {
        int page = page_of_ins(prophecy[pc]);
        if (!page_in_memory(page)) {
            // Page fault
            pf_count += 1;
            // Find the least recently used page
            int lru = 0;
            int lru_index = 0;
            foreach_recency(i) {
                if (recency[i] > lru) {
                    lru = recency[i];
                    lru_index = i;
                }
            }
            mem_pages[lru_index] = page;
            recency[lru_index] = 0;
            foreach_recency(i) {
                if (i != lru_index) {
                    recency[i] += 1;
                }
            }
        } else {
            // Page hit
            foreach_recency(i) {
                if (mem_pages[i] == page) {
                    recency[i] = 0;
                } else {
                    recency[i] += 1;
                }
            }
        }
    }
    fprintf(result, "lru,%d,%d,%d\n", mem_size, ins_count - pf_count, ins_count);
}

void opt(FILE *result) {
    int pf_count = 0;
    foreach_prophecy(pc) {
        int page = page_of_ins(prophecy[pc]);
        if (!page_in_memory(page)) {
            // Page fault
            pf_count += 1;
            // Figure out the farthest page to be used
            int farthest = 0;
            int farthest_index = 0;
            foreach_memory(i) {
                int j = pc + 1;
                while (j < ins_count) {
                    if (page_of_ins(prophecy[j]) == mem_pages[i]) {
                        if (j > farthest) {
                            farthest = j;
                            farthest_index = i;
                        }
                        break;
                    }
                    j += 1;
                }
                if (j == ins_count) {
                    farthest = j;
                    farthest_index = i;
                }
            }
            mem_pages[farthest_index] = page;
        } else {
            // Page hit
        }
    }
    fprintf(result, "opt,%d,%d,%d\n", mem_size, ins_count - pf_count, ins_count);
}

int main() {
    // Open result file
    FILE *result = fopen("result.txt", "w");
    // Initialize
    lcpr_init_time();
    init_prophecy();

    for (int size = mem_size_min; size <= mem_size_max; ++size) {
        mem_size = size;
        printf("Memory size: %d\n", size);

        printf("Simulating FIFO...\n");
        simulate(fifo);
        printf("Simulating LRU...\n");
        simulate(lru);
        printf("Simulating OPT...\n");
        simulate(opt);
    }

    // Finalize
    fclose(result);
    return 0;
}