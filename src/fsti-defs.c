#include <stdio.h>
#include <glib.h>

#include "utils/utils.h"
#include "fsti-error.h"
#include "fsti-defs.h"


size_t fsti_hash(const char *str)
{
    unsigned long hash = 5381;
    int c;
    size_t result;

    while ( (c = *str++) )
        hash = ((hash << 5) + hash) + c;

    result = hash % FSTI_HASHSIZE;
    return result;
}

struct fsti_variant fsti_identify_token(char *token)
{
    double d;
    long i;
    struct fsti_variant result;
    char *err = NULL;

    errno = 0;
    g_strstrip(token);

    i = strtol(token, &err, 10);
    if (errno || *err != 0) {
	errno = 0;
	err = NULL;
	d = strtod(token, &err);
	if (errno || *err != 0) {
	    result.value.str = strdup(token);
            FSTI_ASSERT(result.value.str, FSTI_ERR_NOMEM, NULL);
	    result.type = STR;
	} else {
	    result.value.dbl = d;
	    result.type = DBL;
	}
    } else {
	result.value.longint = i;
	result.type = LONG;
    }

    return result;
}


struct fsti_variant fsti_identify_token_const(const char *token)
{
    char s[FSTI_TOKEN_LEN + 1];
    strncpy(s, token, FSTI_TOKEN_LEN);
    s[FSTI_TOKEN_LEN] = 0;
    FSTI_ASSERT(s, FSTI_ERR_NOMEM, NULL);

    return fsti_identify_token(s);
}

static _Thread_local char full_filename[FILENAME_MAX];

char *fsti_make_full_data_filename(const char *filename)
{
    const char *path;

    path = g_environ_getenv (g_get_environ(), "FSTI_DATA");

    if (path)
        snprintf(full_filename, FILENAME_MAX, "%s%s%s", path, G_DIR_SEPARATOR_S,
                 filename);
    else
        strncpy(full_filename, filename, FILENAME_MAX);

    return full_filename;
}

FILE *fsti_open_data_file(const char *filename, const char *mode)
{
    return fopen(fsti_make_full_data_filename(filename), mode);
}

void fsti_remove_data_file(const char *filename)
{
    remove(fsti_make_full_data_filename(filename));
}
