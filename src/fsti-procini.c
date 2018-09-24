#include <stdlib.h>
#include <stdio.h>

#include "fsti-procini.h"
#include "fsti-error.h"

void fsti_read_ini(const char *filename, GKeyFile * key_file)
{
    GError *error = NULL;
    g_key_file_load_from_file(key_file, filename, G_KEY_FILE_NONE, &error);
    FSTI_ASSERT(error == NULL, FSTI_ERR_KEY_FILE_LOAD, "");
}
