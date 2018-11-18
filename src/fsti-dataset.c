#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

#include "fsti-dataset.h"

static void alloc_dataset(struct fsti_dataset *dataset,
                          size_t rows, size_t cols)
{
    dataset->members = malloc(sizeof(*dataset->members) * (cols + 1));
    FSTI_ASSERT(dataset->members, FSTI_ERR_NOMEM, NULL);
    dataset->min_vals = malloc(sizeof(*dataset->min_vals) * cols);
    FSTI_ASSERT(dataset->min_vals, FSTI_ERR_NOMEM, NULL);
    dataset->max_vals = malloc(sizeof(*dataset->max_vals) * cols);
    FSTI_ASSERT(dataset->max_vals, FSTI_ERR_NOMEM, NULL);
    dataset->divisors = malloc(sizeof(*dataset->divisors) * cols);
    FSTI_ASSERT(dataset->divisors, FSTI_ERR_NOMEM, NULL);
    dataset->multiplicands = malloc(sizeof(*dataset->multiplicands) * cols);
    FSTI_ASSERT(dataset->multiplicands, FSTI_ERR_NOMEM, NULL);
    dataset->dependents = malloc(sizeof(*dataset->dependents) * rows);
    FSTI_ASSERT(dataset->dependents, FSTI_ERR_NOMEM, NULL);
    dataset->next = NULL;
}

static size_t get_index(unsigned multiplicands[], unsigned indices[], size_t n)
{
    size_t index = 0;
    for (size_t i = 0; i < n; ++i)
        index += multiplicands[i] * indices[i];
    return index;
}

static void set_dependents(struct fsti_dataset *dataset,
                           const struct csv *cs, size_t rows, size_t cols)
{
    for (size_t i = 0; i < rows; i++) {
        struct fsti_variant variant =
            fsti_identify_token_const(csv_at(cs, i + 1, cols));
        FSTI_ASSERT(variant.type == DBL || variant.type == LONG,
                    FSTI_ERR_WRONG_TYPE,
                    fsti_sprintf("%s line: %zu col %zu", dataset->filename, 1, i));
        if (variant.type == DBL)
            dataset->dependents[i] = variant.value.dbl;
        else
            dataset->dependents[i] = variant.value.longint;
    }
}

static struct fsti_variant csv_val(const struct csv *cs,
                                   size_t row,
                                   size_t col,
                                   const enum fsti_type  expected,
                                   const char *filename)
{
    struct fsti_variant variant = fsti_identify_token_const(csv_at(cs, row, col));
    FSTI_ASSERT(variant.type == expected, FSTI_ERR_WRONG_TYPE,
                fsti_sprintf("%s line: %zu col %zu", filename, row+2, col+1));
    return variant;
}

static void convert_csv_to_dataset(struct fsti_dataset *dataset,
                                   const struct csv *cs,
                                   const char *filename)
{
    size_t rows, cols;

    strncpy(dataset->filename, filename, FSTI_FILENAME_LEN);
    dataset->filename[FSTI_FILENAME_LEN] = 0;
    FSTI_ASSERT(cs->header.len, FSTI_ERR_DATASET_FILE,
                FSTI_MSG("No header in dataset", filename));
    FSTI_ASSERT(cs->len > 1, FSTI_ERR_DATASET_FILE,
                FSTI_MSG("Too few rows in dataset", filename));
    FSTI_ASSERT(cs->rows[0].len > 1, FSTI_ERR_DATASET_FILE,
                FSTI_MSG("Too few columns in dataset", filename));

    dataset->num_dependents = rows = cs->len - 1;
    dataset->num_independents = cols = cs->rows[0].len - 1;
    alloc_dataset(dataset, rows, cols);

    for (size_t i = 0; i < cols; i++) {
        dataset->members[i] = fsti_agent_elem_by_strname(cs->header.cells[i]);
        dataset->divisors[i] = csv_val(cs, 0, i, LONG, filename).value.longint;
        dataset->min_vals[i] = csv_val(cs, 1, i, LONG, filename).value.longint;
        dataset->max_vals[i] = csv_val(cs, rows, i, LONG, filename).value.longint;
    }
    dataset->multiplicands[cols - 1] = 1;
    if (cols > 1)
        for (size_t i = cols - 1; i-- > 0;) {
            dataset->multiplicands[i] =
                (dataset->max_vals[i + 1] - dataset->min_vals[i + 1] + 1) *
                dataset->multiplicands[i + 1];
        }
    size_t max_index = get_index(dataset->multiplicands, dataset->max_vals,
                                 dataset->num_independents);
    FSTI_ASSERT(max_index + 1 == rows, FSTI_ERR_DATASET_FILE,
                fsti_sprintf("%s: Number of lines expected is %zu",
                             filename, max_index));
    dataset->members[cols] = fsti_agent_elem_by_strname(cs->header.cells[cols]);
    set_dependents(dataset, cs, rows, cols);
}

void fsti_dataset_read(const char *filename,
                       struct fsti_dataset *dataset, char delim)
{
    struct csv cs;
    FILE *f;

    f = fsti_open_data_file(filename, "r");
    FSTI_ASSERT(f, FSTI_ERR_DATASET_FILE, filename);

    cs = csv_read(f, true, delim);
    convert_csv_to_dataset(dataset, &cs, filename);
    csv_free(&cs);
}

double fsti_dataset_lookup(struct fsti_dataset *dataset,
                           struct fsti_agent *agent)
{
    size_t i, index = 0, n = dataset->num_independents;
    unsigned val, vals[n];

    for (i = 0; i < n; i++) {
        val = (unsigned) fsti_agent_elem_val_l(dataset->members[i], agent);
        val /= dataset->divisors[i];
        vals[i] = (val > dataset->max_vals[i]) ? dataset->max_vals[i] : val;
    }
    index = get_index(dataset->multiplicands, vals, n);
    return dataset->dependents[index];
}


void fsti_dataset_free(struct fsti_dataset *dataset)
{
    free(dataset->members);
    free(dataset->min_vals);
    free(dataset->max_vals);
    free(dataset->divisors);
    free(dataset->multiplicands);
    free(dataset->dependents);
}

void fsti_dataset_hash_init(struct fsti_dataset_hash *dataset_hash)
{
    memset(dataset_hash->datasets, 0,
           sizeof(struct fsti_dataset *) * FSTI_HASHSIZE);
}

size_t fsti_dataset_hash_count(struct fsti_dataset_hash *hash)
{
    struct fsti_dataset *entry;
    size_t count = 0;
    for (size_t i = 0; i < FSTI_HASHSIZE; ++i) {
        for(entry = hash->datasets[i]; entry; entry = entry->next)
            ++count;
    }
    return count;
}

struct fsti_dataset *
fsti_dataset_hash_find(const struct fsti_dataset_hash *dataset_hash,
                       const char *filename)
{
    struct fsti_dataset *np;

    size_t index = fsti_hash(filename);
    for (np = dataset_hash->datasets[index];
         np;
         np = np->next)
        if (strcmp(filename, np->filename) == 0)
            return np;
    return NULL;
}

struct fsti_dataset *
fsti_dataset_hash_add(struct fsti_dataset_hash *dataset_hash,
                      const char *filename, char delim)
{
    struct fsti_dataset *dataset;
    size_t hashval;

    dataset = fsti_dataset_hash_find(dataset_hash, filename);
    if (dataset) return dataset;

    dataset = malloc(sizeof(*dataset));
    FSTI_ASSERT(dataset, FSTI_ERR_NOMEM, NULL);
    fsti_dataset_read(filename, dataset, delim);
    hashval = fsti_hash(filename);
    dataset->next = dataset_hash->datasets[hashval];
    dataset_hash->datasets[hashval] = dataset;

    return dataset;
}

void fsti_dataset_hash_free(struct fsti_dataset_hash *hash)
{
    struct fsti_dataset *current, *prev;
    for (size_t i = 0; i < FSTI_HASHSIZE; ++i) {
        current = hash->datasets[i];
        while (current) {
            prev = current;
            current = current->next;
            fsti_dataset_free(prev);
            free(prev);
        }
    }
}

static void dataset_test(struct test_group *tg)
{
    unsigned i, j, k;
    struct fsti_agent agent;
    FILE *f;
    const char *filename = "fsti_test_dataset_in_1234.csv";
    struct fsti_dataset dataset;

    // Test with only 2 columns (the minimum needed)
    f = fsti_open_data_file(filename, "w");
    assert(f);
    fprintf(f, "age;infected\n");
    fprintf(f, "20;0\n");

    for (i = 0; i < 5; i++)
        fprintf(f, "%u;%f\n", i,
                (i * 4) / 20.0);
    fclose(f);

    fsti_dataset_read(filename, &dataset, ';');
    for (i = 0; i < 5; i++) {
        agent.age = i * 20.5;
        double d = fsti_dataset_lookup(&dataset, &agent);
        double result = (i * 4) / 20.0;
        TESTEQ(d, result, *tg);
    }
    fsti_dataset_free(&dataset);

    // Test with 4 columns
    f = fsti_open_data_file(filename, "w");
    assert(f);
    fprintf(f, "age;sex;sex_preferred;infected\n");
    fprintf(f, "20;1;1;0\n");


    for (i = 0; i < 5; i++)
        for (j = 0; j < 2; j++)
            for (k = 0; k < 2; k++)
                fprintf(f, "%u;%u;%u;%f\n", i, j, k,
                        (i * 4 + j * 2 + k) / 20.0);

    fclose(f);

    fsti_dataset_read(filename, &dataset, ';');
    for (i = 0; i < 5; i++)
        for (j = 0; j < 2; j++)
            for (k = 0; k < 2; k++) {
                agent.age = i * 20.5;
                agent.sex = j;
                agent.sex_preferred = k;
                double d = fsti_dataset_lookup(&dataset, &agent);
                double result = (i * 4 + j * 2 + k) / 20.0;
                TESTEQ(d, result, *tg);
            }
    fsti_dataset_free(&dataset);
}

static void dataset_hash_test(struct test_group *tg)
{
    struct fsti_dataset_hash hash;
    char filename[20];
    const char *output =
        "age;sex;sex_preferred;infected\n"
        "1;1;1;0\n"
        "0;0;0;0.1\n"
        "0;0;1;0.2\n"
        "0;1;0;0.3\n"
        "0;1;1;0.4\n"
        "1;0;0;0.5\n"
        "1;0;1;0.6\n"
        "1;1;0;0.7\n"
        "1;1;1;0.8\n";

    for (size_t i = 0; i < 3 * FSTI_HASHSIZE; ++i) {
        sprintf(filename, "%s%zu%s", "t_", i,".csv");
        g_file_set_contents (filename, output, -1, NULL);
    }

    fsti_dataset_hash_init(&hash);
    for (size_t i = 0; i < 3 * FSTI_HASHSIZE; ++i) {
        sprintf(filename, "%s%zu%s", "t_", i, ".csv");
        fsti_dataset_hash_add(&hash, filename, ';');
    }
    TESTEQ(fsti_dataset_hash_count(&hash), 3 * FSTI_HASHSIZE, *tg);

    size_t num_finds = 0;
    for (size_t i = 0; i < 3 * FSTI_HASHSIZE; ++i) {
        sprintf(filename, "%s%zu%s", "t_", i, ".csv");
        if (fsti_dataset_hash_find(&hash, filename)) ++num_finds;
    }
    TESTEQ(num_finds, 3 * FSTI_HASHSIZE, *tg);

    fsti_dataset_hash_free(&hash);

    // cleanup
    for (size_t i = 0; i < 3 * FSTI_HASHSIZE; ++i) {
        sprintf(filename, "%s%zu%s", "t_", i, ".csv");
        fsti_remove_data_file(filename);
    }
}

void fsti_dataset_test(struct test_group *tg)
{
    dataset_test(tg);
    dataset_hash_test(tg);
}
