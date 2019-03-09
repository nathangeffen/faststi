/*
  FastSTIi: large simulations of sexually transmitted infection epidemics.

  Copyright (C) 2019 Nathan Geffen

  FastSTI is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FastSTI is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
