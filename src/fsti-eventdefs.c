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

#include "utils/utils.h"
#include "fsti-eventdefs.h"

static struct fsti_event_register *event_register = NULL;

bool fsti_register_is_initialized()
{
    return event_register != NULL;
}

void fsti_register_add(const char *event_name, fsti_event event)
{
    struct fsti_event_register *entry;
    HASH_FIND_STR(event_register, event_name, entry);
    if (entry == NULL) {
	entry = malloc(sizeof(*entry));
	if (FSTI_ERROR(entry == NULL, FSTI_ERR_NOMEM, NULL))
	    return;
	strncpy(entry->event_name, event_name, FSTI_KEY_LEN);
	HASH_ADD_STR(event_register, event_name, entry);
    }
    entry->event = event;
}

fsti_event fsti_register_get(const char *event_name)
{
    struct fsti_event_register *entry;
    HASH_FIND_STR(event_register, event_name, entry);
    FSTI_ASSERT(entry != NULL && entry->event != NULL, FSTI_ERR_EVENT_NOT_FOUND,
        event_name);
    return entry->event;
}

void fsti_register_free()
{
    struct fsti_event_register *entry, *tmp;
    HASH_ITER(hh, event_register, entry, tmp) {
	HASH_DEL(event_register, entry);
	free(entry);
    }
    event_register = NULL;
}
