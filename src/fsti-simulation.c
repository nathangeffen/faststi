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
#include <stdbool.h>
#include <unistd.h>

#include "fsti-error.h"
#include "fsti-simulation.h"
#include "fsti-dataset.h"

void fsti_event_register_events();

static void set_events(struct fsti_event_array *event_arr,
                       struct fsti_config_entry *entry)
{
    size_t i;
    fsti_event e;

    for (i = 0; i < entry->len; i++) {
        FSTI_ASSERT(entry->variants[i].type == STR, FSTI_ERR_STR_EXPECTED, NULL);
        e = fsti_register_get(entry->variants[i].value.str);
        ARRAY_PUSH(*event_arr, events, e);
    }
}

static void exec_events(struct fsti_simulation *simulation,
            const struct fsti_event_array *event_array)
{
    for (size_t i = 0; i < event_array->len; i++) {
    event_array->events[i] (simulation);
    if (fsti_error)
        return;
    }
}

static void set_all_events(struct fsti_simulation *simulation)
{
    struct fsti_config_entry *entry;
    entry = fsti_config_find(&simulation->config, "before_events");
    if (entry)
        set_events(&simulation->before_events, entry);
    entry = fsti_config_find(&simulation->config, "stabilization_events");
    if (entry)
        set_events(&simulation->stabilization_events, entry);
    entry = fsti_config_find(&simulation->config, "during_events");
    if (entry)
        set_events(&simulation->during_events, entry);
    entry = fsti_config_find(&simulation->config, "after_events");
    if (entry)
        set_events(&simulation->after_events, entry);
}

void fsti_simulation_init(struct fsti_simulation *simulation,
			  const struct fsti_config *config,
			  int sim_number, int config_sim_number)
{
    errno = 0;
    fsti_agent_elems_init();
    simulation->name = "Default";
    fsti_config_copy(&simulation->config, config);
    fsti_dataset_hash_init(&simulation->dataset_hash);
    simulation->sim_number = sim_number;
    simulation->config_sim_number = config_sim_number;
    simulation->iteration = 0;
    simulation->matches = 0;
    simulation->initial_matches = 0;
    simulation->breakups = 0;
    simulation->initial_infections = 0;
    simulation->infections = 0;
    simulation->time_rec = time(NULL);

    simulation->results_file = simulation->agents_output_file =
        simulation->partnerships_file = stdout;

    simulation->report_frequency =
        fsti_config_at0_long(&simulation->config, "report_frequency");
    FSTI_ASSERT(simulation->report_frequency, FSTI_ERR_INVALID_VALUE,
                "Report frequency must be > 0");
    fsti_agent_arr_init(&simulation->agent_arr);
    fsti_agent_ind_init(&simulation->living, &simulation->agent_arr);
    fsti_agent_ind_init(&simulation->dead, &simulation->agent_arr);
    fsti_agent_ind_init(&simulation->mating_pool, &simulation->agent_arr);

    ARRAY_NEW(simulation->before_events, events);
    ARRAY_NEW(simulation->stabilization_events, events);
    ARRAY_NEW(simulation->during_events, events);
    ARRAY_NEW(simulation->after_events, events);


    set_all_events(simulation);

    simulation->rng = gsl_rng_alloc(gsl_rng_mt19937);
    FSTI_ASSERT(errno == 0 && simulation->rng, FSTI_ERR_NOMEM, NULL);
    gsl_rng_set(simulation->rng, sim_number);
    FSTI_ASSERT(errno == 0, FSTI_ERR_NOMEM, NULL);
    simulation->start_date = NULL;
    simulation->time_zone = NULL;
}

struct fsti_dataset *
fsti_simulation_get_dataset(struct fsti_simulation *simulation, char *key)
{
    struct fsti_dataset * dataset;
    struct fsti_config_entry *entry;
    char *filename;

    dataset = NULL;
    entry = fsti_config_find(&simulation->config, key);
    FSTI_ASSERT(entry, FSTI_ERR_KEY_NOT_FOUND, key);
    FSTI_ASSERT(entry->len && entry->variants[0].type == STR,
                FSTI_ERR_STR_EXPECTED, key);
    filename = entry->variants[0].value.str;

    if (strcmp(FSTI_NO_OP, filename)) {
        dataset = fsti_dataset_hash_find(&simulation->dataset_hash, filename);
        FSTI_ASSERT(dataset, FSTI_ERR_MISSING_DATASET, filename);
    }

    return dataset;
}

void fsti_simulation_load_datasets(struct fsti_config *config,
                                   struct fsti_dataset_hash *dataset_hash)
{
    struct fsti_config_entry *entry;
    size_t i;
    char delim;

    delim = fsti_config_at0_str(config, "csv_delimiter")[0];
    for (i = 0; i < FSTI_HASHSIZE; i++) {
        entry = config->entry[i];
        while (entry) {
            if (strncmp(entry->key, "dataset_", sizeof("dataset_")-1) == 0) {
                if (strcmp(entry->variants[0].value.str, FSTI_NO_OP)) {
                    if (entry->variants[0].type == STR)
                        fsti_dataset_hash_add(dataset_hash,
                                              entry->variants[0].value.str,
                                              delim);
                }
            }
            entry = entry->next;
        }
    }
}

void fsti_simulation_config_to_vars(struct fsti_simulation *simulation)
{
    uint16_t year = fsti_config_at0_long(&simulation->config, "start_date");
    uint16_t month = fsti_config_at_long(&simulation->config, "start_date", 1);
    uint16_t day = fsti_config_at_long(&simulation->config, "start_date", 2);
    simulation->time_zone = g_time_zone_new(NULL);
    simulation->start_date = g_date_time_new(simulation->time_zone,
                                             year, month, day, 0, 0, 0);
    FSTI_ASSERT(simulation->start_date, FSTI_ERR_INVALID_DATE, NULL);

    simulation->csv_delimiter =
        fsti_config_at0_str(&simulation->config, "csv_delimiter")[0];
    simulation->agent_csv_header =
        fsti_config_at0_long(&simulation->config, "agent_csv_header");

    simulation->record_matches =
        fsti_config_at0_long(&simulation->config, "record_matches");
    simulation->record_breakups =
        fsti_config_at0_long(&simulation->config, "record_breakups");
    simulation->record_infections =
        fsti_config_at0_long(&simulation->config, "record_infections");

    simulation->stabilization_steps = (unsigned)
	fsti_config_at0_long(&simulation->config, "stabilization_steps");
    simulation->time_step = fsti_config_at0_long(&simulation->config,
                                                 "time_step");
    simulation->age_input_time_step = fsti_config_at0_long(&simulation->config,
                                                           "age_input_time_step");

    simulation->num_time_steps = fsti_config_at0_long(&simulation->config,
                                                 "num_time_steps");
    simulation->match_k = fsti_config_at0_long(&simulation->config,
                                                          "match_k");
    simulation->initial_infect_stage =
        fsti_config_at0_long(&simulation->config, "initial_infect_stage");

    simulation->max_stage = fsti_config_at0_long(&simulation->config,
                                                 "max_stage");

    // Datasets
    if (simulation->dataset_hash.owner)
        fsti_simulation_load_datasets(&simulation->config,
                                      &simulation->dataset_hash);

    simulation->dataset_mortality =
        fsti_simulation_get_dataset(simulation, "dataset_mortality");
    simulation->dataset_single =
        fsti_simulation_get_dataset(simulation, "dataset_single_period");
    simulation->dataset_rel =
        fsti_simulation_get_dataset(simulation, "dataset_rel_period");
    simulation->dataset_infect =
        fsti_simulation_get_dataset(simulation, "dataset_infect");
    simulation->dataset_infect_stage =
        fsti_simulation_get_dataset(simulation, "dataset_infect_stage");

    simulation->dataset_gen_sex =
        fsti_simulation_get_dataset(simulation, "dataset_gen_sex");
    simulation->dataset_gen_sex_preferred =
        fsti_simulation_get_dataset(simulation, "dataset_gen_sex_preferred");
    simulation->dataset_gen_mating =
        fsti_simulation_get_dataset(simulation, "dataset_gen_mating");
    simulation->dataset_gen_infect =
        fsti_simulation_get_dataset(simulation, "dataset_gen_infect");
    simulation->dataset_gen_treated =
        fsti_simulation_get_dataset(simulation, "dataset_gen_treated");
    simulation->dataset_gen_resistant =
        fsti_simulation_get_dataset(simulation, "dataset_gen_resistant");

    simulation->dataset_birth_infect =
        fsti_simulation_get_dataset(simulation, "dataset_birth_infect");
    simulation->dataset_birth_treated =
        fsti_simulation_get_dataset(simulation, "dataset_birth_treated");
    simulation->dataset_birth_resistant =
        fsti_simulation_get_dataset(simulation, "dataset_birth_resistant");

    simulation->dataset_coinfect =
        fsti_simulation_get_dataset(simulation, "dataset_coinfect");

    // birth event vars
    simulation->birth_event_every_n =
        fsti_config_at0_long(&simulation->config, "birth_event_every_n");
    simulation->birth_rate = fsti_config_at0_double(&simulation->config,
                                                    "birth_rate");
    simulation->age_min = fsti_config_at0_double(&simulation->config, "age_min");
    simulation->age_max = fsti_config_at0_double(&simulation->config, "age_max");
    simulation->age_alpha = fsti_config_at0_double(&simulation->config,
                                                   "age_alpha");
    simulation->age_beta = fsti_config_at0_double(&simulation->config,
                                                  "age_beta");
    simulation->prob_gen_male = fsti_config_at0_double(&simulation->config,
                                                       "prob_gen_male");
    simulation->prob_gen_msw = fsti_config_at0_double(&simulation->config,
                                                       "prob_gen_msw");
    simulation->prob_gen_wsm = fsti_config_at0_double(&simulation->config,
                                                      "prob_gen_wsm");
    simulation->prob_birth_male =
        fsti_config_at0_double(&simulation->config, "prob_birth_male");
    simulation->prob_birth_msw =
        fsti_config_at0_double(&simulation->config, "prob_birth_msw");
    simulation->prob_birth_wsm =
        fsti_config_at0_double(&simulation->config, "prob_birth_wsm");
    simulation->prob_birth_infected_msm =
        fsti_config_at0_double(&simulation->config, "prob_birth_infected_msm");
    simulation->prob_birth_infected_msw =
        fsti_config_at0_double(&simulation->config, "prob_birth_infected_msw");
    simulation->prob_birth_infected_wsm =
        fsti_config_at0_double(&simulation->config, "prob_birth_infected_wsm");
    simulation->prob_birth_infected_wsw =
        fsti_config_at0_double(&simulation->config, "prob_birth_infected_wsw");

    FSTI_HOOK_CONFIG_TO_VARS(simulation);
}

void fsti_simulation_run(struct fsti_simulation *simulation)
{
    fsti_simulation_config_to_vars(simulation);
    simulation->state = BEFORE;
    exec_events(simulation, &simulation->before_events);
    if (simulation->stabilization_steps) {
        simulation->state = STABILIZATION;
        simulation->iteration = 0;
        for (uint32_t i = 0; i < simulation->stabilization_steps; i++)
            exec_events(simulation, &simulation->stabilization_events);
    }
    simulation->state = DURING;
    for (simulation->iteration = 0;
         simulation->iteration < simulation->num_time_steps;
         simulation->iteration++)
	exec_events(simulation, &simulation->during_events);

    simulation->state = AFTER;
    exec_events(simulation, &simulation->after_events);
    fflush(NULL);
}

void fsti_simulation_kill_agent(struct fsti_simulation *simulation,
                                size_t *it)
{
    struct fsti_agent *agent, *partner;
    agent = fsti_agent_ind_arrp(&simulation->living, it);
    FSTI_HOOK_PRE_DEATH(simulation, agent);
    for (size_t i = 0; i < agent->num_partners; i++) {
        size_t id = agent->partners[i];
        partner =  fsti_agent_arr_at(&simulation->agent_arr, id);
        fsti_agent_break_partners(agent, partner);
    }
    fsti_agent_ind_push(&simulation->dead, agent->id);
    agent->date_death = fsti_time_add_gdatetime(simulation->start_date,
                                                simulation->iteration,
                                                simulation->time_step);
    fsti_agent_ind_remove(&simulation->living, it);
    FSTI_HOOK_POST_DEATH(simulation, agent);
}

void fsti_simulation_free(struct fsti_simulation *simulation)
{
    fsti_dataset_hash_free(&simulation->dataset_hash);
    gsl_rng_free(simulation->rng);
    fsti_agent_arr_free(&simulation->agent_arr);
    ARRAY_FREE(simulation->before_events, events);
    ARRAY_FREE(simulation->stabilization_events, events);
    ARRAY_FREE(simulation->during_events, events);
    ARRAY_FREE(simulation->after_events, events);
    fsti_config_free(&simulation->config);
    g_date_time_unref(simulation->start_date);
    g_time_zone_unref(simulation->time_zone);
    FSTI_HOOK_SIMULATION_FREE(simulation);
}

void fsti_simulation_test(struct test_group *tg)
{
    struct fsti_simulation simulation;
    struct fsti_config config;
    struct fsti_agent *agent, *a, *b;
    double d, min_age, max_age;
    size_t *it;
    unsigned males, same_sex, infected;
    bool correct;

    fsti_event_register_events();

    // Test that a NULL simulation runs without crashing
    fsti_config_init(&config);
    fsti_config_set_default(&config);
    fsti_simulation_init(&simulation, &config, 0, 0);
    fsti_simulation_run(&simulation);
    TESTEQ(simulation.living.len == 0, true, *tg);
    fsti_config_free(&config);
    fsti_simulation_free(&simulation);

    // Now let's do a real simulation
    fsti_config_init(&config);
    fsti_config_set_default(&config);
    fsti_simulation_init(&simulation, &config, 0, 0);

    fsti_config_add(&config, "before_events","default vals",  "_generate_agents");
    fsti_config_add(&config, "during_events","default vals",  "_age");
    fsti_config_add(&config, "after_events", "write nothing", "_no_op");

    fsti_config_add(&config, "dataset_gen_sex", "default vals",
                    "dataset_gen_sex.csv");
    fsti_config_add(&config, "dataset_gen_sex_preferred", "default vals",
                    "dataset_gen_sex_preferred.csv");

    fsti_config_add(&config, "dataset_gen_infect", "default vals",
                    "dataset_gen_infect.csv");
    fsti_config_add(&config, "dataset_gen_treated", "default vals",
                    "dataset_gen_treated.csv");
    fsti_config_add(&config, "dataset_gen_resistant", "default vals",
                    "dataset_gen_resistant.csv");
    fsti_config_add(&config, "dataset_gen_mating", "default vals",
                    "dataset_gen_mating.csv");
    fsti_config_add(&config, "dataset_birth_infect", "default vals",
                    "dataset_birth_infect.csv");
    fsti_config_add(&config, "dataset_birth_treated", "default vals",
                    "dataset_birth_treated.csv");
    fsti_config_add(&config, "dataset_birth_resistant", "default vals",
                    "dataset_birth_resistant.csv");


    fsti_simulation_init(&simulation, &config, 0, 0);
    fsti_simulation_run(&simulation);

    TESTEQ(simulation.living.len > 0, true, *tg);
    TESTEQ(simulation.living.len, simulation.agent_arr.len, *tg);
    min_age = 200.0 * FSTI_YEAR;
    max_age = -min_age;
    males = same_sex = infected = 0;
    FSTI_FOR_LIVING(simulation, agent, {
            if (agent->age < min_age) min_age = agent->age;
            if (agent->age > max_age) max_age = agent->age;
            if (agent->sex == FSTI_MALE) ++males;
            if (agent->sex == agent->sex_preferred) ++same_sex;
            infected +=agent->infected;
        });

    min_age = fsti_time_in_years(min_age);
    max_age = fsti_time_in_years(max_age);
    TESTEQ(min_age >= 24.0 && min_age <= 26.0, true, *tg);
    TESTEQ(max_age >= 59.0 && max_age <= 60.0, true, *tg);
    d = (double) males / simulation.living.len;
    TESTEQ(d > 0.47 && d < 0.53, true, *tg);
    d = (double) same_sex / simulation.living.len;
    TESTEQ(d > 0.03 && d < 0.07, true, *tg);
    d = (double) infected / simulation.living.len;
    TESTEQ(d > 0.001 && d < 0.3, true, *tg);

    size_t c = 0;
    it = fsti_agent_ind_begin(&simulation.living);
    while (it < fsti_agent_ind_end(&simulation.living)) {
            if (fsti_agent_ind_arrp(&simulation.living, it)->id % 5 == 0) {
                fsti_simulation_kill_agent(&simulation, it);
                ++c;
            } else {
                ++it;
            }
    }

    TESTEQ(simulation.living.len + simulation.dead.len,
           simulation.agent_arr.len, *tg);
    TESTEQ(simulation.dead.len > 0, true, *tg);
    size_t arr[simulation.agent_arr.len];
    memset(arr, 0, sizeof(size_t) * simulation.agent_arr.len);
    correct = true;
    for (size_t i = 0; i < simulation.agent_arr.len; i++)
        if (arr[i]) {
            correct = false;
            break;
        }
    TESTEQ(correct, true, *tg);

    correct = true;
    FSTI_FOR(simulation.living, agent, {
            arr[agent->id]++;
            if (agent->id % 5 == 0) {
                correct = false;
                break;
            }
        });
    TESTEQ(correct, true, *tg);

    correct = true;
    FSTI_FOR(simulation.dead, agent, {
            arr[agent->id]++;
            if (agent->id % 5) {
                correct = false;
                break;
            }
        });
    TESTEQ(correct, true, *tg);

    correct = true;
    for (size_t i = 0; i < simulation.agent_arr.len; i++) {
        if (arr[i] != 1) {
            correct = false;
            break;
        }
    }
    TESTEQ(correct, true, *tg);

    fsti_agent_make_partners(fsti_agent_ind_arr(&simulation.living, 0),
                             fsti_agent_ind_arr(&simulation.living, 1));

    a = fsti_agent_ind_arr(&simulation.living, 0);
    b = fsti_agent_ind_arr(&simulation.living, 1);
    TESTEQ(a->num_partners, 1, *tg);
    TESTEQ(a->num_partners, 1, *tg);
    TESTEQ(b->partners[0], a->id, *tg);
    fsti_simulation_kill_agent(&simulation,
                               fsti_agent_ind_at(&simulation.living, 0));
    TESTEQ(a->partners[0], b->id, *tg);
    TESTEQ(b->partners[0], a->id, *tg);
    TESTEQ(a->num_partners, 0, *tg);
    TESTEQ(a->num_partners, 0, *tg);

    fsti_config_free(&config);
    fsti_simulation_free(&simulation);
}
