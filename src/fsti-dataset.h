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
    size_t entries;
    size_t second_agent; // Column where 2nd agent fields begin. 0 for no 2nd.
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
    bool owner;
};

void fsti_print_dataset(struct fsti_dataset *ds);
void fsti_dataset_read(const char *filename, struct fsti_dataset *dataset,
                       char delim);
void fsti_dataset_read_buffer(const char *buffer,
                              struct fsti_dataset *dataset, char delim);
size_t fsti_dataset_lookup_index(const struct fsti_dataset *dataset,
                                 const struct fsti_agent *agent);
double fsti_dataset_get_by_index(const struct fsti_dataset *dataset,
                                 size_t index, size_t col);
double fsti_dataset_lookup0(const struct fsti_dataset *dataset,
                            const struct fsti_agent *agent);
double fsti_dataset_lookup(const struct fsti_dataset *dataset,
                           const struct fsti_agent *agent, size_t col);
double fsti_dataset_lookup_x2(const struct fsti_dataset *dataset,
                              const struct fsti_agent *a,
                              const struct fsti_agent *b,
                              size_t col);
void fsti_dataset_free(struct fsti_dataset *dataset);

void fsti_dataset_hash_init(struct fsti_dataset_hash *dataset_hash);
struct fsti_dataset *fsti_dataset_hash_find(const struct fsti_dataset_hash *dataset_hash, const char *filename);
struct fsti_dataset *fsti_dataset_hash_add(struct fsti_dataset_hash *dataset_hash, const char *filename, char delim);
void fsti_dataset_hash_copy(struct fsti_dataset_hash *dest,
                            const struct fsti_dataset_hash *src);
void fsti_dataset_hash_free(struct fsti_dataset_hash *hash);

void fsti_dataset_test(struct test_group *tg);

#endif
