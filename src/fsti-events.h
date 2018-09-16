#ifndef FSTI_EVENTS_H
#define FSTI_EVENTS_H

#include "fsti-simulation.h"
#include "fsti-defs.h"
#include "fsti-userdefs.h"
#include "fsti-agent.h"

void fsti_to_float(void *to, const struct fsti_variant *from,
                   struct fsti_simulation *simulation);
void fsti_to_double(void *to, const struct fsti_variant *from,
                   struct fsti_simulation *simulation);
void fsti_to_int(void *to, const struct fsti_variant *from,
                   struct fsti_simulation *simulation);
void fsti_to_unsigned(void *to, const struct fsti_variant *from,
                   struct fsti_simulation *simulation);
void fsti_to_uchar(void *to, const struct fsti_variant *from,
                   struct fsti_simulation *simulation);
void fsti_to_partner(void *to, const struct fsti_variant *from,
                     struct fsti_simulation *simulation);

void fsti_event_age(struct fsti_simulation *simulation);
void fsti_event_report(struct fsti_simulation *simulation);
void fsti_event_write_agents_csv(struct fsti_simulation *simulation);
void fsti_event_write_agents_pretty(struct fsti_simulation *simulation);
void fsti_event_register_standard_events();

static struct fsti_agent fsti_global_agent;
static const struct fsti_csv_entry fsti_csv_entries[] = {
    FSTI_CSV_ENTRIES
};

static const struct fsti_csv_agent fsti_global_csv =
{
    .agent = &fsti_global_agent,
    .num_entries = sizeof(fsti_csv_entries) / sizeof(struct fsti_csv_entry),
    .entries = fsti_csv_entries
};


#endif
