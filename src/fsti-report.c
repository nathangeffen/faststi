#include "fsti-report.h"

int fsti_cmpf(const void *a, const void *b)
{
    float *x = (float *) a;
    float *y = (float *) b;

    if (*x < *y) return -1;
    if (*x > *y) return 1;
    return 0;
}
