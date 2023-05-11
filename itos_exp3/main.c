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
#define each_cache(index) for (int index = 0; index < cache_size; ++index)

bool impl_page_present_in_cache(const int *cache, int cache_size, int page) {
    for (int i = 0; i < cache_size; ++i) {
        if (cache[i] == page) return true;
    }
    return false;
}
#define present(n) impl_page_present_in_cache(cache, cache_size, n)

#define init_handler(name) void * name (int cache_size)
typedef void * (*init_handler_t)(int);

#define hit_handler(name) void name (void *state, const int *cache, int cache_size, const int *prophecy, int req_page)
typedef void (*hit_handler_t)(void *, const int *, int, const int *, int);

#define miss_handler(name) void name (void *state, int *cache, int cache_size, const int *prophecy, int req_page)
typedef void (*miss_handler_t)(void *, int *, int, const int *, int);

#define destructor(name) void name (void *state)
typedef void (*destructor_t)(void *);

init_handler(fifo_init) {
    int *head = malloc(sizeof(int));
    *head = 0;
    return (void *) head;
}

hit_handler(fifo_hit) {}

miss_handler(fifo_miss) {
    // Replace the page at the head of the queue
    int *head = (int *) state;
    cache[*head] = req_page;
    *head = (*head + 1) % cache_size;
}

destructor(fifo_destroy) {
    free(state);
}

init_handler(noop_init) {return NULL;}

hit_handler(noop_hit) {}

miss_handler(noop_miss) {}

destructor(noop_destroy) {}

init_handler(lru_init) {
    int *usage = malloc(sizeof(int) * cache_size);
    for (int i = 0; i < cache_size; ++i) {
        usage[i] = 0;
    }
    return (void *) usage;
}

hit_handler(lru_hit) {
    // Increment usage of all pages except the one that was hit
    int *usage = (int *) state;
    for (int i = 0; i < cache_size; ++i) {
        if (cache[i] != req_page) {
            usage[i] += 1;
        } else {
            usage[i] = 0;
        }
    }
}

miss_handler(lru_miss) {
    // Find the least recently used page
    int *usage = (int *) state;
    int lru_index = 0;
    for (int i = 1; i < cache_size; ++i) {
        if (usage[i] > usage[lru_index]) {
            lru_index = i;
        }
    }
    cache[lru_index] = req_page;
    usage[lru_index] = 0;
    // Increment usage of all other pages
    for (int i = 0; i < cache_size; ++i) {
        if (i != lru_index) {
            usage[i] += 1;
        }
    }
}

destructor(lru_destroy) {
    free(state);
}

init_handler(opt_init) {
    return NULL;
}

hit_handler(opt_hit) {}

miss_handler(opt_miss) {
    int farthest_distance = -1;
    int farthest_index = -1;
    for (int i = 0; i < cache_size; ++i) {
        int distance = 0;
        for (int j = req_page + 1; j < ins_count; ++j) {
            if (cache[i] == page_of(prophecy[j])) {
                break;
            }
            ++distance;
        }
        if (distance > farthest_distance) {
            farthest_distance = distance;
            farthest_index = i;
        }
    }
    cache[farthest_index] = req_page;
}

destructor(opt_destroy) {}

void simulate(const int *prophecy,
              const char *name,
              init_handler_t init,
              hit_handler_t hit,
              miss_handler_t miss,
              destructor_t destroy,
              int cache_size) {
    int hit_count = 0;
    int *cache = malloc(sizeof(int) * cache_size);
    for (int i = 0; i < cache_size; ++i) {
        cache[i] = -1;
    }
    void *state = init(cache_size);
    for (int i = 0; i < ins_count; ++i) {
        int req_page = page_of(prophecy[i]);
        if (present(req_page)) {
            ++hit_count;
            hit(state, cache, cache_size, prophecy, req_page);
        } else {
            miss(state, cache, cache_size, prophecy, req_page);
        }
    }
    free(cache);
    destroy(state);
    printf("%s,%d,%d,%d\n", name, cache_size, hit_count, ins_count);
}

int main() {
    FILE *result = fopen("result.txt", "w");
    freopen("result.txt", "w", stdout);
    int prophecy[ins_count];
    lcpr_init_time();
    populate_prophecy(prophecy);

    for (int size = cache_size_min; size <= cache_size_max; ++size) {
        simulate(prophecy, "noop", noop_init, noop_hit, noop_miss, noop_destroy, size);
        simulate(prophecy, "opt", opt_init, opt_hit, opt_miss, opt_destroy, size);
        simulate(prophecy, "fifo", fifo_init, fifo_hit, fifo_miss, fifo_destroy, size);
        simulate(prophecy, "lru", lru_init, lru_hit, lru_miss, lru_destroy, size);
        printf("\n");
    }
    fclose(result);
    return 0;
}