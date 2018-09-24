#ifndef FSTI_PROCINI_H
#define FSTI_PROCINI_H

#include <glib.h>
#include "fsti-config.h"

void fsti_read_ini(const char *filename, GKeyFile * key_file);

#endif
