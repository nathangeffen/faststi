#ifndef FSTI_DATASET_H
#define FSTI_DATASET_H

#include "fsti-agent.h"
#include "fsti-defaults.h"
#include "utils/csv.h"

#define FSTI_FILENAME_LEN 32

struct fsti_dataset {
    char filename[FSTI_FILENAME_LEN + 1];
    size_t num_independents;
    size_t num_dependents;
    struct fsti_agent_elem **members;
    unsigned *min_vals;
    unsigned *max_vals;
    unsigned *divisors;
    unsigned *multiplicands;
    double *dependents;
};

void fsti_read_dataset(const char *filename, struct fsti_dataset *dataset,
                       char delim);
void fsti_dataset_test(struct test_group *tg);
void fsti_dataset_free(struct fsti_dataset *dataset);

#endif
