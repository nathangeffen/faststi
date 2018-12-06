#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

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
    char *event_name;

    errno = 0;
    simulation->name = "Default";
    fsti_config_copy(&simulation->config, config);
    simulation->sim_number = sim_number;
    simulation->config_sim_number = config_sim_number;
    simulation->iteration = 0;

    simulation->results_file = simulation->agents_output_file = stdout;

    simulation->report_frequency =
        fsti_config_at0_long(&simulation->config, "REPORT_FREQUENCY");
    FSTI_ASSERT(simulation->report_frequency, FSTI_ERR_INVALID_VALUE,
                "Report frequency must be > 0");
    simulation->stop = false;
    event_name = fsti_config_at0_str(&simulation->config, "STOP_EVENT");
    if (event_name)
	simulation->stop_event = fsti_register_get(event_name);
    else
	simulation->stop_event = NULL;

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
}

struct fsti_dataset *
fsti_simulation_get_dataset(struct fsti_simulation *simulation, char *key)
{
    struct fsti_dataset * dataset = NULL;
    char *filename = fsti_config_at0_str(&simulation->config, key);

    if (strcmp(FSTI_NO_OP, filename)) {
        dataset = fsti_dataset_hash_find(simulation->dataset_hash, filename);
        FSTI_ASSERT(dataset, FSTI_ERR_MISSING_DATASET, filename);
    }

    return dataset;
}

void fsti_simulation_config_to_vars(struct fsti_simulation *simulation)
{
    simulation->start_date = fsti_config_at0_double(&simulation->config,
						    "START_DATE");
    simulation->stabilization_steps = (unsigned)
	fsti_config_at0_long(&simulation->config, "STABILIZATION_STEPS");
    simulation->end_date = fsti_config_at0_double(&simulation->config,
						  "END_DATE");
    simulation->time_step = fsti_config_at0_double(&simulation->config,
						   "TIME_STEP");
    simulation->current_date = simulation->start_date -
	simulation->stabilization_steps * simulation->time_step;
    simulation->num_iterations = (unsigned)
	(simulation->end_date - simulation->start_date) /
	simulation->time_step;
    simulation->match_k = (unsigned) fsti_config_at0_long(&simulation->config,
                                                          "MATCH_K");
    simulation->initial_mating_pool_prob = (float)
        fsti_config_at0_double(&simulation->config, "INITIAL_MATING_PROB");
    simulation->mating_pool_prob = (float)
        fsti_config_at0_double(&simulation->config, "MATING_PROB");
    simulation->csv_delimiter =
        fsti_config_at0_str(&simulation->config, "CSV_DELIMITER")[0];
    simulation->agent_csv_header =
        fsti_config_at0_long(&simulation->config, "AGENT_CSV_HEADER");
    simulation->dataset_mortality =
        fsti_simulation_get_dataset(simulation, "DATASET_MORTALITY");
    simulation->dataset_mating_pool =
        fsti_simulation_get_dataset(simulation, "DATASET_MATING_POOL");

    FSTI_HOOK_CONFIG_TO_VARS(simulation);
}



void fsti_simulation_write_agents_ind_csv(struct fsti_simulation *simulation,
                                          struct fsti_agent_ind *agent_ind)
{
    struct fsti_agent *agent;
    FSTI_FOR(*agent_ind, agent, {
            fsti_agent_print_csv(simulation->agents_output_file,
                                 simulation->sim_number, simulation->current_date,
                                 agent, simulation->csv_delimiter);
        });
}

void fsti_simulation_write_agents_arr_csv(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;

    for (agent = simulation->agent_arr.agents;
         agent < simulation->agent_arr.agents + simulation->agent_arr.len;
         ++agent) {
        fsti_agent_print_csv(simulation->agents_output_file,
                             simulation->sim_number, simulation->current_date,
                             agent, simulation->csv_delimiter);
    }
}



void fsti_simulation_run(struct fsti_simulation *simulation)
{
    fsti_simulation_config_to_vars(simulation);
    simulation->state = BEFORE;
    exec_events(simulation, &simulation->before_events);
    FSTI_ASSERT(simulation->stop_event, FSTI_ERR_NO_STOP_EVENT, NULL);
    simulation->state = DURING;
    for (simulation->stop_event(simulation);
	 simulation->stop == false;
         simulation->stop_event(simulation),
             simulation->current_date += simulation->time_step) {
	exec_events(simulation, &simulation->during_events);
    }
    simulation->state = AFTER;
    exec_events(simulation, &simulation->after_events);
}

void fsti_simulation_kill_agent(struct fsti_simulation *simulation,
                                size_t *it)
{
    struct fsti_agent *agent, *partner;
    agent = fsti_agent_ind_arrp(&simulation->living, it);
    for (size_t i = 0; i < agent->num_partners; i++) {
        size_t id = agent->partners[i];
        partner =  fsti_agent_arr_at(&simulation->agent_arr, id);
        fsti_agent_break_partners(agent, partner);
    }
    fsti_agent_ind_push(&simulation->dead, agent->id);
    agent->date_death = simulation->current_date;
    fsti_agent_ind_remove(&simulation->living, it);

}

void fsti_simulation_free(struct fsti_simulation *simulation)
{
    gsl_rng_free(simulation->rng);
    fsti_agent_arr_free(&simulation->agent_arr);
    ARRAY_FREE(simulation->before_events, events);
    ARRAY_FREE(simulation->during_events, events);
    ARRAY_FREE(simulation->after_events, events);
    fsti_config_free(&simulation->config);
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

    FSTI_CONFIG_ADD(&config, "AFTER_EVENTS", "Write nothing", "_NO_OP");

    fsti_simulation_init(&simulation, &config, 0, 0);
    fsti_simulation_run(&simulation);

    TESTEQ(simulation.living.len > 0, true, *tg);
    TESTEQ(simulation.living.len, simulation.agent_arr.len, *tg);
    min_age = 200.0;
    max_age = -min_age;
    males = same_sex = infected = 0;
    FSTI_FOR_LIVING(simulation, agent, {
            if (agent->age < min_age) min_age = agent->age;
            if (agent->age > max_age) max_age = agent->age;
            if (agent->sex == FSTI_MALE) ++males;
            if (agent->sex == agent->sex_preferred) ++same_sex;
            infected +=agent->infected;
        });

    TESTEQ(min_age >= 25.0 && min_age <= 26.0, true, *tg);
    TESTEQ(max_age >= 59.0 && max_age <= 60.0, true, *tg);
    d = (double) males / simulation.living.len;
    TESTEQ(d > 0.47 && d < 0.53, true, *tg);
    d = (double) same_sex / simulation.living.len;
    TESTEQ(d > 0.03 && d < 0.07, true, *tg);
    d = (double) infected / simulation.living.len;
    TESTEQ(d > 0.001 && d < 0.009, true, *tg);


    /* single_rate = fsti_config_at0_double(&config, "INITIAL_SINGLE_RATE"); */

    /* actual_single_rate = (float) partners / simulation.living.len; */
    /* TESTEQ(actual_single_rate > single_rate / 2.0, true, *tg); */
    /* TESTEQ(actual_single_rate < single_rate * 1.5, true, *tg); */
    /* FSTI_FOR_LIVING(simulation, agent, { */
    /*         if (agent->num_partners) ++partners; */
    /*     }); */

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
