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

const int ins_count = 32000;
const int ins_per_page = 100;
const int cache_size_min = 40;
const int cache_size_max = 320;

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
#define each_cache(index) for (int index = 0; index < cache_size; ++index)

bool _i_page_present_in_cache(int *cache, int cache_size, int page) {
    for (int i = 0; i < cache_size; ++i) {
        if (cache[i] == page) return true;
    }
    return false;
}
#define present(n) _i_page_present_in_cache(cache, cache_size, n)

#define evaluator(name) int name (const int *seq, int cache_size)
typedef int (*evaluator_t)(const int *, int);

#define init_handler(name) void * name ()
typedef void * (*init_handler_t)();

#define hit_handler(name) void name (void *state, int *cache, int cache_size, const int *prophecy, int req_page)
typedef void (*hit_handler_t)(void *, int *, int, const int *, int);

#define miss_handler(name) void name (void *state, int *cache, int cache_size, const int *prophecy, int req_page)
typedef void (*miss_handler_t)(void *, int *, int, const int *, int);

init_handler(fifo_init) {
    return (void *) 0;
}

hit_handler(fifo_hit) {}

miss_handler(fifo_miss) {
    cache[(size_t) state] = req_page;
    *((size_t *) state) += 1;
}

init_handler(noop_init) {
    return NULL;
}

hit_handler(noop_hit) {}

miss_handler(noop_miss) {}

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

evaluator(opt) {
    int cache[cache_size];
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
            int replace_index = -1;
            int farthest_distance = -1;
            for (int j = 0; j < cache_size; ++j) {
                int distance = 0;
                for (int k = i + 1; k < ins_count; ++k) {
                    if (cache[j] == page_of(seq[k])) {
                        break;
                    }
                    ++distance;
                }
                if (distance > farthest_distance) {
                    replace_index = j;
                    farthest_distance = distance;
                }
            }
            cache[replace_index] = page;
            miss_count += 1;
        }
    }
    return ins_count - miss_count;
}

void simulate(const int *prophecy,
                   const char *name,
                   init_handler_t init,
                   hit_handler_t hit,
                   miss_handler_t miss,
                   int cache_size) {
    int hit_count = 0;
    int cache[cache_size];
    for (int i = 0; i < cache_size; ++i) {
        cache[i] = -1;
    }
    void *state = init();
    for (int ins_index = 0; ins_index < ins_count; ++ins_index) {
        int req_page = page_of(ins_index);
        if (present(req_page)) {
            hit_count += 1;
            hit(state, cache, cache_size, prophecy, req_page);
        } else {
            miss(state, cache, cache_size, prophecy, req_page);
        }
    }
    printf("%s,%d,%d,%d\n", name, cache_size, hit_count, ins_count);
}

void test(const int *seq, const char *name, evaluator_t eval, int cache_size) {
    int hit_count = eval(seq, cache_size);
    printf("R %s,%d,%d,%d\n", name, cache_size, hit_count, ins_count);
}

int main() {
    int prophecy[ins_count];
    lcpr_init_time();
    populate_prophecy(prophecy);
//    printf("--- PROPHECY ---\n");
//    for (int i = 0; i < ins_count; ++i) {
//        printf("%d ", prophecy[i]);
//    }
//    printf("\n\n");

    for (int size = cache_size_min; size <= cache_size_max; ++size) {
        simulate(prophecy, "noop", noop_init, noop_hit, noop_miss, size);
        test(prophecy, "opt", opt, size);
        simulate(prophecy, "fifo", fifo_init, fifo_hit, fifo_miss, size);
        test(prophecy, "lru", lru, size);
        printf("\n");
    }

}