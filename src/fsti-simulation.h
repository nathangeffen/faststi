#ifndef FSTI_SIMULATION_H
#define FSTI_SIMULATION_H

#include <glib.h>
#include <gsl/gsl_rng.h>

#include "fsti-agent.h"
#include "fsti-eventdefs.h"
#include "fsti-config.h"
#include "fsti-defaults.h"

#define FSTI_FOR_LIVING(simulation, agent, code)       \
    FSTI_FOR((simulation).living, agent, code)


enum fsti_simulation_state {
    BEFORE,
    DURING,
    AFTER
};

struct fsti_simulation {
    struct fsti_config config;
    struct fsti_agent_arr agent_arr;
    struct fsti_agent_ind living;
    struct fsti_agent_ind dead;
    struct fsti_agent_ind mating_pool;

    struct fsti_agent csv_agent; // Used when reading in agent CSV file

    uint32_t sim_number;
    uint32_t config_sim_number;
    char *name;
    struct fsti_event_array before_events;
    struct fsti_event_array during_events;
    struct fsti_event_array after_events;
    enum fsti_simulation_state state;
    const gsl_rng_type *T;
    gsl_rng *rng;

    // Useful event variables available to all simulations
    GDateTime *start_date;
    GTimeZone *time_zone;
    int32_t time_step;
    int32_t age_input_time_step;
    double initial_mating_pool_prob;
    double mating_pool_prob;
    uint32_t stabilization_steps;
    uint32_t num_iterations;
    uint32_t iteration;
    uint32_t report_frequency;
    uint32_t match_k;
    bool agent_csv_header;
    char csv_delimiter;
    FILE *results_file;
    FILE *agents_output_file;
    struct fsti_dataset_hash *dataset_hash;
    struct fsti_dataset *dataset_mortality;
    struct fsti_dataset *dataset_mating_pool;
    FSTI_SIMULATION_FIELDS
};

void fsti_simulation_init(struct fsti_simulation *simulation,
			  const struct fsti_config *config,
			  int sim_number, int config_sim_number);
size_t fsti_simulation_agent_size(struct fsti_simulation *simulation);
struct fsti_dataset *
fsti_simulation_get_dataset(struct fsti_simulation *simulation, char *key);
void fsti_simulation_config_to_vars(struct fsti_simulation *simulation);
struct fsti_agent *
fsti_simulation_new_agent(struct fsti_simulation *simulation);
void fsti_simulation_run(struct fsti_simulation *simulation);
fsti_event fsti_get_event(const char *event_name);
void fsti_simulation_kill_agent(struct fsti_simulation *simulation, size_t *it);
void fsti_simulation_free(struct fsti_simulation *simulation);
void fsti_simulation_test(struct test_group *tg);

#endif
