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

#ifndef FSTI_EVENTDEFS_H
#define FSTI_EVENTDEFS_H

#include "utils/uthash.h"
#include "fsti-defs.h"
#include "fsti-error.h"

struct fsti_simulation;

typedef void (*fsti_event) (struct fsti_simulation *);

struct fsti_event_array {
    size_t len;
    size_t capacity;
    fsti_event *events;
};

struct fsti_event_register {
    char event_name[FSTI_KEY_LEN];
    fsti_event event;
    UT_hash_handle hh;
};

void fsti_register_add(const char *event_name, fsti_event event);
fsti_event fsti_register_get(const char *event_name);
void fsti_register_free();

#endif
