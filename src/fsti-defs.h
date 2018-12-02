#ifndef FSTI_DEFS_H
#define FSTI_DEFS_H

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#include "utils/utils.h"

#define FSTI_HASHSIZE 101
#define FSTI_KEY_LEN 30
#define FSTI_DESC_LEN 200
#define FSTI_TOKEN_LEN 200

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

#define FSTI_NO_OP "_NO_OP"

#define FSTI_GET_TYPE(var)  _Generic((var),                             \
                                     _Bool: BOOL,                       \
                                     char: CHAR,                        \
                                     signed char: SCHAR,                \
                                     unsigned char: UCHAR,              \
                                     short: SHRT,                       \
                                     unsigned short: USHRT,             \
                                     int: INT,                          \
                                     unsigned int: UINT,                \
                                     long: LONG,                        \
                                     unsigned long int: ULONG,          \
                                     long long int: LLONG,              \
                                     unsigned long long int: ULLONG,    \
                                     float: FLT,                        \
                                     double: DBL,                       \
                                     long double: LDBL)

#define FSTI_GET_TRANSFORMER(var)  _Generic((var),                      \
                                            _Bool: fsti_to_bool,        \
                                            unsigned char: fsti_to_uchar, \
                                            uint16_t: fsti_to_uint16_t, \
                                            uint32_t: fsti_to_uint32_t, \
                                            uint64_t: fsti_to_uint64_t, \
                                            int: fsti_to_int,           \
                                            float: fsti_to_float,       \
                                            double: fsti_to_double)

#define FSTI_AGENT_ELEM_ENTRY(member)                           \
    {                                                           \
        #member,                                                \
        offsetof(struct fsti_agent, member),                    \
        FSTI_GET_TYPE(fsti_thread_local_agent.member),          \
        FSTI_GET_TRANSFORMER(fsti_thread_local_agent.member)    \
    }

enum fsti_struct_part {
    AGENT,
    DATA
};

enum fsti_type {
    NONE = -1,
    UNKNOWN = 0,
    CHAR,
    SCHAR,
    UCHAR,
    SHRT,
    USHRT,
    INT,
    UINT,
    LONG,
    ULONG,
    LLONG,
    ULLONG,
    FLT,
    DBL,
    LDBL,
    STR,
    BOOL,
    UINT8_T,
    UINT16_T,
    SIZE_T,
    UNKNOWN_COMMAND = 64,
    VARY,
    COVARY
};

struct fsti_dataset;
struct fsti_agent;
struct fsti_simulation;

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
                                    struct fsti_agent *agent);

void fsti_cnv_vals(void *dest, const void *src,
                   enum fsti_type dest_type, enum fsti_type src_type);
size_t fsti_hash(const char *str);
struct fsti_variant fsti_identify_token(char *token);
struct fsti_variant fsti_identify_token_const(const char *token);
int fsti_variant_print(FILE *f, const struct fsti_variant *variant);
char *fsti_make_full_data_filename(const char *filename);
FILE *fsti_open_data_file(const char *filename, const char *mode);
void fsti_remove_data_file(const char *filename);
void fsti_test_defs(struct test_group *tg);

/* These are declared in fsti-defs.h but defined in fsti-events.c
 * because they need to be visible in fsti-defaults.c which cannot
 * include fsti-agents.h or fsti-events.h.
 */
void fsti_to_float(void *to, const struct fsti_variant *from,
                   struct fsti_agent *agent);
void fsti_to_double(void *to, const struct fsti_variant *from,
                   struct fsti_agent *agent);
void fsti_to_int(void *to, const struct fsti_variant *from,
                   struct fsti_agent *agent);
void fsti_to_uint8_t(void *to, const struct fsti_variant *from,
                      struct fsti_agent *agent);
void fsti_to_uint16_t(void *to, const struct fsti_variant *from,
                      struct fsti_agent *agent);
void fsti_to_uint32_t(void *to, const struct fsti_variant *from,
                      struct fsti_agent *agent);
void fsti_to_uint64_t(void *to, const struct fsti_variant *from,
                      struct fsti_agent *agent);
void fsti_to_bool(void *to, const struct fsti_variant *from,
                  struct fsti_agent *agent);
void fsti_to_unsigned(void *to, const struct fsti_variant *from,
                   struct fsti_agent *agent);
void fsti_to_uchar(void *to, const struct fsti_variant *from,
                   struct fsti_agent *agent);
void fsti_to_size_t(void *to, const struct fsti_variant *from,
                     struct fsti_agent *agent);
void fsti_to_partner(void *to, const struct fsti_variant *from,
                     struct fsti_agent *agent);



#endif
