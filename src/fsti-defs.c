#include <stdio.h>
#include <glib.h>

#include "utils/utils.h"
#include "fsti-error.h"
#include "fsti-defs.h"


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
