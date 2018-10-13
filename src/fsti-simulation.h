#ifndef FSTI_SIMULATION_H
#define FSTI_SIMULATION_H

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

    const struct fsti_csv_agent *csv;

    int sim_number;
    int config_sim_number;
    bool stop;
    char *name;
    fsti_event stop_event;
    struct fsti_event_array before_events;
    struct fsti_event_array during_events;
    struct fsti_event_array after_events;
    enum fsti_simulation_state state;
    const gsl_rng_type *T;
    gsl_rng *rng;

    // Useful event variables available to all simulations
    float start_date;
    float end_date;
    float current_date;
    float time_step;
    float mating_pool_prob;
    unsigned stabilization_steps;
    unsigned num_iterations;
    unsigned iteration;
    unsigned report_frequency;
    unsigned match_k;
    FILE *results_file;
    FILE *agents_output_file;
    FSTI_SIMULATION_FIELDS
};

void fsti_simulation_init(struct fsti_simulation *simulation,
			  const struct fsti_config *config,
			  int sim_number, int config_sim_number);
size_t fsti_simulation_agent_size(struct fsti_simulation *simulation);
void fsti_simulation_config_to_vars(struct fsti_simulation *simulation);
struct fsti_agent *
fsti_simulation_new_agent(struct fsti_simulation *simulation);
void fsti_simulation_write_agents_ind_csv(struct fsti_simulation *simulation,
                                          struct fsti_agent_ind *agent_ind);
void fsti_simulation_write_agents_arr_csv(struct fsti_simulation *simulation);
void fsti_simulation_run(struct fsti_simulation *simulation);
fsti_event fsti_get_event(const char *event_name);
void fsti_simulation_set_csv(struct fsti_simulation *simulation,
                             const struct fsti_csv_agent *csv);
void fsti_simulation_kill_agent(struct fsti_simulation *simulation, size_t *it);
void fsti_simulation_free(struct fsti_simulation *simulation);
void fsti_simulation_test(struct test_group *tg);

#endif
