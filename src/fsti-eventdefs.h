#ifndef FSTI_EVENTDEFS_H
#define FSTI_EVENTDEFS_H

#include "uthash.h"
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
