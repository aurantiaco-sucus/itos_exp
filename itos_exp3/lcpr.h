//
// Lightweight Cross Platform Randomness
//

#ifndef ITOS_EXP3_LCPR_H
#define ITOS_EXP3_LCPR_H

// POSIX compliant environment
#ifdef __unix__
#define _LCPR_SYS_POSIX_COMPLIANT
#endif

#ifdef __APPLE__
#define _LCPR_SYS_POSIX_COMPLIANT
#endif

#ifdef __linux__
#define _LCPR_SYS_POSIX_COMPLIANT
#endif

// Windows NT environment
#ifdef _WIN32
#define _LCPR_SYS_WINDOWS_NT
#endif

#ifdef _WIN64
#define _LCPR_SYS_WINDOWS_NT
#endif

#if defined(_LCPR_SYS_POSIX_COMPLIANT)

#include <stdlib.h>
#include <time.h>

void lcpr_init_time() {
    srand48((long) time(NULL));
}

int lcpr_rand_int_bounded(int upper_bound) {
    return (int) (drand48() * (double) upper_bound);
}

#elif defined(_LCPR_SYS_WINDOWS_NT)

#include <time.h>
#include <windows.h>

void lcpr_init_time() {
    srand((unsigned int) time(NULL));
}

int lcpr_rand_int_bounded(int upper_bound) {
    double randomness = ((double) rand()) / ((double) RAND_MAX);
    return (int) (randomness * (double) upper_bound);
}

#endif

#endif //ITOS_EXP3_LCPR_H
