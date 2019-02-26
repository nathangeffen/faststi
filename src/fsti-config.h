#ifndef FSTI_CONFIG_H
#define FSTI_CONFIG_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/utils.h"

#include "fsti-defs.h"

#define FSTI_CONFIG_ADD_STR(config, elem, desc,  ...)                 \
    do {                                                               \
        const char * _vals[] = {__VA_ARGS__};                          \
        size_t _n = sizeof(_vals) / sizeof(char *);                    \
        fsti_config_add_strs(config, #elem, desc, _vals, _n);          \
    } while(0)

#define FSTI_CONFIG_ADD_DBL(config, elem, desc,  ...)                 \
    do {                                                               \
        double _vals[] = {__VA_ARGS__};                                \
        size_t _n = sizeof(_vals) / sizeof(double);                    \
        fsti_config_add_doubles(config, #elem, desc, _vals, _n);       \
    } while(0)

#define FSTI_CONFIG_ADD_LONG(config, elem, desc,  ...)                 \
    do {                                                                \
        long _vals[] = {__VA_ARGS__};                                   \
        size_t _n = sizeof(_vals) / sizeof(long);                       \
        fsti_config_add_longs(config, #elem, desc, _vals, _n);          \
    } while(0)


#define FSTI_FIRST_OF(N, ...) N

#define FSTI_STRS(...)                                                  \
    (const char *[]) {__VA_ARGS__},                                     \
        sizeof((const char *[]) {__VA_ARGS__})  / sizeof(const char *)

#define FSTI_LONGS(...)                                                 \
    (long[]) {__VA_ARGS__},                                             \
        sizeof((long[]) {__VA_ARGS__}) / sizeof(long)

#define FSTI_DBLS(...)                                                  \
    (double[]) {__VA_ARGS__},                                           \
        sizeof((double[]) {__VA_ARGS__}) /   sizeof(double)

#define FSTI_TYPE_ARR(v, ...)                                           \
    _Generic( (v),                                                      \
              int: (long []) {__VA_ARGS__},                             \
              long: (long []) {__VA_ARGS__},                            \
              double: (double[]) {__VA_ARGS__}                          \
        )

#define FSTI_SIZE_ARR(v, ...)                                           \
    _Generic( (v),                                                      \
              int: sizeof( (long[]) {__VA_ARGS__}) /                    \
              sizeof(long),                                             \
              long: sizeof( (long[]) {__VA_ARGS__}) /                   \
              sizeof(long),                                             \
              double:sizeof( (double[]) {__VA_ARGS__}) /                \
              sizeof(double)                                            \
        )

#define FSTI_CONFIG_ADD(config, key, description, ...)                  \
    _Generic((FSTI_FIRST_OF(__VA_ARGS__)),                              \
             int: fsti_config_add_longs,                                \
             long: fsti_config_add_longs,                               \
             double: fsti_config_add_doubles                            \
        ) (config, #key, description,                                   \
           FSTI_TYPE_ARR(FSTI_FIRST_OF(__VA_ARGS__), __VA_ARGS__),      \
           FSTI_SIZE_ARR(FSTI_FIRST_OF(__VA_ARGS__), __VA_ARGS__))

struct fsti_config_entry {
    char key[FSTI_KEY_LEN];
    char description[FSTI_DESC_LEN];
    struct fsti_variant *variants;
    size_t len;
    size_t order;
    bool initialize;
    size_t offset;
    size_t bytes;
    struct fsti_config_entry *next;
};

struct fsti_config {
    struct fsti_config_entry *entry[FSTI_HASHSIZE];
};

void fsti_config_init(struct fsti_config *config);
void fsti_config_print_entry(const struct fsti_config_entry *entry, char delim);
void fsti_config_print_all(struct fsti_config *config);
struct fsti_config_entry *fsti_config_find(const struct fsti_config *config, const char *key);
struct fsti_variant *fsti_config_at(const struct fsti_config *config, const char *key, size_t index);
struct fsti_variant *fsti_config_at0(const struct fsti_config *config, const char *key);
char *fsti_config_at_str(const struct fsti_config *config, const char *key, size_t index);
char *fsti_config_at0_str(const struct fsti_config *config, const char *key);
long fsti_config_at_long(const struct fsti_config *config, const char *key,
                         size_t index);
long fsti_config_at0_long(const struct fsti_config *config, const char *key);
double fsti_config_at_double(const struct fsti_config *config, const char *key,
                             size_t index);
double fsti_config_at0_double(const struct fsti_config *config, const char *key);

void fsti_config_add_strs(struct fsti_config *config,
                          const char *key,
                          const char *description,
                          const char *vals[],
                          size_t n);
void fsti_config_add_doubles(struct fsti_config *config,
                             const char *key,
                             const char *description,
                             double vals[],
                             size_t n);
void fsti_config_add_longs(struct fsti_config *config,
                           const char *key,
                           const char *description,
                           long vals[],
                           size_t n);
void fsti_config_add_arr(struct fsti_config *config, const char *key,
                         const char *description,
                         const struct fsti_variant *variants, size_t n);
void fsti_config_add(struct fsti_config *config, const char *key,
                     const char *description, const char *values);
struct fsti_config_entry *
fsti_config_replace_arr(struct fsti_config *config,
                        const char *key, const char *description,
                        const struct fsti_variant *variants, size_t n);
size_t fsti_config_count(struct fsti_config *config);
void fsti_config_free(struct fsti_config *config);
void fsti_config_replace_values(struct fsti_config *config, const char *key,
                                const char *values);
void fsti_config_process_key_values(struct fsti_config *config,
                                    char *key_value_str);
void fsti_config_process_strings(struct fsti_config *config,
                                 char **config_strings);
void fsti_config_copy(struct fsti_config *dest, const struct fsti_config *src);
struct fsti_variant_array fsti_variant_array_fill(const enum fsti_type types[],
                                                  const union fsti_value vals[],
                                                  size_t len);
void fsti_config_test(struct test_group *tg);

#endif
