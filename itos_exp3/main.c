#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "lcpr.h"

#define loop while(true)

// --- Default Configuration ---
//const int ins_count = 320;
//const int ins_per_page = 10;
//const int cache_size_min = 4;
//const int cache_size_max = 32;

const int ins_count = 320;
const int ins_per_page = 10;
const int cache_size_min = 4;
const int cache_size_max = 32;

void populate_prophecy(int *seq) {
#define tick_tock index += 1; if (index == ins_count) break;
    int index = 0;
    loop {
        seq[index] = lcpr_rand_int_bounded(ins_count);
        tick_tock
        seq[index] = seq[index - 1] + 1;
        tick_tock
        seq[index] = lcpr_rand_int_bounded(seq[index - 1]);
        tick_tock
        seq[index] = seq[index - 1] + 1;
        tick_tock
        seq[index] = seq[index - 1] + lcpr_rand_int_bounded(ins_count - seq[index - 1]);
        tick_tock
    }
}

#define page_of(addr) (addr / ins_per_page)
#define evaluator(name) int name (const int *seq, int cache_size)
typedef int (*evaluator_t)(const int *, int);

evaluator(fifo) {
    int cache[cache_size];
    int head = 0;
    int miss_count = 0;
    for (int i = 0; i < ins_count; ++i) {
        int page = page_of(seq[i]);
        bool hit = false;
        for (int j = 0; j < cache_size; ++j) {
            if (cache[j] == page) {
                hit = true;
                break;
            }
        }
        if (!hit) {
            cache[head] = page;
            head = (head + 1) % cache_size;
            miss_count += 1;
        }
    }
    return ins_count - miss_count;
}

evaluator(opt) { // WRONG
    int cache[cache_size];
    int last_occurrence[ins_count];
    int miss_count = 0;
    for (int i = 0; i < ins_count; ++i) {
        last_occurrence[seq[i]] = i;
    }
    for (int i = 0; i < ins_count; ++i) {
        int page = page_of(seq[i]);
        bool hit = false;
        int opt_index = -1;
        for (int j = 0; j < cache_size; ++j) {
            if (cache[j] == page) {
                hit = true;
                break;
            }
            int next_occurrence = ins_count;
            for (int k = i + 1; k < ins_count; ++k) {
                if (seq[k] == cache[j]) {
                    next_occurrence = k;
                    break;
                }
            }
            if (next_occurrence > opt_index) {
                opt_index = j;
            }
        }
        if (!hit) {
            cache[opt_index] = page;
            miss_count += 1;
        }
    }
    return ins_count - miss_count;
}

evaluator(lru) {
    int cache[cache_size];
    int usage[cache_size];
    int miss_count = 0;
    for (int i = 0; i < cache_size; ++i) {
        cache[i] = -1;
        usage[i] = 0;
    }
    for (int i = 0; i < ins_count; ++i) {
        int page = page_of(seq[i]);
        bool hit = false;
        int lru_index = 0;
        for (int j = 0; j < cache_size; ++j) {
            if (cache[j] == page) {
                hit = true;
                usage[j] = i; // update usage time for hit
                break;
            } else if (cache[j] == -1) { // found empty slot
                cache[j] = page;
                usage[j] = i;
                miss_count += 1;
                hit = true;
                break;
            } else if (usage[j] < usage[lru_index]) { // find least recently used index
                lru_index = j;
            }
        }
        if (!hit) { // replace least recently used page
            cache[lru_index] = page;
            usage[lru_index] = i;
            miss_count += 1;
        }
    }
    return ins_count - miss_count;
}

evaluator(noop) {
    return 0; // No-op
}

void test(const int *seq, const char *name, evaluator_t eval, int cache_size) {
    int hit_count = eval(seq, cache_size);
    printf("R %s,%d,%d,%d\n", name, cache_size, hit_count, ins_count);
}

int main() {
    int seq[ins_count];
    lcpr_init_time();
    populate_prophecy(seq);
    printf("--- PROPHECY ---\n");
    for (int i = 0; i < ins_count; ++i) {
        printf("%d ", seq[i]);
    }
    printf("\n\n");

    for (int size = cache_size_min; size <= cache_size_max; ++size)
        test(seq, "noop", noop, size);
    for (int size = cache_size_min; size <= cache_size_max; ++size)
        test(seq, "opt", opt, size);
    for (int size = cache_size_min; size <= cache_size_max; ++size)
        test(seq, "fifo", fifo, size);
    for (int size = cache_size_min; size <= cache_size_max; ++size)
        test(seq, "lru", lru, size);
}