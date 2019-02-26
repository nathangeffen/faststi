#ifndef FSTI_SIMULATION_H
#define FSTI_SIMULATION_H

#include <time.h>

#include <glib.h>
#include <gsl/gsl_rng.h>

#include "fsti-agent.h"
#include "fsti-dataset.h"
#include "fsti-eventdefs.h"
#include "fsti-config.h"
#include "fsti-defaults.h"

#define FSTI_FOR_LIVING(simulation, agent, code)       \
    FSTI_FOR((simulation).living, agent, code)

enum fsti_simulation_state {
    BEFORE,
    STABILIZATION,
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
    struct fsti_event_array stabilization_events;
    struct fsti_event_array during_events;
    struct fsti_event_array after_events;
    bool record_matches;
    bool record_breakups;
    bool record_infections;
    uint32_t initial_matches;
    uint32_t matches;
    uint32_t breakups;
    uint32_t initial_infections;
    uint32_t infections;
    enum fsti_simulation_state state;
    const gsl_rng_type *T;
    gsl_rng *rng;

    // Useful event variables available to all simulations
    GDateTime *start_date;
    GTimeZone *time_zone;
    int32_t time_step;
    int32_t age_input_time_step;
    time_t time_rec;

    uint8_t initial_infect_stage;

    uint32_t stabilization_steps;
    uint32_t num_time_steps;
    uint32_t iteration;
    uint32_t report_frequency;
    uint32_t match_k;
    uint8_t max_stage;
    bool agent_csv_header;
    char csv_delimiter;
    FILE *results_file;
    FILE *agents_output_file;
    FILE *partnerships_file;

    uint32_t age_min;
    uint32_t age_max;

    // Generate agent event vars
    double age_alpha;
    double age_beta;
    double prob_gen_male;
    double prob_gen_msw;
    double prob_gen_wsm;

    // Birth event vars
    uint32_t birth_event_every_n;

    double birth_rate;
    double prob_birth_male;
    double prob_birth_msw;
    double prob_birth_wsm;
    double prob_birth_infected_msm;
    double prob_birth_infected_msw;
    double prob_birth_infected_wsm;
    double prob_birth_infected_wsw;

    struct fsti_dataset_hash dataset_hash;
    struct fsti_dataset *dataset_mortality;
    struct fsti_dataset *dataset_single;
    struct fsti_dataset *dataset_rel;
    struct fsti_dataset *dataset_infect_stage;
    struct fsti_dataset *dataset_infect;
    struct fsti_dataset *dataset_coinfect;
    struct fsti_dataset *dataset_gen_sex;
    struct fsti_dataset *dataset_gen_sex_preferred;
    struct fsti_dataset *dataset_gen_infect;
    struct fsti_dataset *dataset_gen_treated;
    struct fsti_dataset *dataset_gen_resistant;
    struct fsti_dataset *dataset_gen_mating;

    struct fsti_dataset *dataset_birth_infect;
    struct fsti_dataset *dataset_birth_treated;
    struct fsti_dataset *dataset_birth_resistant;

    FSTI_SIMULATION_FIELDS
};

void fsti_simulation_init(struct fsti_simulation *simulation,
			  const struct fsti_config *config,
			  int sim_number, int config_sim_number);
size_t fsti_simulation_agent_size(struct fsti_simulation *simulation);
struct fsti_dataset *
fsti_simulation_get_dataset(struct fsti_simulation *simulation, char *key);
void fsti_simulation_load_datasets(struct fsti_config *config,
                                   struct fsti_dataset_hash *dataset_hash);
void fsti_simulation_config_to_vars(struct fsti_simulation *simulation);
struct fsti_agent *
fsti_simulation_new_agent(struct fsti_simulation *simulation);
void fsti_simulation_run(struct fsti_simulation *simulation);
fsti_event fsti_get_event(const char *event_name);
void fsti_simulation_kill_agent(struct fsti_simulation *simulation, size_t *it);
void fsti_simulation_free(struct fsti_simulation *simulation);
void fsti_simulation_test(struct test_group *tg);

#endif
