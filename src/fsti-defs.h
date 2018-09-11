#ifndef FSTI_DEFS_H
#define FSTI_DEFS_H

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define FSTI_KEY_LEN 30
#define FSTI_DESC_LEN 200

#define FSTI_AGENT_OFFSET(member)               \
    offsetof(struct fsti_agent, member)

#define FSTI_AGENT_DATA_OFFSET(structure, member)                       \
    offsetof(structure, member) + offsetof(struct fsti_agent, data)

#define FSTI_AGENT_FIELD(member, var_type ) \
    { FSTI_AGENT_OFFSET(member), var_type }

#define FSTI_AGENT_DATA_FIELD(structure, member, var_type ) \
    { FSTI_AGENT_DATA_OFFSET(structure, member), var_type }

enum fsti_struct_part {
    AGENT,
    DATA
};

enum fsti_type {
    NONE = -1,
    UNKNOWN = 0,
    CHAR,
    LONG,
    DBL,
    STR,
    BOOL,
    INT,
    UNSIGNED,
    UNKNOWN_COMMAND = 64,
    VARY,
    COVARY
};

struct fsti_agent_csv_entry {
    size_t offset;
    enum fsti_type type;
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

struct fsti_variant fsti_identify_token(char *token);

#endif
