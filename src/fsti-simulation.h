#ifndef FSTI_SIMULATION_H
#define FSTI_SIMULATION_H

#include <gsl/gsl_rng.h>

#include "fsti-agent.h"
#include "fsti-eventdefs.h"
#include "fsti-config.h"
#include "fsti-defaults.h"

struct fsti_simulation {
    struct fsti_config config;
    struct fsti_agent_arr agent_arr;
    size_t capacity;

    struct fsti_agent_csv_entry *csv_entries;
    size_t num_csv_entries;
    size_t agent_data_size;

    int sim_number;
    int config_sim_number;
    bool stop;
    char *name;
    fsti_event stop_event;
    struct fsti_event_array before_events;
    struct fsti_event_array during_events;
    struct fsti_event_array after_events;
    const gsl_rng_type *T;
    gsl_rng *rng;
    FILE *report;

    // Useful event variables available to all simulations
    double start_date;
    double end_date;
    double current_date;
    double time_step;
    unsigned stabilization_steps;
    unsigned num_iterations;
    unsigned iteration;
    unsigned report_frequency;
    void *data;
};

void fsti_simulation_init(struct fsti_simulation *simulation,
			  const struct fsti_config *config,
			  int sim_number, int config_sim_number);
size_t fsti_simulation_agent_size(struct fsti_simulation *simulation);
void fsti_event_hash_strings_to_vars(struct fsti_simulation *simulation);
struct fsti_agent *
fsti_simulation_new_agent(struct fsti_simulation *simulation);
void fsti_simulation_run(struct fsti_simulation *simulation);
fsti_event fsti_get_event(const char *event_name);
void fsti_simulation_set_csv(struct fsti_simulation *simulation,
                             struct fsti_agent_csv_entry entries[],
                             size_t num_entries,
                             size_t agent_size);
void fsti_deep_copy_agents(struct fsti_agent **dest,
                           struct fsti_agent **from,
                           size_t num_agents,
                           size_t agent_size);
void fsti_copy_agents(struct fsti_agent **dest, struct fsti_agent **from, size_t n);
void fsti_simulation_free(struct fsti_simulation *simulation);
#endif
