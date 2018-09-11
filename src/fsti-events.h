#ifndef FSTI_EVENTS_H
#define FSTI_EVENTS_H

#include "fsti-simulation.h"
#include "fsti-defs.h"

void fsti_event_age(struct fsti_simulation *simulation);
void fsti_event_report(struct fsti_simulation *simulation);
void fsti_event_register_standard_events();

#endif
