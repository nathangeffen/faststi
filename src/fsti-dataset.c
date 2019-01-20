#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "fsti-dataset.h"

#define max(a, b) (a) > (b) ? (a) : (b)

static void alloc_dataset(struct fsti_dataset *dataset,
                          size_t rows, size_t cols)
{
    dataset->members = malloc(sizeof(*dataset->members) * cols);
    FSTI_ASSERT(dataset->members, FSTI_ERR_NOMEM, NULL);
    dataset->min_vals = malloc(sizeof(*dataset->min_vals) * cols);
    FSTI_ASSERT(dataset->min_vals, FSTI_ERR_NOMEM, NULL);
    dataset->max_vals = malloc(sizeof(*dataset->max_vals) * cols);
    FSTI_ASSERT(dataset->max_vals, FSTI_ERR_NOMEM, NULL);
    dataset->divisors = malloc(sizeof(*dataset->divisors) * cols);
    FSTI_ASSERT(dataset->divisors, FSTI_ERR_NOMEM, NULL);
    dataset->multiplicands = malloc(sizeof(*dataset->multiplicands) * cols);
    FSTI_ASSERT(dataset->multiplicands, FSTI_ERR_NOMEM, NULL);
    dataset->dependents = malloc(sizeof(*dataset->dependents) *
                                 rows * dataset->num_dependents);
    FSTI_ASSERT(dataset->dependents, FSTI_ERR_NOMEM, NULL);
    dataset->next = NULL;
}

void fsti_print_dataset(struct fsti_dataset *ds)
{
    printf("Filename: %s\n", ds->filename);
    printf("Num independents: %zu\n", ds->num_independents);
    printf("Num dependents: %zu\n", ds->num_dependents);
    printf("Number of entries (rows excluding header): %zu\n", ds->entries);
    printf("Column at which 2nd agent starts (0 if only one agent: %zu\n",
           ds->second_agent);
    if (ds->num_independents) {
        printf("Minimum values: %u", ds->min_vals[0]);
        for (size_t i = 1; i < ds->num_independents; ++i)
            printf(", %u", ds->min_vals[i]);
        printf("\nMaximum values: %u", ds->max_vals[0]);
        for (size_t i = 1; i < ds->num_independents; i++)
            printf(", %u", ds->max_vals[i]);
        printf("\nMultiplicands: %u", ds->multiplicands[0]);
        for (size_t i = 1; i < ds->num_independents; ++i)
            printf(", %u", ds->multiplicands[i]);
        printf("\n");
    }
    printf("Dependent values: 0 %f", ds->dependents[0]);
    for (size_t i = 1; i < ds->entries; i++)
        printf(",%zu %f", i, ds->dependents[i]);
    printf("\n");
}

static size_t get_index(unsigned multiplicands[], unsigned indices[], size_t n)
{
    size_t index = 0;
    for (size_t i = 0; i < n; ++i) {
        index += multiplicands[i] * indices[i];
    }
    return index;
}

static void set_dependents(struct fsti_dataset *dataset,
                           const struct csv *cs, size_t rows, size_t cols)
{
    for (size_t i = 0; i < dataset->num_dependents; i++) {
        for (size_t j = 0; j < rows; j++) {
            size_t col_no = dataset->num_independents + i;
            struct fsti_variant variant =
                fsti_identify_token_const(csv_at(cs, j, col_no));
            FSTI_ASSERT(variant.type == DBL || variant.type == LONG,
                    FSTI_ERR_WRONG_TYPE,
                        fsti_sprintf("%s line: %zu col %zu",
                                     dataset->filename, j, col_no));
            if (variant.type == DBL) {
                dataset->dependents[i * rows + j] = variant.value.dbl;
            } else {
                dataset->dependents[i * rows + j] = variant.value.longint;
            }
        }
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
                fsti_sprintf("Value: %s File: %s line: %zu col %zu",
                             csv_at(cs, row, col), filename, row+2, col+1));
    return variant;
}

static size_t calc_dependents(struct fsti_dataset *ds,
                              const struct csv *cs)
{
    size_t result;
    char **split_values, **it;

    split_values = g_strsplit(cs->header.cells[cs->header.len - 1], "|", -1);

    it = split_values;

    FSTI_ASSERT(it && *it, FSTI_ERR_DATASET_FILE,
                fsti_sprintf("Header field error in file: %s Field: %s",
                             ds->filename, cs->header.cells[cs->header.len - 1]));

    ++it;
    if (it && *it) {
        result = atoi(*it);
        FSTI_ASSERT(result && result <= cs->rows[0].len, FSTI_ERR_DATASET_FILE,
                FSTI_MSG("More dependents specified than columns", ds->filename));
    } else {
        result = 1;
    }

    g_strfreev(split_values);

    return result;
}


static unsigned parse_divisor(char *s)
{
    char num_part[12];
    size_t n;
    size_t i = 0;
    char *c;
    unsigned val;

    n = strlen(s);
    for (c = s; c < s + n && i < 12; c++) {
        if (*c >= '0' && *c <= '9')
            num_part[i++] = *c;
        else
            break;
    }
    FSTI_ASSERT(i == 0 || *c == 0 || *c == '-', FSTI_ERR_DATASET_FILE,
                fsti_sprintf("Unknown denominator: %s", s));

    if (i > 0) {
        num_part[i] = 0;
        val = atoi(num_part);
    } else {
        val = 1;
    }

    if (i == 0 || *c++ == '-') {
        if (strcmp(c, "MINUTE") == 0)
            val *= FSTI_MINUTE;
        else  if (strcmp(c, "HOUR") == 0)
            val *= FSTI_HOUR;
        else  if (strcmp(c, "DAY") == 0)
            val *= FSTI_DAY;
        else  if (strcmp(c, "WEEK") == 0)
            val *= FSTI_WEEK;
        else  if (strcmp(c, "MONTH") == 0)
            val *= FSTI_MONTH;
        else  if (strcmp(c, "YEAR") == 0)
            val *= FSTI_YEAR;
        else
            FSTI_ASSERT(0, FSTI_ERR_DATASET_FILE,
                        fsti_sprintf("Unknown denominator: %s", s));
    }
    return val;
}

static void parse_header_field(struct fsti_dataset *ds,
                               const struct csv *cs,
                               size_t col)
{
    char **split_values, **it;

    split_values = g_strsplit(cs->header.cells[col], "|", -1);
    it = split_values;
    FSTI_ASSERT(it, FSTI_ERR_DATASET_FILE,
                fsti_sprintf("Header field error in file: %s Field: %s",
                             ds->filename, cs->header.cells[col]));

    ds->members[col] = fsti_agent_elem_by_strname(*it);
    it++;
    if (it && *it) { // divisor
        ds->divisors[col] = parse_divisor(*it);
        it++; // check if dataset caters for two agents
        if (*it && strcmp(*it, "~") == 0) ds->second_agent = col;
    } else {
        ds->divisors[col] = 1;
    }
    ds->min_vals[col] = csv_val(cs, 0, col, LONG, ds->filename).value.longint;
    ds->max_vals[col] = csv_val(cs, cs->len - 1,
                                col, LONG, ds->filename).value.longint;
    g_strfreev(split_values);
}

static void convert_csv_to_dataset(struct fsti_dataset *dataset,
                                   const struct csv *cs,
                                   const char *filename)
{
    size_t rows, cols, max_index;

    strncpy(dataset->filename, filename, FSTI_FILENAME_LEN);
    dataset->filename[FSTI_FILENAME_LEN] = 0;
    FSTI_ASSERT(cs->header.len, FSTI_ERR_DATASET_FILE,
                FSTI_MSG("No header in dataset", filename));
    FSTI_ASSERT(cs->len > 0, FSTI_ERR_DATASET_FILE,
                FSTI_MSG("Too few rows in dataset", filename));
    FSTI_ASSERT(cs->rows[0].len > 0, FSTI_ERR_DATASET_FILE,
                FSTI_MSG("Too few columns in dataset", filename));


    dataset->second_agent = 0; // By default only 1 agent processed
    dataset->entries = rows = cs->len;
    dataset->num_dependents = calc_dependents(dataset, cs);
    dataset->num_independents = cols = cs->rows[0].len - dataset->num_dependents;
    alloc_dataset(dataset, rows, cols);

    for (size_t i = 0; i < cols; i++)
        parse_header_field(dataset, cs, i);

    if (cols > 0) {
        dataset->multiplicands[cols - 1] = 1;
        if (cols > 1) {
            for (size_t i = cols - 1; i-- > 0;) {
                dataset->multiplicands[i] =
                    (dataset->max_vals[i + 1] - dataset->min_vals[i + 1] + 1) *
                    dataset->multiplicands[i + 1];
            }
        }
    }
    max_index = get_index(dataset->multiplicands, dataset->max_vals,
                                 dataset->num_independents);
    FSTI_ASSERT(max_index + 1 == rows, FSTI_ERR_DATASET_FILE,
                fsti_sprintf("%s: Number of lines expected is %zu",
                             filename, max_index));
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
                           struct fsti_agent *agent, size_t col)
{
    size_t i, index = 0, n = dataset->num_independents;
    unsigned val, vals[n];

    for (i = 0; i < n; i++) {
        val = (unsigned) fsti_agent_elem_val_l(dataset->members[i], agent);
        val /= dataset->divisors[i];
        vals[i] = (val > dataset->max_vals[i]) ? dataset->max_vals[i] : val;
    }
    index = get_index(dataset->multiplicands, vals, n);
    return dataset->dependents[col * dataset->entries + index];
}

double fsti_dataset_lookup0(struct fsti_dataset *dataset,
                            struct fsti_agent *agent)
{
    return fsti_dataset_lookup(dataset, agent, 0);
}


double fsti_dataset_lookup_x2(struct fsti_dataset *dataset,
                              struct fsti_agent *a,
                              struct fsti_agent *b,
                              size_t col)
{
    size_t i, index = 0, n = dataset->second_agent;
    unsigned val, vals[n];

    for (i = 0; i < n; i++) {
        val = (unsigned) fsti_agent_elem_val_l(dataset->members[i], a);
        val /= dataset->divisors[i];
        vals[i] = (val > dataset->max_vals[i]) ? dataset->max_vals[i] : val;
    }
    for (; i < dataset->num_independents; i++) {
        val = (unsigned) fsti_agent_elem_val_l(dataset->members[i], b);
        val /= dataset->divisors[i];
        vals[i] = (val > dataset->max_vals[i]) ? dataset->max_vals[i] : val;
    }
    index = get_index(dataset->multiplicands, vals, dataset->num_independents);
    return dataset->dependents[col * dataset->entries + index];
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
    unsigned i, j, k, c;
    struct fsti_agent agent;
    FILE *f;
    const char *filename = "fsti_test_dataset_in_1234.csv";
    struct fsti_dataset dataset;

    // Test  parsing of divisors
    char *s = "525948";
    unsigned x = parse_divisor(s);
    TESTEQ(x, 525948, *tg);
    s = "YEAR";
    x = parse_divisor(s);
    TESTEQ(x, FSTI_YEAR, *tg);
    s = "5-YEAR";
    x = parse_divisor(s);
    TESTEQ(x, 5 * FSTI_YEAR, *tg);
    s = "200-YEAR";
    x = parse_divisor(s);
    TESTEQ(x, 200 * FSTI_YEAR, *tg);

    // Test with only 1 column (the minimum needed)
    f = fsti_open_data_file(filename, "w");
    FSTI_ASSERT(f, FSTI_ERR_DATASET_FILE, FSTI_MSG("Could not open ", filename));
    fprintf(f, "0\n0.5\n");
    fclose(f);

    fsti_dataset_read(filename, &dataset, ';');

    double d = fsti_dataset_lookup0(&dataset, &agent);
    TESTEQ(d, 0.5, *tg);
    fsti_dataset_free(&dataset);

    // Test with 4 columns and comments
    f = fsti_open_data_file(filename, "w");
    FSTI_ASSERT(f, FSTI_ERR_DATASET_FILE, FSTI_MSG("Could not open ", filename));
    fprintf(f, "# spurious comment 1\n");
    fprintf(f, "# spurious comment 2\n");
    fprintf(f, "age|20;sex;sex_preferred;0\n");
    fprintf(f, "# spurious comment 3\n");

    for (i = 0; i < 5; i++)
        for (j = 0; j < 2; j++)
            for (k = 0; k < 2; k++) {
                fprintf(f, "%u;%u;%u;%f\n", i, j, k,
                        (i * 4 + j * 2 + k) / 20.0);
                fprintf(f, "# spurious comment\n");
            }

    fprintf(f, "# spurious comment 5\n");

    fclose(f);


    fsti_dataset_read(filename, &dataset, ';');

    for (i = 0; i < 5; i++)
        for (j = 0; j < 2; j++)
            for (k = 0; k < 2; k++) {
                agent.age = i * 20.5;
                agent.sex = j;
                agent.sex_preferred = k;
                double d = fsti_dataset_lookup0(&dataset, &agent);
                double result = (i * 4 + j * 2 + k) / 20.0;
                TESTEQ(d, result, *tg);
            }
    fsti_dataset_free(&dataset);


    // Test with 5 columns, 2 of which are dependents
    f = fsti_open_data_file(filename, "w");
    FSTI_ASSERT(f, FSTI_ERR_DATASET_FILE, FSTI_MSG("Could not open ", filename));

    fprintf(f, "age|20;sex|1;sex_preferred;0;1|2\n");

    c = 0;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 2; j++)
            for (k = 0; k < 2; k++, c++)
                fprintf(f, "%u;%u;%u;%f;%f\n", i, j, k, 1.0 * c, 100.0 * c);

    fclose(f);

    fsti_dataset_read(filename, &dataset, ';');

    c = 0;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 2; j++)
            for (k = 0; k < 2; k++, c++) {
                agent.age = i * 20.5;
                agent.sex = j;
                agent.sex_preferred = k;
                double d = fsti_dataset_lookup(&dataset, &agent, 0);
                double result = c;
                TESTEQ(d, result, *tg);
                d = fsti_dataset_lookup(&dataset, &agent, 1);
                result = 100.0 * c;
                TESTEQ(d, result, *tg);
            }
    fsti_dataset_free(&dataset);

    // Test with 6 columns, 3 of which are dependents
    f = fsti_open_data_file(filename, "w");
    FSTI_ASSERT(f, FSTI_ERR_DATASET_FILE, FSTI_MSG("Could not open ", filename));

    fprintf(f, "age|20;sex;sex_preferred;0;1;2|3\n");

    c = 0;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 2; j++)
            for (k = 0; k < 2; k++, c++)
                fprintf(f, "%u;%u;%u;%f;%f;%f\n",
                        i, j, k, 1.0 * c, 100.0 * c, 1000.0 * c);

    fclose(f);

    fsti_dataset_read(filename, &dataset, ';');

    c = 0;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 2; j++)
            for (k = 0; k < 2; k++, c++) {
                agent.age = i * 20.5;
                agent.sex = j;
                agent.sex_preferred = k;
                double d = fsti_dataset_lookup(&dataset, &agent, 0);
                double result = c;
                TESTEQ(d, result, *tg);
                d = fsti_dataset_lookup(&dataset, &agent, 1);
                result = 100.0 * c;
                TESTEQ(d, result, *tg);
                d = fsti_dataset_lookup(&dataset, &agent, 2);
                result = 1000.0 * c;
                TESTEQ(d, result, *tg);
            }
    fsti_dataset_free(&dataset);


    fsti_remove_data_file(filename);
}

static void dataset_hash_test(struct test_group *tg)
{
    struct fsti_dataset_hash hash;
    char filename[20];
    const char *output =
        "age|1;sex;sex_preferred|1;infected\n"
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
        g_file_set_contents (fsti_make_full_data_filename(filename),
                             output, -1, NULL);
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
