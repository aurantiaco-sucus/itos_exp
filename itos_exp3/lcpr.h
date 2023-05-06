//
// Lightweight Cross Platform Randomness
//

#ifndef ITOS_EXP3_LCPR_H
#define ITOS_EXP3_LCPR_H

#define _LCPR_SYS_POSIX_COMPLIANT() (defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L)

#ifdef _LCPR_SYS_POSIX_COMPLIANT // POSIX compliant environment

#include <stdlib.h>
#include <time.h>

void lcpr_init_time() {
    srand48((long) time(NULL));
}

int lcpr_rand_int_bounded(int upper_bound) {
    return (int) (drand48() * (double) upper_bound);
}

#else

#endif

#endif //ITOS_EXP3_LCPR_H
