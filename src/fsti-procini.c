#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

#include "fsti-procini.h"
#include "fsti-error.h"

void fsti_read_ini(const char *filename, GKeyFile * key_file)
{
    GError *error = NULL;
    g_key_file_load_from_file(key_file, filename, G_KEY_FILE_NONE, &error);
    FSTI_ASSERT(error == NULL, FSTI_ERR_KEY_FILE_LOAD, "");
    /* if (error) { */
    /*     g_error_free(error); */
    /*     g_key_file_free(key_file); */
    /*     return FSTI_ERROR_WITH_MSG(FSTI_ERR_KEY_FILE_LOAD, error->message); */
    /* } */

    /* char ** groups  = g_key_file_get_groups(key_file, NULL); */

    /* for (char ** group = groups; *group != NULL; ++group) { */
    /*      char ** keys = g_key_file_get_keys(key_file, *group, */
    /*                                      NULL, NULL); */
    /*      for (char ** key = keys; *key != NULL; ++key) { */
    /*              printf("Key: %s\t", *key); */
    /*              char * value = g_key_file_get_value (key_file, *group, */
    /*                                              *key, NULL); */
    /*              printf("Value: %s\n", value); */
    /*              free(value); */
    /*      } */
    /*      g_strfreev(keys); */
    /* } */

    /* g_strfreev(groups); */
    /* g_key_file_free(key_file); */
}
