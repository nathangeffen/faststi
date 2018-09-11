#ifndef FSTI_CONFIG_H
#define FSTI_CONFIG_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <useful.h>

#include "uthash.h"
#include "fsti-defs.h"

#define FSTI_HASHSIZE 101

#define FSTI_CONFIG_ADD(config, key, description, value)        \
    do {                                                        \
        _Generic((value),					\
                 char *: fsti_config_add_str,			\
                 int: fsti_config_add_long,			\
                 long: fsti_config_add_long,			\
                 double: fsti_config_add_double			\
            ) (config, key, description, value);		\
    } while(0)


struct fsti_config_entry {
    char key[FSTI_KEY_LEN];
    char description[FSTI_DESC_LEN];
    struct fsti_variant *variants;
    size_t len;
    struct fsti_config_entry *next;
};

struct fsti_config {
    struct fsti_config_entry *entry[FSTI_HASHSIZE];
};

void fsti_config_init(struct fsti_config *config);
void fsti_config_print_entry(const struct fsti_config_entry *entry);
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
void fsti_config_add_str(struct fsti_config *config, const char *key,
                         const char *description, const char *val);
void fsti_config_add_double(struct fsti_config *config, const char *key,
                            const char *description, double val);
void fsti_config_add_long(struct fsti_config *config, const char *key,
                          const char *description, long val);
void fsti_config_add_arr(struct fsti_config *config, const char *key,
                         const char *description,
                         const struct fsti_variant *variants, size_t n);
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
