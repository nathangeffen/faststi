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
    entry = fsti_config_find(&simulation->config, "BEFORE_EVENTS");
    if (entry)
        set_events(&simulation->before_events, entry);
    entry = fsti_config_find(&simulation->config, "DURING_EVENTS");
    if (entry)
        set_events(&simulation->during_events, entry);
    entry = fsti_config_find(&simulation->config, "AFTER_EVENTS");
    if (entry)
        set_events(&simulation->after_events, entry);
}

void fsti_simulation_init(struct fsti_simulation *simulation,
			  const struct fsti_config *config,
			  int sim_number, int config_sim_number)
{
    errno = 0;
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
        fsti_config_at0_long(&simulation->config, "REPORT_FREQUENCY");
    FSTI_ASSERT(simulation->report_frequency, FSTI_ERR_INVALID_VALUE,
                "Report frequency must be > 0");
    fsti_agent_arr_init(&simulation->agent_arr);
    fsti_agent_ind_init(&simulation->living, &simulation->agent_arr);
    fsti_agent_ind_init(&simulation->dead, &simulation->agent_arr);
    fsti_agent_ind_init(&simulation->mating_pool, &simulation->agent_arr);

    ARRAY_NEW(simulation->before_events, events);
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

void fsti_simulation_config_to_vars(struct fsti_simulation *simulation)
{
    uint16_t year = fsti_config_at0_long(&simulation->config, "START_DATE");
    uint16_t month = fsti_config_at_long(&simulation->config, "START_DATE", 1);
    uint16_t day = fsti_config_at_long(&simulation->config, "START_DATE", 2);
    simulation->time_zone = g_time_zone_new(NULL);
    simulation->start_date = g_date_time_new(simulation->time_zone,
                                             year, month, day, 0, 0, 0);
    FSTI_ASSERT(simulation->start_date, FSTI_ERR_INVALID_DATE, NULL);

    simulation->csv_delimiter =
        fsti_config_at0_str(&simulation->config, "CSV_DELIMITER")[0];
    simulation->agent_csv_header =
        fsti_config_at0_long(&simulation->config, "AGENT_CSV_HEADER");

    simulation->record_matches =
        fsti_config_at0_long(&simulation->config, "OUTPUT_MATCHES");
    simulation->record_breakups =
        fsti_config_at0_long(&simulation->config, "OUTPUT_BREAKUPS");
    simulation->record_infections =
        fsti_config_at0_long(&simulation->config, "OUTPUT_INFECTIONS");

    simulation->stabilization_steps = (unsigned)
	fsti_config_at0_long(&simulation->config, "STABILIZATION_STEPS");
    simulation->time_step = fsti_config_at0_long(&simulation->config,
                                                 "TIME_STEP");
    simulation->age_input_time_step = fsti_config_at0_long(&simulation->config,
                                                           "AGE_INPUT_TIME_STEP");

    simulation->num_iterations = fsti_config_at0_long(&simulation->config,
                                                 "NUM_TIME_STEPS");
    simulation->match_k = fsti_config_at0_long(&simulation->config,
                                                          "MATCH_K");
    simulation->initial_mating_pool_prob =
        fsti_config_at0_double(&simulation->config, "INITIAL_MATING_PROB");
    simulation->mating_pool_prob =
        fsti_config_at0_double(&simulation->config, "MATING_PROB");

    for (size_t i = 0; i < FSTI_INFECTION_RISKS; i++)
        simulation->infection_risk[i] = fsti_config_at_double(
            &simulation->config, "INFECTION_RISK", i);
    simulation->max_stage = fsti_config_at0_long(&simulation->config,
                                                 "MAX_STAGE");
    simulation->initial_infection_rate =
        fsti_config_at0_double(&simulation->config, "INITIAL_INFECTION_RATE");

    simulation->initial_infect_stage =
        fsti_config_at0_long(&simulation->config, "INITIAL_INFECT_STAGE");

    simulation->initial_treated_rate =
        fsti_config_at0_double(&simulation->config, "INITIAL_TREATED_RATE");
    simulation->initial_resistant_rate =
        fsti_config_at0_double(&simulation->config, "INITIAL_RESISTANT_RATE");

    simulation->dataset_mortality =
        fsti_simulation_get_dataset(simulation, "DATASET_MORTALITY");
    simulation->dataset_single =
        fsti_simulation_get_dataset(simulation, "DATASET_SINGLE_PERIOD");
    simulation->dataset_rel =
        fsti_simulation_get_dataset(simulation, "DATASET_REL_PERIOD");
    simulation->dataset_infect_stage =
        fsti_simulation_get_dataset(simulation, "DATASET_INFECT_STAGE");
    simulation->dataset_infect =
        fsti_simulation_get_dataset(simulation, "DATASET_INFECT");
    simulation->dataset_gen_mating =
        fsti_simulation_get_dataset(simulation, "DATASET_GEN_MATING");
    simulation->dataset_gen_infect =
        fsti_simulation_get_dataset(simulation, "DATASET_GEN_INFECT");
    simulation->dataset_gen_treated =
        fsti_simulation_get_dataset(simulation, "DATASET_GEN_TREATED");
    simulation->dataset_gen_resistant =
        fsti_simulation_get_dataset(simulation, "DATASET_GEN_RESISTANT");
    simulation->dataset_coinfect =
        fsti_simulation_get_dataset(simulation, "DATASET_COINFECT");


    // Birth event vars
    simulation->birth_event_every_n =
        fsti_config_at0_long(&simulation->config, "BIRTH_EVENT_EVERY_N");
    simulation->birth_rate = fsti_config_at0_double(&simulation->config,
                                                    "BIRTH_RATE");
    simulation->age_min = fsti_config_at0_double(&simulation->config, "AGE_MIN");
    simulation->age_max = fsti_config_at0_double(&simulation->config, "AGE_MAX");
    simulation->age_alpha = fsti_config_at0_double(&simulation->config,
                                                   "AGE_ALPHA");
    simulation->age_beta = fsti_config_at0_double(&simulation->config,
                                                  "AGE_BETA");
    simulation->prob_gen_male = fsti_config_at0_double(&simulation->config,
                                                       "PROB_GEN_MALE");
    simulation->prob_gen_msw = fsti_config_at0_double(&simulation->config,
                                                       "PROB_GEN_MSW");
    simulation->prob_gen_wsm = fsti_config_at0_double(&simulation->config,
                                                      "PROB_GEN_WSM");
    simulation->prob_birth_male =
        fsti_config_at0_double(&simulation->config, "PROB_BIRTH_MALE");
    simulation->prob_birth_msw =
        fsti_config_at0_double(&simulation->config, "PROB_BIRTH_MSW");
    simulation->prob_birth_wsm =
        fsti_config_at0_double(&simulation->config, "PROB_BIRTH_WSM");
    simulation->prob_birth_infected_msm =
        fsti_config_at0_double(&simulation->config, "PROB_BIRTH_INFECTED_MSM");
    simulation->prob_birth_infected_msw =
        fsti_config_at0_double(&simulation->config, "PROB_BIRTH_INFECTED_MSW");
    simulation->prob_birth_infected_wsm =
        fsti_config_at0_double(&simulation->config, "PROB_BIRTH_INFECTED_WSM");
    simulation->prob_birth_infected_wsw =
        fsti_config_at0_double(&simulation->config, "PROB_BIRTH_INFECTED_WSW");

    FSTI_HOOK_CONFIG_TO_VARS(simulation);
}

void fsti_simulation_run(struct fsti_simulation *simulation)
{
    fsti_simulation_config_to_vars(simulation);
    simulation->state = BEFORE;
    exec_events(simulation, &simulation->before_events);
    simulation->state = DURING;
    for (simulation->iteration = 0;
         simulation->iteration < simulation->num_iterations;
         simulation->iteration++)
	exec_events(simulation, &simulation->during_events);

    simulation->state = AFTER;
    exec_events(simulation, &simulation->after_events);
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
    //unsigned partners = 0;
    //float actual_single_rate, single_rate;
    size_t *it;
    unsigned males, same_sex, infected;
    bool correct;

    fsti_event_register_events();
    fsti_config_init(&config);
    fsti_config_set_default(&config);

    FSTI_CONFIG_ADD(&config, "DATASET_GEN_TREATED", "Default vals",
                    "dataset_gen_treated.csv");
    FSTI_CONFIG_ADD(&config, "AFTER_EVENTS", "Write nothing", "_NO_OP");

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
    TESTEQ(d > 0.001 && d < 0.1, true, *tg);

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
