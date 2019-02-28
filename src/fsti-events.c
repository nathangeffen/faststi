#include <stddef.h>

#include <glib.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "fsti-defs.h"
#include "fsti-events.h"
#include "fsti-report.h"
#include "fsti-dataset.h"

struct fsti_agent fsti_global_agent;
struct test_group *fsti_events_tg;

static void
process_cell(const struct fsti_simulation *simulation,
             struct fsti_agent *agent, char *cell, void *to,
             fsti_transform_func transformer)
{
    struct fsti_variant variant;

    variant = fsti_identify_token(cell);
    transformer(to, &variant, simulation, agent);
}

void fsti_event_write_partnerships_csv_header(struct fsti_simulation *simulation)
{
    // BUG: In multithreaded run, no guarantee that this will be the top line
    // in the agent csv output, although unlikely not to be.

     if (simulation->sim_number == 0) {
        char delim = simulation->csv_delimiter;
        fprintf(simulation->partnerships_file,
                "name%csim%cnum%cdate%cagent_1%cagent_2%cdesc\n",
                delim, delim, delim, delim, delim, delim);
    }
}

static void output_partnership(struct fsti_simulation *simulation,
                               struct fsti_agent *a,
                               struct fsti_agent *b,
                               uint8_t type)
{
    char desc[20];
    char date[FSTI_DATE_LEN];
    char delim = simulation->csv_delimiter;

    switch(type) {
    case FSTI_INITIAL_MATCH: strcpy(desc, "INITIAL_MATCH"); break;
    case FSTI_MATCH: strcpy(desc, "MATCH"); break;
    case FSTI_BREAKUP: strcpy(desc, "BREAKUP"); break;
    case FSTI_INFECTION: strcpy(desc, "INFECTION"); break;
    default: strcpy(desc, "UNKNOWN");
    };
    fsti_time_add_sprint(simulation->start_date, simulation->iteration,
                             simulation->time_step, date);
    fprintf(simulation->partnerships_file, "%s%c%u%c%u%c%s%c%u%c%u%c%s\n",
            simulation->name, delim,
            simulation->sim_number, delim,
            simulation->config_sim_number, delim,
            date, delim,
            a->id, delim, b->id, delim, desc);
}

static void make_partnerships_mutual(struct fsti_agent_ind *agent_ind)
{
    struct fsti_agent *a, *b;
    size_t i;
    FSTI_FOR(*agent_ind, a, {
            for (i = 0; i < a->num_partners; i++) {
                b = fsti_agent_partner_get(agent_ind->agent_arr, a, i);
                FSTI_ASSERT(b->num_partners < FSTI_MAX_PARTNERS,
                            FSTI_ERR_OUT_OF_BOUNDS,
                            fsti_sprintf("Cannot make partnership "
                                         "for agents with ids: %zu %zu",
                                         a->id, b->id));
                fsti_agent_make_half_partner(b, a);
            }
        });
}

static
void count_initial_partnerships_infections(struct fsti_simulation *simulation)
{
    struct fsti_agent *a, *b;
    size_t i;

    FSTI_FOR_LIVING(*simulation, a, {
            if (a->infected) ++simulation->initial_infections;
            for (i = 0; i < a->num_partners; i++) {
                b = fsti_agent_partner_get(&simulation->agent_arr, a, i);
                if (b->id > a->id) {
                    ++simulation->initial_matches;
                    if (simulation->record_matches)
                        output_partnership(simulation, a, b, FSTI_INITIAL_MATCH);
                }
            }
        });
}

static void read_agents(struct fsti_simulation *simulation)
{
    size_t i, j;
    bool process_partners;
    FILE *f;
    struct csv cs;
    const char *filename;

    filename = fsti_config_at0_str(&simulation->config, "agents_input_file");
    process_partners = fsti_config_at0_long(&simulation->config,
                                            "mutual_csv_partners");
    f = fopen(filename, "r");
    FSTI_ASSERT(f, FSTI_ERR_AGENT_FILE, filename);

    cs = csv_read(f, simulation->agent_csv_header, simulation->csv_delimiter);
    FSTI_ASSERT(cs.header.len, FSTI_ERR_AGENT_FILE,
                FSTI_MSG("No header in agent csv file", filename));

    struct fsti_agent_elem *elems[cs.header.len];

    for (i = 0; i < cs.header.len; i++)
        elems[i] = fsti_agent_elem_by_strname(cs.header.cells[i]);

    for (i = 0; i < cs.len; i++) {
        memset(&simulation->csv_agent, 0, sizeof(struct fsti_agent));
        for (j = 0; j < cs.rows[i].len; ++j) {
            process_cell(simulation, &simulation->csv_agent,
                         cs.rows[i].cells[j],
                         (char *) &simulation->csv_agent + elems[j]->offset,
                         elems[j]->transformer);
            FSTI_ASSERT(fsti_error == 0,
                        FSTI_ERR_INVALID_CSV_FILE,
                        fsti_sprintf("File: %s at line %zu", filename, i+2));
        }
        fsti_agent_arr_push(&simulation->agent_arr,
                            &simulation->csv_agent);
    }
    fsti_agent_ind_fill_n(&simulation->living, cs.len);
    if (process_partners) make_partnerships_mutual(&simulation->living);
    count_initial_partnerships_infections(simulation);

    csv_free(&cs);
    fclose(f);
}

void fsti_event_read_agents(struct fsti_simulation *simulation)
{
    static GMutex mutex;
    static bool initialized_agents = false;

    g_mutex_lock (&mutex);
    if (initialized_agents == false)  {
        read_agents(simulation);
        fsti_agent_arr_copy(&fsti_saved_agent_arr,
                            &simulation->agent_arr, false);
        initialized_agents = true;
        g_mutex_unlock (&mutex);
    } else {
        g_mutex_unlock (&mutex);
        fsti_agent_arr_copy(&simulation->agent_arr,
                            &fsti_saved_agent_arr, false);
        fsti_agent_ind_fill_n(&simulation->living, simulation->agent_arr.len);
    }
}

static inline long dataset_val(const struct fsti_simulation *sim,
                               const struct fsti_agent *ag,
                               const struct fsti_dataset *ds,
                               long a,
                               long b)
{
    return (gsl_rng_uniform(sim->rng) < fsti_dataset_lookup0(ds, ag)) ? a : b;
}


void fsti_generate_age(struct fsti_simulation *simulation,
                       struct fsti_agent *agent)
{
    double a, b;
    int32_t min, max;

    a = simulation->age_alpha;
    b = simulation->age_beta;
    min = simulation->age_min;
    max = simulation->age_max;
    agent->age = gsl_ran_beta(simulation->rng, a, b) * (max - min) + min;
}



void fsti_generate_sex(struct fsti_simulation *simulation,
                       struct fsti_agent *agent)
{
    FSTI_ASSERT(simulation->dataset_gen_sex, FSTI_ERR_MISSING_DATASET,
                "For parameter dataset_gen_sex");
    agent->sex = dataset_val(simulation, agent, simulation->dataset_gen_sex,
                             FSTI_MALE, FSTI_FEMALE);
}

void fsti_generate_sex_preferred(struct fsti_simulation *simulation,
                                 struct fsti_agent *agent)
{

    FSTI_ASSERT(simulation->dataset_gen_sex_preferred, FSTI_ERR_MISSING_DATASET,
                "For parameter dataset_gen_sex_preferred");

    agent->sex_preferred =
        dataset_val(simulation, agent, simulation->dataset_gen_sex_preferred,
                    FSTI_MALE, FSTI_FEMALE);
}

static void set_infected(struct fsti_simulation *simulation,
                         struct fsti_agent *agent,
                         struct fsti_dataset *ds)
{
    double r, d;
    size_t i, num_stages, index;

    num_stages = ds->num_dependents;
    r = gsl_rng_uniform(simulation->rng);

    agent->infected = 0;
    index = fsti_dataset_lookup_index(ds, agent);
    for (i = 0; i < num_stages; i++) {
        d = fsti_dataset_get_by_index(ds, index, i);
        if (r < d) {
            agent->infected = i + 1;
            break;
        }
    }
    if (agent->infected) simulation->initial_infections++;
}

void fsti_generate_infected(struct fsti_simulation *simulation,
                            struct fsti_agent *agent)
{
    struct fsti_dataset *ds = simulation->dataset_gen_infect;

    FSTI_ASSERT(ds, FSTI_ERR_MISSING_DATASET,
                "For parameter dataset_gen_infect");
    set_infected(simulation, agent, ds);
}

static void set_treated(const struct fsti_simulation *simulation,
                        struct fsti_agent *agent,
                        struct fsti_dataset *ds)
{
    double r, d;
    size_t i, num_stages, index;

    num_stages = simulation->dataset_gen_treated->num_dependents;
    r = gsl_rng_uniform(simulation->rng);

    agent->treated = 0;
    index = fsti_dataset_lookup_index(ds, agent);
    for (i = 0; i < num_stages; i++) {
        d = fsti_dataset_get_by_index(ds, index, i);
        if (r < d) {
            agent->treated = i + 1;
            break;
        }
    }
}

void fsti_generate_treated(struct fsti_simulation *simulation,
                           struct fsti_agent *agent)
{
    struct fsti_dataset *ds;

    if (agent->infected) {
        ds = simulation->dataset_gen_treated;
        FSTI_ASSERT(ds, FSTI_ERR_MISSING_DATASET,
                    "For parameter dataset_gen_treated.");
        set_treated(simulation, agent, ds);
    }
}

void fsti_generate_resistant(struct fsti_simulation *simulation,
                             struct fsti_agent *agent)
{
    if (agent->treated) {
        FSTI_ASSERT(simulation->dataset_gen_resistant, FSTI_ERR_MISSING_DATASET,
                    "For parameter dataset_gen_resistant.");
        agent->resistant = dataset_val(simulation, agent,
                                       simulation->dataset_gen_resistant, 1, 0);
    }
}


void fsti_event_generate_agents(struct fsti_simulation *simulation)
{
    size_t i;
    struct fsti_agent agent;
    unsigned num_agents;

    num_agents = fsti_config_at0_long(&simulation->config, "num_agents");

    for (i = 0; i < num_agents; i++) {
        memset(&agent, 0, sizeof(agent));
        FSTI_AGENT_GENERATE(simulation, &agent);
        fsti_agent_arr_push(&simulation->agent_arr, &agent);
    }
    fsti_agent_ind_fill_n(&simulation->living, simulation->agent_arr.len);
}

void fsti_birth_age(struct fsti_simulation *simulation,
                    struct fsti_agent *a)
{
    a->age = simulation->age_min;
}

void fsti_birth_sex(struct fsti_simulation *simulation,
                    struct fsti_agent *agent)
{
    agent->sex =
        (gsl_rng_uniform(simulation->rng) < simulation->prob_birth_male)
        ? FSTI_MALE : FSTI_FEMALE;
}

void fsti_birth_sex_preferred(struct fsti_simulation *simulation,
                                  struct fsti_agent *a)
{
    double r = gsl_rng_uniform(simulation->rng);
    if (a->sex == FSTI_MALE) {
        if (r < simulation->prob_birth_msw)
            a->sex_preferred = FSTI_FEMALE;
        else
            a->sex_preferred = FSTI_MALE;
    } else {
        if (r < simulation->prob_birth_wsm)
            a->sex_preferred = FSTI_MALE;
        else
            a->sex_preferred = FSTI_FEMALE;
    }
}

void fsti_birth_infected(struct fsti_simulation *simulation,
                             struct fsti_agent *agent)
{
    FSTI_ASSERT(simulation->dataset_birth_infect, FSTI_ERR_MISSING_DATASET,
                "For parameter dataset_birth_sex_infect");
    set_infected(simulation, agent, simulation->dataset_birth_infect);
}

void fsti_birth_treated(struct fsti_simulation *simulation,
                            struct fsti_agent *agent)
{
    if (agent->infected) {
        FSTI_ASSERT(simulation->dataset_birth_treated, FSTI_ERR_MISSING_DATASET,
                    "For parameter dataset_birth_treated.");
        set_treated(simulation, agent, simulation->dataset_birth_treated);
    }
}

void fsti_birth_resistant(struct fsti_simulation *simulation,
                          struct fsti_agent *agent)
{
    if (agent->treated) {
        FSTI_ASSERT(simulation->dataset_gen_resistant, FSTI_ERR_MISSING_DATASET,
                    "For parameter dataset_birth_resistant.");
        agent->resistant = dataset_val(simulation, agent,
                                       simulation->dataset_gen_resistant, 1, 0);
    }
}


void fsti_event_birth(struct fsti_simulation *simulation)
{
    if (simulation->iteration % simulation->birth_event_every_n == 0) {
        double mu = simulation->birth_rate * simulation->living.len;
        uint32_t births = gsl_ran_poisson(simulation->rng, mu);

        for(size_t i = 0; i < births; i++) {
            struct fsti_agent agent;
            memset(&agent, 0, sizeof(agent));
            FSTI_AGENT_BIRTH(simulation, &agent);
            fsti_agent_arr_push(&simulation->agent_arr, &agent);
            agent.id = simulation->agent_arr.len - 1;
            fsti_agent_ind_push(&simulation->living, agent.id);
        }
    }
}

void fsti_event_shuffle_living(struct fsti_simulation *simulation)
{
    fsti_agent_ind_shuffle(&simulation->living, simulation->rng);
}

void fsti_event_shuffle_mating_pool(struct fsti_simulation *simulation)
{
    fsti_agent_ind_shuffle(&simulation->mating_pool, simulation->rng);
}

void fsti_event_age(struct fsti_simulation *simulation)
{
    struct fsti_agent *a;
    FSTI_FOR_LIVING(*simulation, a, {
            a->age += simulation->time_step;
        });
}

void fsti_event_report(struct fsti_simulation *simulation)
{
    if (simulation->state == DURING && simulation->iteration &&
        simulation->iteration % simulation->report_frequency != 0)
        return;

    FSTI_REPORT;
    FSTI_HOOK_REPORT;
}

void fsti_event_write_results_csv_header(struct fsti_simulation *simulation)
{
    // BUG: In multithreaded run, no guarantee that this will be the top line
    // in the agent csv output, although unlikely not to be.
    if (simulation->sim_number == 0)
        FSTI_REPORT_OUTPUT_HEADER(simulation->csv_delimiter);
}

static void agent_print_pretty(FILE *f, unsigned id, struct fsti_agent *agent)
{
    FSTI_AGENT_PRINT_PRETTY(f, id, agent);
}

static void write_agents_ind_csv(struct fsti_simulation *simulation,
                                 struct fsti_agent_ind *agent_ind)
{
    struct fsti_agent *agent;

    for (agent = simulation->agent_arr.agents;
         agent < simulation->agent_arr.agents + simulation->agent_arr.len;
         ++agent) {
        FSTI_AGENT_PRINT_CSV(simulation, agent, simulation->csv_delimiter);
    }
}

static void write_agents_arr_csv(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;

    for (agent = simulation->agent_arr.agents;
         agent < simulation->agent_arr.agents + simulation->agent_arr.len;
         ++agent) {
        FSTI_AGENT_PRINT_CSV(simulation, agent, simulation->csv_delimiter);
    }
}

void fsti_event_write_agents_csv_header(struct fsti_simulation *simulation)
{
    // BUG: In multithreaded run, no guarantee that this will be the top line
    // in the agent csv output, although unlikely not to be.
    if (simulation->sim_number == 0) {
        FSTI_AGENT_PRINT_CSV_HEADER(simulation->agents_output_file,
                                    simulation->csv_delimiter);
    }
}

void fsti_event_write_living_agents_csv(struct fsti_simulation *simulation)
{
    if (simulation->state == DURING && simulation->iteration &&
        simulation->iteration % simulation->report_frequency != 0)
        return;
    write_agents_ind_csv(simulation, &simulation->living);
}

void fsti_event_write_dead_agents_csv(struct fsti_simulation *simulation)
{
    if (simulation->state == DURING && simulation->iteration &&
        simulation->iteration % simulation->report_frequency != 0)
        return;
    write_agents_ind_csv(simulation, &simulation->dead);
}


void fsti_event_write_agents_csv(struct fsti_simulation *simulation)
{
    if (simulation->state == DURING && simulation->iteration &&
        simulation->iteration % simulation->report_frequency != 0)
            return;
    write_agents_arr_csv(simulation);
}

void fsti_event_write_agents_pretty(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;
    FSTI_FOR_LIVING(*simulation, agent, {
            agent_print_pretty(simulation->agents_output_file,
                               simulation->sim_number,
                               agent);
        });
}

void fsti_event_initial_mating_pool(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;
    double r, d;

    FSTI_ASSERT(simulation->dataset_gen_mating, FSTI_ERR_MISSING_DATASET,
                "For parameter dataset_gen_mating.");
    fsti_agent_ind_clear(&simulation->mating_pool);

    FSTI_FOR_LIVING(*simulation, agent, {
            if (agent->num_partners == 0) {
                r = gsl_rng_uniform(simulation->rng);
                d = fsti_dataset_lookup0(simulation->dataset_gen_mating, agent);
                if (r < d)
                    fsti_agent_ind_push(&simulation->mating_pool, agent->id);
            }
        });
}

void fsti_event_mating_pool(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;
    fsti_agent_ind_clear(&simulation->mating_pool);
    FSTI_FOR_LIVING(*simulation, agent, {
            if (agent->num_partners == 0) {
                if (agent->relchange[0] < simulation->iteration)
                    fsti_agent_ind_push(&simulation->mating_pool, agent->id);
            }
        });
}

static void
set_rel_period(struct fsti_simulation *simulation, struct fsti_agent *a)
{
    double scale, shape;
    uint32_t iterations;

    FSTI_ASSERT(simulation->dataset_rel, FSTI_ERR_MISSING_DATASET,
                        "For parameter dataset_rel_period.");
    scale = fsti_dataset_lookup(simulation->dataset_rel, a, 0);
    shape = fsti_dataset_lookup(simulation->dataset_rel, a, 1);
    iterations = gsl_ran_weibull(simulation->rng, scale, shape);
    a->relchange[0] = simulation->iteration + iterations;

}

static void make_partners(struct fsti_simulation *simulation,
                          struct fsti_agent *a, struct fsti_agent *b)
{
    fsti_agent_make_partners(a, b);
    set_rel_period(simulation, a);
    b->relchange[0] = a->relchange[0];
    if (simulation->state == DURING) {
        ++simulation->matches;
        if (simulation->record_matches)
            output_partnership(simulation, a, b, FSTI_MATCH);
    } else if (simulation->state == BEFORE) {
        ++simulation->initial_matches;
        if (simulation->record_matches)
            output_partnership(simulation, a, b, FSTI_INITIAL_MATCH);
    }
}

static void
set_single_period(struct fsti_simulation *simulation, struct fsti_agent *a)
{
    double scale, shape;
    uint32_t iterations;

    FSTI_ASSERT(simulation->dataset_single, FSTI_ERR_MISSING_DATASET,
                "For parameter dataset_single_period.");
    scale = fsti_dataset_lookup(simulation->dataset_single, a, 0);
    shape = fsti_dataset_lookup(simulation->dataset_single, a, 1);
    iterations = gsl_ran_weibull(simulation->rng, scale, shape);
    a->relchange[0] = simulation->iteration + iterations;
}

void fsti_event_initial_relchange(struct fsti_simulation *simulation)
{
    struct fsti_agent *a, *b;

    FSTI_FOR_LIVING(*simulation, a, {
            b = fsti_agent_partner_get0(&simulation->agent_arr, a);
            if (b) {
                a->relchange[0] =
                    gsl_rng_uniform(simulation->rng) * (double) a->relchange[0];
                b->relchange[0] = a->relchange[0];
            } else {
                FSTI_SET_SINGLE_PERIOD(simulation, a);
                // Initial period averages to half
                a->relchange[0] =
                    gsl_rng_uniform(simulation->rng) * (double) a->relchange[0];
            }
        });
}

void fsti_event_breakup(struct fsti_simulation *simulation)
{
    struct fsti_agent *a, *b;

    FSTI_FOR_LIVING(*simulation, a, {
            // If agent is in partnership and duration of partnership has run out
            b = fsti_agent_partner_get0(&simulation->agent_arr, a);
            if (b && a->relchange[0] < simulation->iteration) {
                fsti_agent_break_partners(a, b);
                // Determine at what day in the future these agents will start
                // looking for new partners

                FSTI_SET_SINGLE_PERIOD(simulation, a);
                FSTI_SET_SINGLE_PERIOD(simulation, b);
                simulation->breakups++;
                if (simulation->record_breakups)
                    output_partnership(simulation, a, b, FSTI_BREAKUP);
            }
        });
}

void fsti_event_death(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;
    double d, r;
    size_t *it;

    FSTI_ASSERT(simulation->dataset_mortality, FSTI_ERR_MISSING_DATASET,
                "For parameter dataset_mortality.");

    it = simulation->living.indices;
    while (it < (simulation->living.indices + simulation->living.len)) {
        agent = fsti_agent_ind_arrp(&simulation->living, it);
        d = fsti_dataset_lookup0(simulation->dataset_mortality, agent);
        r = gsl_rng_uniform(simulation->rng);
        if (r < d)
            fsti_simulation_kill_agent(simulation, it);
        else
            ++it;
    }
}

void fsti_event_knn_match(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent, *candidate, *partner;
    size_t *it, *jt, *start, *n, *begin, *end;
    size_t num_agents = simulation->mating_pool.len;
    float d, best_dist;
    size_t k = simulation->match_k;

    // Too few agents to bother
    if (num_agents < 2) return;
    // Can only match even number of agents
    if (num_agents % 2) fsti_agent_ind_pop(&simulation->mating_pool);

    begin = fsti_agent_ind_begin(&simulation->mating_pool);
    end = fsti_agent_ind_end(&simulation->mating_pool);

    for (it = begin; it < end - 1; it++) {
        agent = fsti_agent_ind_arrp(&simulation->mating_pool, it);
        if (FSTI_AGENT_HAS_PARTNER(agent)) continue;
        start = it + 1;
        n = (start + k) < end ? (start + k) : end;
        best_dist = FLT_MAX;
        partner = NULL;
        for (jt = start; jt < n; jt++) {
            candidate = fsti_agent_ind_arrp(&simulation->mating_pool, jt);
            if (FSTI_AGENT_HAS_PARTNER(candidate)) continue;
            d = FSTI_AGENT_DISTANCE(agent, candidate);
            if (d < best_dist) {
                best_dist = d;
                partner = candidate;
            }
        }
        if (partner) {
            make_partners(simulation, agent, partner);
            FSTI_HOOK_AFTER_MATCH(simulation, agent, partner);
        }
    }
}

void fsti_event_breakup_and_pair(struct fsti_simulation *simulation)
{
    fsti_event_breakup(simulation);
    fsti_event_mating_pool(simulation);
    fsti_event_shuffle_mating_pool(simulation);
    fsti_event_knn_match(simulation);
}


void fsti_event_generate_and_pair(struct fsti_simulation *simulation)
{
    fsti_event_generate_agents(simulation);
    fsti_event_initial_mating_pool(simulation);
    fsti_event_shuffle_mating_pool(simulation);
    fsti_event_knn_match(simulation);
    fsti_event_initial_relchange(simulation);
}

void fsti_event_infect(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent, *partner;
    size_t i;
    double d, r;

    FSTI_ASSERT(simulation->dataset_infect, FSTI_ERR_MISSING_DATASET,
                "For parameter dataset_infect.");
    FSTI_FOR_LIVING(*simulation, agent, {
            if (agent->infected == 0) {
                for (i = 0; i < agent->num_partners; i++) {
                    partner = fsti_agent_arr_at(&simulation->agent_arr,
                                                agent->partners[i]);
                    if (partner->infected) {
                        d = fsti_dataset_lookup_x2(simulation->dataset_infect,
                                                   agent, partner, 0);
                        r = gsl_rng_uniform(simulation->rng);
                        if (r < d) {
                            agent->infected = simulation->initial_infect_stage;
                            simulation->infections++;
                            if (simulation->record_infections)
                                output_partnership(simulation, agent, partner,
                                                   FSTI_INFECTION);
                            break;
                        }
                    }
                }
            }
        });
}

void fsti_event_infect_stage(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;
    size_t index;
    long change;
    double d, r;

    FSTI_ASSERT(simulation->dataset_infect_stage, FSTI_ERR_MISSING_DATASET,
                "For parameter dataset_infect_stage.");

    FSTI_FOR_LIVING(*simulation, agent, {
            if (agent->infected > 0) {
                index =
                    fsti_dataset_lookup_index(simulation->dataset_infect_stage,
                                              agent);

                // Check for stage change
                d = fsti_dataset_get_by_index(simulation->dataset_infect_stage,
                                              index, 0);
                r = gsl_rng_uniform(simulation->rng);
                if (r < d) {
                    change = fsti_dataset_get_by_index(
                        simulation->dataset_infect_stage, index, 1);
                    agent->infected += change;
                }

                // Check for treatment change
                d = fsti_dataset_get_by_index(simulation->dataset_infect_stage,
                                              index, 2);
                r = gsl_rng_uniform(simulation->rng);
                if (r < d) {
                    change = fsti_dataset_get_by_index(
                        simulation->dataset_infect_stage, index, 3);
                    agent->treated += change;
                }

                // Check for resistance change
                d = fsti_dataset_get_by_index(simulation->dataset_infect_stage,
                                              index, 4);
                r = gsl_rng_uniform(simulation->rng);
                if (r < d) {
                    change = fsti_dataset_get_by_index(
                        simulation->dataset_infect_stage, index, 5);
                    agent->resistant += change;
                }
            }
        });
}

void fsti_event_coinfect(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;
    double d, r;

    FSTI_ASSERT(simulation->dataset_coinfect, FSTI_ERR_MISSING_DATASET,
                "For parameter dataset_coinfect.");

    FSTI_FOR_LIVING(*simulation, agent, {
            if (agent->coinfected == 0) {
                d = fsti_dataset_lookup0(simulation->dataset_coinfect, agent);
                r = gsl_rng_uniform(simulation->rng);
                if (r < d) agent->coinfected = 1;
            }
        });
}

void fsti_event_no_op(struct fsti_simulation *simulation)
{
    return;
}

/* Test Events */

/* These are for testing only and not designed for speed. */

bool run_test(struct fsti_simulation *simulation)
{
    unsigned freq = fsti_config_at0_long(&simulation->config, "event_test_freq");
    freq  = !((simulation->iteration + 1) % freq);
    return freq;
}


void fsti_event_test_breakup(struct fsti_simulation *simulation)
{
    struct fsti_agent *a;
    static _Thread_local uint32_t breakup_before = 0;
    uint32_t breakup_after = 0;

    FSTI_FOR_LIVING(*simulation, a, {
            if (a->num_partners == 1 &&
                simulation->iteration > a->relchange[0])
                breakup_before++;
        });

    fsti_event_breakup(simulation);
    if (run_test(simulation)) {
        FSTI_FOR_LIVING(*simulation, a, {
                if (a->num_partners == 1 &&
                    simulation->iteration > a->relchange[0])
                    breakup_after++;
            });
        TESTEQ(breakup_after == 0, true, *fsti_events_tg);
    }
    if (simulation->iteration == simulation->num_time_steps - 1) {
        TESTEQ(breakup_before > 5000, true, *fsti_events_tg);
        breakup_before = 0;
    }
}


void fsti_event_test_mating_pool(struct fsti_simulation *simulation)
{
    struct fsti_agent *a;
    bool bad_logic;
    static _Thread_local uint32_t num_in_pool = 0;

    fsti_event_mating_pool(simulation);
    num_in_pool += simulation->mating_pool.len;

    if (run_test(simulation)) {
        if(simulation->mating_pool.len > 1) {
            bad_logic = false;
            FSTI_FOR(simulation->mating_pool, a, {
                    if (a->relchange[0] >= simulation->iteration ||
                        a->num_partners > 0) {
                        bad_logic = true;
                        break;
                    }
                });
            TESTEQ(bad_logic, false, *fsti_events_tg);
        }
    }
    if (simulation->iteration == simulation->num_time_steps - 1) {
        TESTEQ(num_in_pool > 5000, true, *fsti_events_tg);
        num_in_pool = 0;
    }
}


void fsti_event_test_shuffle_mating(struct fsti_simulation *simulation)
{
    struct fsti_agent *a;
    uint32_t *it, agents[simulation->mating_pool.len], differences = 0;

    if (run_test(simulation) && simulation->mating_pool.len > 10) {
        it = agents;
        FSTI_FOR(simulation->mating_pool, a, {
                *it++ = a->id;
            });

        it = agents;
        FSTI_FOR(simulation->mating_pool, a, {
                if (*it++ != a->id) differences++;
            });
        TESTEQ(differences, 0, *fsti_events_tg);
    }

    fsti_event_shuffle_mating_pool(simulation);

    differences = 0;
    it = agents;
    if (run_test(simulation) && simulation->mating_pool.len > 10) {
        FSTI_FOR(simulation->mating_pool, a, {
                if (*it++ != a->id) differences++;
            });
        double prop = (double) differences / simulation->mating_pool.len;
        TESTEQ(prop > 0.85, true, *fsti_events_tg);
    }
}

void fsti_event_test_knn_match(struct fsti_simulation *simulation)
{
    size_t n = simulation->mating_pool.len;
    bool bad_logic;
    unsigned matches_before = 0, matches_after = 0;
    struct fsti_agent *a;
    static _Atomic bool matches_made = false;

    if (n > 1) matches_made = true;

    if (run_test(simulation)) {
        FSTI_FOR_LIVING(*simulation, a, {
                if (a->num_partners) ++matches_before;
            });
    }

    fsti_event_knn_match(simulation);

    if (run_test(simulation)) {
        bad_logic = false;
        FSTI_FOR_LIVING(*simulation, a, {
                if (a->num_partners) {
                    ++matches_after;
                    if (a->relchange[0] < simulation->iteration)
                        bad_logic = true;
                }
            });

        if (n > 1) {
            TESTEQ(bad_logic, false, *fsti_events_tg);
            TESTEQ(matches_after > matches_before, true, *fsti_events_tg);
            matches_after -= matches_before;
            if (n % 2 != 0) --n;
            TESTEQ(matches_after, n, *fsti_events_tg);
        }
    }

    // Check that there were at least some iterations where matches occurred
    if (simulation->iteration == simulation->num_time_steps - 1) {
        TESTEQ(matches_made > 0, true, *fsti_events_tg);
        matches_made = 0;
    }
}

void fsti_event_test_infect(struct fsti_simulation *simulation)
{
    struct fsti_agent *a;
    uint32_t infected_before = 0, infected_after = 0;
    static _Thread_local uint32_t infections = 0;

    FSTI_FOR_LIVING(*simulation, a, {
            if (a->infected) ++infected_before;
        });

    fsti_event_infect(simulation);

    FSTI_FOR_LIVING(*simulation, a, {
            if (a->infected) ++infected_after;
        });
    infections += infected_after - infected_before;

    if (run_test(simulation))
        TESTEQ(infected_before <= infected_after, true, *fsti_events_tg);

    if (simulation->iteration == simulation->num_time_steps - 1) {
        TESTEQ(infections > 0, true, *fsti_events_tg);
        infections = 0;
    }
}

void fsti_event_test_birth(struct fsti_simulation *simulation)
{
    static _Thread_local uint32_t births = 0;
    uint32_t births_before, births_after;

    births_before = simulation->living.len;
    fsti_event_birth(simulation);
    births_after = simulation->living.len;
    births += births_after - births_before;


    if (simulation->iteration == simulation->num_time_steps - 1) {
        TESTEQ(births > 0, true, *fsti_events_tg);
        births = 0;
    }
}

void fsti_event_test_death(struct fsti_simulation *simulation)
{
    static _Thread_local uint32_t deaths = 0;
    uint32_t deaths_before, deaths_after;

    deaths_before = simulation->dead.len;
    fsti_event_death(simulation);
    deaths_after = simulation->dead.len;
    deaths += deaths_after - deaths_before;

    if (simulation->iteration == simulation->num_time_steps - 1) {
        TESTEQ(deaths > 0, true, *fsti_events_tg);
        deaths = 0;
    }
}

/* End of test events */

void fsti_event_register_events()
{
    static bool initialized_events = false;

    if (initialized_events == false) {
        initialized_events = true;
        fsti_register_add("_read_agents", fsti_event_read_agents);
        fsti_register_add("_generate_agents", fsti_event_generate_agents);
        fsti_register_add("_age", fsti_event_age);
        fsti_register_add("_death", fsti_event_death);
        fsti_register_add("_initial_mating", fsti_event_initial_mating_pool);
        fsti_register_add("_initial_rel", fsti_event_initial_relchange);
        fsti_register_add("_mating_pool", fsti_event_mating_pool);
        fsti_register_add("_breakup", fsti_event_breakup);
        fsti_register_add("_shuffle_living", fsti_event_shuffle_living);
        fsti_register_add("_shuffle_mating", fsti_event_shuffle_mating_pool);
        fsti_register_add("_rkpm", fsti_event_knn_match);
        fsti_register_add("_breakup_and_pair", fsti_event_breakup_and_pair);
        fsti_register_add("_generate_and_pair", fsti_event_generate_and_pair);
        fsti_register_add("_infect", fsti_event_infect);
        fsti_register_add("_stage", fsti_event_infect_stage);
        fsti_register_add("_coinfect", fsti_event_coinfect);
        fsti_register_add("_birth", fsti_event_birth);
        fsti_register_add("_report", fsti_event_report);
        fsti_register_add("_write_results_csv_header",
                          fsti_event_write_results_csv_header);
        fsti_register_add("_write_agents_csv_header",
                          fsti_event_write_agents_csv_header);
        fsti_register_add("_write_partnerships_csv_header",
                          fsti_event_write_partnerships_csv_header);
        fsti_register_add("_write_agents_csv", fsti_event_write_agents_csv);
        fsti_register_add("_write_living_agents_csv",
                          fsti_event_write_living_agents_csv);
        fsti_register_add("_write_dead_agents_csv",
                          fsti_event_write_dead_agents_csv);
        fsti_register_add("_write_agents_pretty", fsti_event_write_agents_pretty);
        fsti_register_add(FSTI_NO_OP, fsti_event_no_op);

        /* test events */
        fsti_register_add("_test_breakup", fsti_event_test_breakup);
        fsti_register_add("_test_mating_pool", fsti_event_test_mating_pool);
        fsti_register_add("_test_shuffle_mating", fsti_event_test_shuffle_mating);
        fsti_register_add("_test_rkpm", fsti_event_test_knn_match);
        fsti_register_add("_test_infect", fsti_event_test_infect);
        fsti_register_add("_test_birth", fsti_event_test_birth);
        fsti_register_add("_test_death", fsti_event_test_death);

        FSTI_HOOK_EVENTS_REGISTER;
    }
}
