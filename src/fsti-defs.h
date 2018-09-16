#ifndef FSTI_DEFS_H
#define FSTI_DEFS_H

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define FSTI_KEY_LEN 30
#define FSTI_DESC_LEN 200

/* Useful constants */
#define FSTI_MALE 0
#define FSTI_FEMALE 1
#define FSTI_NONBINARY 2
#define FSTI_HETEROSEXUAL 0
#define FSTI_HOMOSEXUAL 1
#define FSTI_MSM 0
#define FSTI_MSW 1
#define FSTI_WSM 2
#define FSTI_WSW 3

#define FSTI_CSV_ENTRY(member, function) \
    {&fsti_global_agent.member, function},

enum fsti_struct_part {
    AGENT,
    DATA
};

enum fsti_type {
    NONE = -1,
    UNKNOWN = 0,
    CHAR,
    UCHAR,
    LONG,
    FLOAT,
    DBL,
    STR,
    BOOL,
    INT,
    UNSIGNED,
    UNKNOWN_COMMAND = 64,
    VARY,
    COVARY
};

struct fsti_agent;
struct fsti_simulation;

struct fsti_csv_agent {
    struct fsti_agent *agent;
    size_t num_entries;
    const struct fsti_csv_entry *entries;
};

union fsti_value {
    long longint;
    double dbl;
    char *str;
};

struct fsti_variant {
    union fsti_value value;
    enum fsti_type type;
};

struct fsti_variant_array {
    size_t len;
    size_t capacity;
    struct fsti_variant *variants;
};

typedef void (*fsti_transform_func)(void *to, const struct fsti_variant *from,
                                    struct fsti_simulation *simulation);

struct fsti_csv_entry {
     void *dest;
     fsti_transform_func transformer;
};

struct fsti_variant fsti_identify_token(char *token);

#endif
