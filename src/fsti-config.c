#include <stdio.h>
#include <glib.h>

#include "fsti-error.h"
#include "fsti-config.h"
#include "fsti-dataset.h"

static void free_vals(struct fsti_config_entry *entry)
{
    for (size_t i = 0; i < entry->len; ++i) {
        if (entry->variants[i].type == STR)
            free(entry->variants[i].value.str);
    }
    free(entry->variants);
    entry->len = 0;
    entry->variants = NULL;
}

static void free_entry(struct fsti_config_entry *entry)
{
    free_vals(entry);
    free(entry);
}

void fsti_config_init(struct fsti_config *config)
{
    memset(config->entry, 0, sizeof(struct fsti_config_entry *) * FSTI_HASHSIZE);
}

void fsti_config_print_entry(const struct fsti_config_entry *entry)
{
    printf("%s,%s,", entry->key, entry->description);
    for (size_t i = 0; i < entry->len; ++i) {
	if (i > 0)
	    putchar(',');
	switch (entry->variants[i].type) {
	case LONG:
	    printf("%ld", entry->variants[i].value.longint);
	    break;
	case DBL:
	    printf("%f", entry->variants[i].value.dbl);
	    break;
	case STR:
	    printf("%s", entry->variants[i].value.str);
	    break;
	default:
	    puts("This shouldn't happen");
	    assert(0);
	}
    }
    putchar('\n');
}

void fsti_config_print_all(struct fsti_config *config)
{
    for (size_t i = 0; i < FSTI_HASHSIZE; ++i) {
        struct fsti_config_entry *entry = config->entry[i];
        while(entry) {
            fsti_config_print_entry(entry);
            entry = entry->next;
        }
    }
}

struct fsti_config_entry *fsti_config_find(const struct fsti_config *config,
                                          const char *key)
{
    struct fsti_config_entry *np;

    for (np = config->entry[fsti_hash(key)]; np != NULL; np = np->next)
        if (strcmp(key, np->key) == 0)
            return np;
    return NULL;
}

struct fsti_variant *fsti_config_at(const struct fsti_config *config,
				    const char *key, size_t index)
{
    struct fsti_config_entry *entry;

    entry = fsti_config_find(config, key);
    FSTI_ASSERT(entry, FSTI_ERR_KEY_NOT_FOUND, key);
    FSTI_ASSERT(entry, FSTI_ERR_OUT_OF_BOUNDS, NULL);

    return &entry->variants[index];
}

struct fsti_variant *fsti_config_at0(const struct fsti_config
				     *config, const char *key)
{
    return fsti_config_at(config, key, 0);
}

char *fsti_config_at_str(const struct fsti_config *config,
			 const char *key, size_t index)
{
    struct fsti_variant *variant;

    variant = fsti_config_at(config, key, index);
    FSTI_ASSERT(variant && variant->type == STR, FSTI_ERR_STR_EXPECTED, key);
    return variant->value.str;
}

char *fsti_config_at0_str(const struct fsti_config *config,
			  const char *key)
{
    return fsti_config_at_str(config, key, 0);
}

long
fsti_config_at_long(const struct fsti_config *config,
		    const char *key, size_t index)
{
    struct fsti_variant *variant;

    variant = fsti_config_at(config, key, index);

    FSTI_ASSERT(variant && variant->type == LONG, FSTI_ERR_LONG_EXPECTED, key);
    return variant->value.longint;
}


long
fsti_config_at0_long(const struct fsti_config *config, const char *key)
{
    return fsti_config_at_long(config, key, 0);
}

double
fsti_config_at_double(const struct fsti_config *config,
		      const char *key, size_t index)
{
    struct fsti_variant *variant;

    variant = fsti_config_at(config, key, index);
    if (variant->type == DBL)
        return variant->value.dbl;
    else if (variant->type == LONG)
        return  variant->value.longint;
    FSTI_ASSERT(false, FSTI_ERR_DBL_EXPECTED, key);
}


double
fsti_config_at0_double(const struct fsti_config *config, const char *key)
{
    return fsti_config_at_double(config, key, 0);
}


static struct fsti_config_entry
*fsti_config_replace(struct fsti_config *config,
                     const char *key,
                     const char *description)
{
    struct fsti_config_entry *entry = fsti_config_find(config, key);

    if (entry) {
	if (description) {
	    strncpy(entry->description, description, FSTI_DESC_LEN);
            entry->description[FSTI_DESC_LEN - 1] = '\0';
        }
    }

    return entry;
}

static struct fsti_config_entry *config_add(struct fsti_config *config,
                                            const char *key,
                                            const char *description)
{
    struct fsti_config_entry *entry;
    size_t hashval;

    entry = fsti_config_find(config, key);
    if (entry) {
        free_vals(entry);
    } else {
        entry = malloc(sizeof(*entry));
        FSTI_ASSERT(entry, FSTI_ERR_NOMEM, NULL);
        strncpy(entry->key, key, FSTI_KEY_LEN);
        entry->key[FSTI_KEY_LEN - 1] = '\0';
        hashval = fsti_hash(key);
        entry->next = config->entry[hashval];
        config->entry[hashval] = entry;
    }
    strncpy(entry->description, description, FSTI_DESC_LEN);
    entry->description[FSTI_DESC_LEN - 1] = '\0';
    entry->variants = NULL;
    entry->len = 0;
    return entry;
}

static void set_types(struct fsti_config_entry *entry,
		      const enum fsti_type *types, size_t n)
{
    if (entry->variants)
	free_vals(entry);

    entry->variants = calloc(n, sizeof(*entry->variants));
    FSTI_ASSERT(entry->variants, FSTI_ERR_NOMEM, NULL);

    for (size_t i = 0; i < n; ++i)
	entry->variants[i].type = types[i];

    entry->len = n;
}

static void set_variants(struct fsti_config_entry *entry,
			 const struct fsti_variant *variants, size_t n)
{
    if (entry->variants)
	free_vals(entry);

    entry->variants = calloc(n, sizeof(*entry->variants));
    FSTI_ASSERT(entry->variants, FSTI_ERR_NOMEM, NULL);

    for (size_t i = 0; i < n; ++i) {
	entry->variants[i].type = variants[i].type;
	if (entry->variants[i].type == STR) {
	    entry->variants[i].value.str = strdup(variants[i].value.str);
            FSTI_ASSERT(entry->variants[i].value.str, FSTI_ERR_NOMEM, NULL);
	} else {
	    entry->variants[i].value = variants[i].value;
	}
    }
    entry->len = n;
}


void fsti_config_add_str(struct fsti_config *config,
			 const char *key,
			 const char *description, const char *val)
{
    const enum fsti_type types = { STR };
    struct fsti_config_entry *entry = config_add(config, key, description);

    set_types(entry, &types, 1);
    entry->variants[0].value.str = strdup(val);
    FSTI_ASSERT(entry->variants[0].value.str, FSTI_ERR_NOMEM, NULL);
}


void fsti_config_add_double(struct fsti_config *config,
			    const char *key,
			    const char *description, double val)
{
    const enum fsti_type types = { DBL };
    struct fsti_config_entry *entry = config_add(config, key, description);

    set_types(entry, &types, 1);
    entry->variants[0].value.dbl = val;
}

void fsti_config_add_long(struct fsti_config *config,
			  const char *key,
			  const char *description, long val)
{
    const enum fsti_type types = { LONG };
    struct fsti_config_entry *entry = config_add(config, key, description);

    set_types(entry, &types, 1);
    entry->variants[0].value.longint = val;
}


void fsti_config_add_arr(struct fsti_config *config,
			 const char *key,
			 const char *description,
			 const struct fsti_variant *variants, size_t n)
{
    struct fsti_config_entry *entry = config_add(config, key, description);
    set_variants(entry, variants, n);
}

void fsti_config_add(struct fsti_config *config, const char *key,
                     const char *description, const char *values)
{
    config_add(config, key, description);
    fsti_config_replace_values(config, key, values);
}

struct fsti_config_entry *fsti_config_replace_arr(struct fsti_config *config,
                                                  const char *key,
                                                  const char *description,
                                                  const struct fsti_variant
                                                  *variants, size_t n)
{
    struct fsti_config_entry *entry =
	fsti_config_replace(config, key, description);
    FSTI_ASSERT(entry, FSTI_ERR_KEY_NOT_FOUND, key);
    free_vals(entry);
    set_variants(entry, variants, n);
    return entry;
}

size_t fsti_config_count(struct fsti_config *config)
{
    struct fsti_config_entry *entry;
    size_t count = 0;
    for (size_t i = 0; i < FSTI_HASHSIZE; ++i) {
        for(entry = config->entry[i]; entry; entry = entry->next)
            ++count;
    }
    return count;
}

void fsti_config_free(struct fsti_config *config)
{
    struct fsti_config_entry *entry, *prev;
    for (size_t i = 0; i < FSTI_HASHSIZE; ++i) {
        entry = config->entry[i];
        while (entry) {
            prev = entry;
            entry = entry->next;
            free_entry(prev);
        }
    }
}

static void parse_values(struct fsti_variant_array *variant_arr,
			 const char *values)
{
    assert(values);
    struct fsti_variant variant;
    char **split_values = g_strsplit_set(values, ";", 0);
    char **value = split_values;

    FSTI_ASSERT(split_values && split_values[0], FSTI_ERR_KEY_NOT_FOUND, NULL);

    while (*value) {
	g_strstrip(*value);
        FSTI_ASSERT(strcmp(*value, ""), FSTI_ERR_NO_VALUE_FOR_KEY, NULL);
	variant = fsti_identify_token(*value);
	ARRAY_PUSH(*variant_arr, variants, variant);
	++value;
    }
    g_strfreev(split_values);
}


void fsti_config_replace_values(struct fsti_config *config,
                                const char *key, const char *values)
{
    struct fsti_variant_array variant_arr;

    ARRAY_NEW(variant_arr, variants);
    parse_values(&variant_arr, values);
    fsti_config_replace_arr(config, key, NULL,
                           variant_arr.variants, variant_arr.len);

    for (size_t i = 0; i < variant_arr.len; ++i)
        if (variant_arr.variants[i].type == STR)
            free(variant_arr.variants[i].value.str);
    ARRAY_FREE(variant_arr, variants);
}


void fsti_config_process_key_values(struct fsti_config *config,
				    char *key_value_str)
{
    char **key_values = g_strsplit_set(key_value_str, "=", 0);

    FSTI_ASSERT(key_values, FSTI_ERR_NOMEM, NULL);
    FSTI_ASSERT(key_values[0], FSTI_ERR_KEY_NOT_FOUND, key_value_str);

    FSTI_ASSERT(strcmp(g_strstrip(key_values[0]), ""), FSTI_ERR_KEY_NOT_FOUND,
                key_value_str);
    FSTI_ASSERT(key_values[1], FSTI_ERR_NO_VALUE_FOR_KEY, key_value_str);
    FSTI_ASSERT(strcmp(g_strstrip(key_values[1]), ""), FSTI_ERR_NO_VALUE_FOR_KEY,
                key_value_str);
    fsti_config_replace_values(config, key_values[0], key_values[1]);
    g_strfreev(key_values);
}

void fsti_config_process_strings(struct fsti_config *config,
				 char **config_strings)
{
    assert(config_strings);

    for (char **s = config_strings; s && *s; ++s)
	fsti_config_process_key_values(config, *s);
}

void fsti_config_copy(struct fsti_config *dest, const struct fsti_config *src)
{
    struct fsti_config_entry *entry;

    fsti_config_init(dest);
    for (size_t i = 0; i < FSTI_HASHSIZE; ++i)
        for (entry = src->entry[i]; entry; entry = entry->next)
            fsti_config_add_arr(dest, entry->key, entry->description,
                                entry->variants, entry->len);
}

struct fsti_variant_array
fsti_variant_array_fill(const enum fsti_type types[],
			const union fsti_value vals[], size_t len)
{
    struct fsti_variant_array variant_arr;
    ARRAY_NEW(variant_arr, variants);

    for (size_t i = 0; i < len; i++) {
	struct fsti_variant v;
	v.type = types[i];
	v.value = vals[i];
	ARRAY_PUSH(variant_arr, variants, v);
    }

    return variant_arr;
}

void fsti_config_test(struct test_group *tg)
{
    struct fsti_config config;

    fsti_config_init(&config);
    fsti_config_add_str(&config, "KEY1", "Description", "VALUE1");
    struct fsti_config_entry *entry = fsti_config_find(&config, "KEY1");
    TESTEQ(strcmp(entry->key, "KEY1"), 0, *tg);
    TESTEQ(strcmp(entry->description, "Description"), 0, *tg);
    TESTEQ(entry->variants[0].type, STR, *tg);
    TESTEQ(strcmp(entry->variants[0].value.str, "VALUE1"), 0, *tg);
    TESTEQ(entry->len, 1, *tg);
    fsti_config_add_str(&config, "KEY2", "Description", "VALUE2");
    entry = fsti_config_find(&config, "KEY2");
    TESTEQ(strcmp(entry->key, "KEY2"), 0, *tg);
    TESTEQ(entry->variants[0].type, STR, *tg);
    TESTEQ(strcmp(entry->variants[0].value.str, "VALUE2"), 0, *tg);
    fsti_config_add_double(&config, "KEY3", "Description", 23.1);
    entry = fsti_config_find(&config, "KEY3");
    TESTEQ(strcmp(entry->key, "KEY3"), 0, *tg);
    TESTEQ(entry->variants[0].type, DBL, *tg);
    TESTEQ(entry->variants[0].value.dbl, 23.1, *tg);
    fsti_config_add_long(&config, "KEY4", "Description", 9);
    entry = fsti_config_find(&config, "KEY4");
    TESTEQ(strcmp(entry->key, "KEY4"), 0, *tg);
    TESTEQ(entry->variants[0].type, LONG, *tg);
    TESTEQ(entry->variants[0].value.longint, 9, *tg);
    fsti_config_add_str(&config, "KEY1", "Description", "VALUE3");
    entry = fsti_config_find(&config, "KEY1");
    TESTEQ(strcmp(entry->key, "KEY1"), 0, *tg);
    TESTEQ(entry->variants[0].type, STR, *tg);
    TESTEQ(strcmp(entry->variants[0].value.str, "VALUE3"), 0, *tg);
    fsti_config_add_double(&config, "KEY2", "Description", 13);
    entry = fsti_config_find(&config, "KEY2");
    TESTEQ(strcmp(entry->key, "KEY2"), 0, *tg);
    TESTEQ(entry->variants[0].type, DBL, *tg);
    TESTEQ(entry->variants[0].value.dbl, 13, *tg);
    FSTI_CONFIG_ADD(&config, "KEY4", "Description", "VALUE4");
    entry = fsti_config_find(&config, "KEY4");
    TESTEQ(strcmp(entry->key, "KEY4"), 0, *tg);
    TESTEQ(entry->variants[0].type, STR, *tg);
    TESTEQ(strcmp(entry->variants[0].value.str, "VALUE4"), 0, *tg);
    FSTI_CONFIG_ADD(&config, "KEY5", "Description", 18.6);
    entry = fsti_config_find(&config, "KEY5");
    TESTEQ(strcmp(entry->key, "KEY5"), 0, *tg);
    TESTEQ(entry->variants[0].type, DBL, *tg);
    TESTEQ(entry->variants[0].value.dbl, 18.6, *tg);
    FSTI_CONFIG_ADD(&config, "KEY6", "Description", 18);
    entry = fsti_config_find(&config, "KEY6");
    TESTEQ(strcmp(entry->key, "KEY6"), 0, *tg);
    TESTEQ(entry->variants[0].type, LONG, *tg);
    TESTEQ(entry->variants[0].value.longint, 18, *tg);

    struct fsti_variant_array variant_arr;
    const enum fsti_type types[] = { STR, DBL, LONG, STR };
    const union fsti_value vals[] = {
        {.str = "String1"},
        {.dbl = 32.5},
        {.longint = 42},
        {.str = "String2"}
    };
    variant_arr = fsti_variant_array_fill(types, vals, 4);
    fsti_config_add_arr(&config, "KEY7", "Description",
        		variant_arr.variants, variant_arr.len);
    entry = fsti_config_find(&config, "KEY7");
    TESTEQ(strcmp(entry->key, "KEY7"), 0, *tg);
    TESTEQ(entry->len, 4, *tg);
    TESTEQ(strcmp(entry->variants[0].value.str, "String1"), 0, *tg);
    TESTEQ(entry->variants[0].type, STR, *tg);
    TESTEQ(entry->variants[1].value.dbl, 32.5, *tg);
    TESTEQ(entry->variants[1].type, DBL, *tg);
    TESTEQ(entry->variants[2].value.longint, 42, *tg);
    TESTEQ(entry->variants[2].type, LONG, *tg);
    TESTEQ(strcmp(entry->variants[3].value.str, "String2"), 0, *tg);
    TESTEQ(entry->variants[3].type, STR, *tg);

    TESTEQ(7, fsti_config_count(&config), *tg);

    char *strings[] = {
        "KEY1 = 27.3; \"Hello\tworld\" ; 18",
        "KEY2=Hi;!VARY;1;20;1",
        NULL
    };
    fsti_config_process_strings(&config, strings);
    entry = fsti_config_find(&config, "KEY1");
    TESTEQ(entry->len, 3, *tg);
    TESTEQ(entry->variants[0].type, DBL, *tg);
    TESTEQ(entry->variants[0].value.dbl, 27.3, *tg);
    TESTEQ(entry->variants[1].type, STR, *tg);

    const char *s = "\"Hello\tworld\"";
    TESTEQ(strcmp(entry->variants[1].value.str, s), 0, *tg);
    TESTEQ(entry->variants[2].type, LONG, *tg);
    TESTEQ(entry->variants[2].value.longint, 18, *tg);
    entry = fsti_config_find(&config, "KEY2");
    TESTEQ(entry->len, 5, *tg);

    // Test copying
    struct fsti_config copy;
    fsti_config_copy(&copy, &config);
    TESTEQ(7, fsti_config_count(&copy), *tg);
    entry = fsti_config_find(&copy, "KEY1");
    TESTEQ(entry->len, 3, *tg);
    TESTEQ(entry->variants[0].type, DBL, *tg);
    TESTEQ(entry->variants[0].value.dbl, 27.3, *tg);
    TESTEQ(entry->variants[1].type, STR, *tg);
    TESTEQ(strcmp(entry->variants[1].value.str, s), 0, *tg);
    TESTEQ(entry->variants[2].type, LONG, *tg);
    TESTEQ(entry->variants[2].value.longint, 18, *tg);
    entry = fsti_config_find(&copy, "KEY2");
    TESTEQ(entry->len, 5, *tg);
    fsti_config_free(&copy);
    // Check that the free didn't screw up the original
    TESTEQ(7, fsti_config_count(&config), *tg);

    ARRAY_FREE(variant_arr, variants);
    fsti_config_free(&config);

    // Test that most flexible add method works
    fsti_config_init(&config);
    fsti_config_add(&config, "KEY1", "Description", "CONST;5.0;10;12;ABC");
    entry = fsti_config_find(&config, "KEY1");
    TESTEQ(entry->len, 5, *tg);
    TESTEQ(entry->variants[0].type, STR, *tg);
    TESTEQ(strcmp(entry->variants[0].value.str, "CONST"), 0, *tg);
    TESTEQ(entry->variants[1].type, DBL, *tg);
    TESTEQ(entry->variants[1].value.dbl, 5.0, *tg);
    TESTEQ(entry->variants[2].type, LONG, *tg);
    TESTEQ(entry->variants[2].value.longint, 10, *tg);
    TESTEQ(entry->variants[3].type, LONG, *tg);
    TESTEQ(entry->variants[3].value.longint, 12, *tg);
    TESTEQ(entry->variants[4].type, STR, *tg);
    TESTEQ(strcmp(entry->variants[4].value.str, "ABC"), 0, *tg);

    fsti_config_free(&config);
}
