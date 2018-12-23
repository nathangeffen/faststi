#ifndef FSTI_DEFS_H
#define FSTI_DEFS_H

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <glib.h>

#include "utils/utils.h"

/* Technical limits */
#define FSTI_HASHSIZE 101
#define FSTI_KEY_LEN 30
#define FSTI_DESC_LEN 200
#define FSTI_TOKEN_LEN 200
#define FSTI_DATE_LEN 11

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

#define FSTI_MINUTE 1
#define FSTI_HOUR 60
#define FSTI_DAY 1440
#define FSTI_WEEK 10080
#define FSTI_MONTH 43830
#define FSTI_YEAR 525949
#define FSTI_5_YEAR FSTI_YEAR * 5


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
                                     long double: LDBL,                 \
                                     struct fsti_date: INT)

#define FSTI_GET_TRANSFORMER(var)  _Generic((var),                      \
                                            _Bool: fsti_to_bool,        \
                                            unsigned char: fsti_to_uchar, \
                                            uint16_t: fsti_to_uint16_t, \
                                            uint32_t: fsti_to_uint32_t, \
                                            uint64_t: fsti_to_uint64_t, \
                                            int: fsti_to_int,           \
                                            float: fsti_to_float,       \
                                            double: fsti_to_double,     \
                                            struct fsti_date: fsti_to_int)

#define FSTI_AGENT_ELEM_ENTRY(member)                           \
    {                                                           \
        #member,                                                \
        offsetof(struct fsti_agent, member),                    \
        FSTI_GET_TYPE(fsti_thread_local_agent.member),          \
        FSTI_GET_TRANSFORMER(fsti_thread_local_agent.member)    \
    }

struct fsti_date {
    union {
        int32_t date;
        struct {
            int32_t year:14;
            int32_t month:6;
            int32_t day:6;
        };
    };
};

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
                                    const struct fsti_simulation *simulation,
                                    struct fsti_agent *agent);

extern bool fsti_keep_test_files;

void fsti_to_float(void *to, const struct fsti_variant *from,
                   const struct fsti_simulation *simulation,
                   struct fsti_agent *agent);
void fsti_to_double(void *to, const struct fsti_variant *from,
                   const struct fsti_simulation *simulation,
                    struct fsti_agent *agent);
void fsti_to_age(void *to, const struct fsti_variant *from,
                 const struct fsti_simulation *simulation,
                 struct fsti_agent *agent);
void fsti_to_int(void *to, const struct fsti_variant *from,
                   const struct fsti_simulation *simulation,
                 struct fsti_agent *agent);
void fsti_to_uint8_t(void *to, const struct fsti_variant *from,
                      const struct fsti_simulation *simulation,
                     struct fsti_agent *agent);
void fsti_to_uint16_t(void *to, const struct fsti_variant *from,
                      const struct fsti_simulation *simulation,
                      struct fsti_agent *agent);
void fsti_to_uint32_t(void *to, const struct fsti_variant *from,
                      const struct fsti_simulation *simulation,
                      struct fsti_agent *agent);
void fsti_to_int32_t(void *to, const struct fsti_variant *from,
                      const struct fsti_simulation *simulation,
                      struct fsti_agent *agent);
void fsti_to_uint64_t(void *to, const struct fsti_variant *from,
                      const struct fsti_simulation *simulation,
                      struct fsti_agent *agent);
void fsti_to_int64_t(void *to, const struct fsti_variant *from,
                      const struct fsti_simulation *simulation,
                      struct fsti_agent *agent);
void fsti_to_bool(void *to, const struct fsti_variant *from,
                  const struct fsti_simulation *simulation,
                  struct fsti_agent *agent);
void fsti_to_unsigned(void *to, const struct fsti_variant *from,
                   const struct fsti_simulation *simulation,
                      struct fsti_agent *agent);
void fsti_to_uchar(void *to, const struct fsti_variant *from,
                   const struct fsti_simulation *simulation,
                   struct fsti_agent *agent);
void fsti_to_size_t(void *to, const struct fsti_variant *from,
                     const struct fsti_simulation *simulation,
                    struct fsti_agent *agent);
void fsti_to_partner(void *to, const struct fsti_variant *from,
                     const struct fsti_simulation *simulation,
                     struct fsti_agent *agent);

void fsti_cnv_vals(void *dest, const void *src,
                   enum fsti_type dest_type, enum fsti_type src_type);
size_t fsti_hash(const char *str);
struct fsti_variant fsti_identify_token(char *token);
struct fsti_variant fsti_identify_token_const(const char *token);
int fsti_variant_print(FILE *f, const struct fsti_variant *variant);
struct fsti_julian_date fsti_time_set_julian(uint16_t year, uint16_t day);
char *fsti_time_sprint(struct fsti_date date);
struct fsti_date
fsti_time_add_gdatetime(GDateTime *base, int32_t steps, int32_t step_size);
uint16_t fsti_time_in_years(int32_t t);
char *fsti_make_full_data_filename(const char *filename);
FILE *fsti_open_data_file(const char *filename, const char *mode);
void fsti_remove_file(const char *filename);
void fsti_remove_data_file(const char *filename);
void fsti_test_defs(struct test_group *tg);




#endif
