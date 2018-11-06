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
    struct fsti_dataset *next;
};

struct fsti_dataset_hash {
    struct fsti_dataset *datasets[FSTI_HASHSIZE];
};

extern const char const *fsti_dataset_names[];

void fsti_dataset_add(struct fsti_dataset_hash *dataset_hash,
                      const char *filename);
void fsti_dataset_read(const char *filename, struct fsti_dataset *dataset,
                       char delim);
void fsti_dataset_test(struct test_group *tg);
void fsti_dataset_free(struct fsti_dataset *dataset);

#endif
