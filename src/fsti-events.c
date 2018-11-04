#include <glib.h>
#include <stddef.h>
#include "fsti-defs.h"
#include "fsti-events.h"
#include "fsti-report.h"

struct fsti_agent fsti_global_agent;

const struct fsti_csv_entry fsti_csv_entries[] = {
    FSTI_CSV_ENTRIES
};



const struct fsti_csv_agent fsti_global_csv =
{
    .agent = &fsti_global_agent,
    .num_entries = sizeof(fsti_csv_entries) / sizeof(struct fsti_csv_entry),
    .entries = fsti_csv_entries
};


void fsti_to_float(void *to, const struct fsti_variant *from,
                   struct fsti_agent *agent)
{
    float v;
    switch(from->type) {
    case DBL: v = from->value.dbl; break;
    case LONG: v = from->value.longint; break;
    default: fsti_error = FSTI_ERR_INVALID_VALUE; return;
    }
    memcpy(to, &v, sizeof(float));
}

void fsti_to_double(void *to, const struct fsti_variant *from,
                   struct fsti_agent *agent)
{
    double v;
    switch(from->type) {
    case DBL: v = from->value.dbl; break;
    case LONG: v = from->value.longint; break;
    default: fsti_error = FSTI_ERR_INVALID_VALUE; return;
    }
    memcpy(to, &v, sizeof(double));
}

void fsti_to_int(void *to, const struct fsti_variant *from,
                  struct fsti_agent *agent)
{
    int v;
    switch(from->type) {
    case DBL: v = from->value.dbl; break;
    case LONG: v = from->value.longint; break;
    default: fsti_error = FSTI_ERR_INVALID_VALUE; return;
    }
    memcpy(to, &v, sizeof(int));
}

void fsti_to_uint8_t(void *to, const struct fsti_variant *from,
                      struct fsti_agent *agent)
{
    uint8_t v;
    switch(from->type) {
    case DBL: v = from->value.dbl; break;
    case LONG: v = from->value.longint; break;
    default: fsti_error = FSTI_ERR_INVALID_VALUE; return;
    }
    memcpy(to, &v, sizeof(uint8_t));
}

void fsti_to_uint16_t(void *to, const struct fsti_variant *from,
                      struct fsti_agent *agent)
{
    uint16_t v;
    switch(from->type) {
    case DBL: v = from->value.dbl; break;
    case LONG: v = from->value.longint; break;
    default: fsti_error = FSTI_ERR_INVALID_VALUE; return;
    }
    memcpy(to, &v, sizeof(uint16_t));
}

void fsti_to_uint32_t(void *to, const struct fsti_variant *from,
                      struct fsti_agent *agent)
{
    uint32_t v;
    switch(from->type) {
    case DBL: v = from->value.dbl; break;
    case LONG: v = from->value.longint; break;
    default: fsti_error = FSTI_ERR_INVALID_VALUE; return;
    }
    memcpy(to, &v, sizeof(uint32_t));
}

void fsti_to_uint64_t(void *to, const struct fsti_variant *from,
                      struct fsti_agent *agent)
{
    uint64_t v;
    switch(from->type) {
    case DBL: v = from->value.dbl; break;
    case LONG: v = from->value.longint; break;
    default: fsti_error = FSTI_ERR_INVALID_VALUE; return;
    }
    memcpy(to, &v, sizeof(uint64_t));
}


void fsti_to_bool(void *to, const struct fsti_variant *from,
                  struct fsti_agent *agent)
{
    bool v;
    switch(from->type) {
    case DBL: v = from->value.dbl; break;
    case LONG: v = from->value.longint; break;
    default: fsti_error = FSTI_ERR_INVALID_VALUE; return;
    }
    memcpy(to, &v, sizeof(bool));
}


void fsti_to_unsigned(void *to, const struct fsti_variant *from,
                      struct fsti_agent *agent)
{
    unsigned v;
    switch(from->type) {
    case DBL: v = from->value.dbl; break;
    case LONG: v = from->value.longint; break;
    default: fsti_error = FSTI_ERR_INVALID_VALUE; return;
    }
    memcpy(to, &v, sizeof(unsigned));
}

void fsti_to_size_t(void *to, const struct fsti_variant *from,
                    struct fsti_agent *agent)
{
    size_t v;
    switch(from->type) {
    case DBL: v = from->value.dbl; break;
    case LONG: v = from->value.longint; break;
    default: fsti_error = FSTI_ERR_INVALID_VALUE; return;
    }
    memcpy(to, &v, sizeof(size_t));
}

void fsti_to_uchar(void *to, const struct fsti_variant *from,
                   struct fsti_agent *agent)
{
    unsigned char v;
    switch(from->type) {
    case DBL: v = (unsigned char) from->value.dbl; break;
    case LONG: v = (unsigned char) from->value.longint; break;
    default: fsti_error = FSTI_ERR_INVALID_VALUE; return;
    }
    memcpy(to, &v, sizeof(unsigned char));
}

void fsti_to_partner(void *to, const struct fsti_variant *from,
                     struct fsti_agent *agent)
{
    long v;
    size_t i;
    switch(from->type) {
    case DBL: v = (long) from->value.dbl; break;
    case LONG: v = (long) from->value.longint; break;
    default: fsti_error = FSTI_ERR_INVALID_VALUE; return;
    }
    if (v >= 0) {
        i = (unsigned) v;
        agent->num_partners++;
        memcpy(to, &i, sizeof(size_t));
    } else {
        memset(to, 0, sizeof(size_t));
    }
}


static void
process_cell(struct fsti_agent *agent, char *cell, void *to,
             fsti_transform_func transformer)
{
    struct fsti_variant variant;

    variant = fsti_identify_token(cell);
    transformer(to, &variant, agent);
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

static void read_agents(struct fsti_simulation *simulation)
{
    size_t i, j;
    bool process_partners;
    FILE *f;
    struct csv cs;
    const char *filename;

    filename = fsti_config_at0_str(&simulation->config, "AGENTS_INPUT_FILE");
    process_partners = fsti_config_at0_long(&simulation->config,
                                            "MUTUAL_CSV_PARTNERS");
    f = fopen(filename, "r");
    FSTI_ASSERT(f, FSTI_ERR_AGENT_FILE, filename);

    cs = csv_read(f, simulation->agent_csv_header, simulation->csv_delimiter);

    //fsti_agent_arr_fill_n(&simulation->agent_arr, cs.len);

    for (i = 0; i < cs.len; ++i) {
        memset(simulation->csv->agent, 0, sizeof(struct fsti_agent));
        FSTI_ASSERT(cs.rows[i].len == simulation->csv->num_entries,
                    FSTI_ERR_INVALID_CSV_FILE,
                    fsti_sprintf("File: %s at line %zu", filename, i+2));
        for (j = 0; j < cs.rows[i].len; ++j) {
            FSTI_ASSERT(j < simulation->csv->num_entries,
                        FSTI_ERR_INVALID_CSV_FILE,
                        fsti_sprintf("File: %s at line %zu", filename, i+2));
            process_cell(simulation->csv->agent,
                         cs.rows[i].cells[j],
                         simulation->csv->entries[j].dest,
                         simulation->csv->entries[j].transformer);
            FSTI_ASSERT(fsti_error == 0,
                        FSTI_ERR_INVALID_CSV_FILE,
                        fsti_sprintf("File: %s at line %zu", filename, i+2));
        }
        fsti_agent_arr_push(&simulation->agent_arr,
                            simulation->csv->agent);
    }
    fsti_agent_ind_fill_n(&simulation->living, cs.len);
    if (process_partners)
        make_partnerships_mutual(&simulation->living);

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

void fsti_event_create_agents(struct fsti_simulation *simulation)
{
    size_t i;
    struct fsti_agent agent;

    unsigned initial_num_agents = (unsigned)
        fsti_config_at0_long(&simulation->config, "NUM_AGENTS");
    float prob_male = (float) fsti_config_at0_double(&simulation->config,
                                                 "PROB_MALE");
    float prob_msm = (float) fsti_config_at0_double(&simulation->config,
                                                "PROB_MSM");
    float prob_wsw = (float) fsti_config_at0_double(&simulation->config,
                                                  "PROB_WSW");
    float age_min = (float) fsti_config_at0_double(&simulation->config,
                                                 "AGE_MIN");
    float age_range = (float) fsti_config_at0_double(&simulation->config,
                                                   "AGE_RANGE");
    float initial_infection_rate = (float)
        fsti_config_at0_double(&simulation->config, "INITIAL_INFECTION_RATE");
    float initial_single_rate = (float)
        fsti_config_at0_double(&simulation->config, "INITIAL_SINGLE_RATE");

    for (i = 0; i < initial_num_agents; i++) {
        agent.id = i;
        agent.sex = gsl_rng_uniform(simulation->rng) < prob_male ?
            FSTI_MALE : FSTI_FEMALE;
        if (agent.sex == FSTI_MALE) {
            agent.sex_preferred =
                gsl_rng_uniform(simulation->rng) < prob_msm ?
                FSTI_MALE : FSTI_FEMALE;
        } else {
            agent.sex_preferred =
                gsl_rng_uniform(simulation->rng) < prob_wsw ?
                FSTI_FEMALE : FSTI_MALE;
        }
        agent.age = gsl_rng_uniform_int(simulation->rng, age_range)
            + age_min;
        agent.infected = gsl_rng_uniform(simulation->rng) <
            initial_infection_rate ? 1.0 : 0.0;
        agent.cured = 0.0;
        agent.date_death = 0.0;
        agent.cause_of_death = 0;
        if (i % 2 == 0  ||
            (gsl_rng_uniform(simulation->rng) < initial_single_rate)){
            agent.num_partners = 0;
        } else {
            agent.num_partners = 1;
            agent.partners[0] = i - 1;
        }
        FSTI_HOOK_CREATE_AGENT(simulation, agent);
        fsti_agent_arr_push(&simulation->agent_arr, &agent);
    }
    fsti_agent_ind_fill_n(&simulation->living, simulation->agent_arr.len);
    make_partnerships_mutual(&simulation->living);
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

static void outputf(struct fsti_simulation *simulation, char *desc, double val)
{
    char c = simulation->csv_delimiter;
    fprintf(simulation->results_file, "%s%c%d%c%d%c%s%c%f\n",
            simulation->name, c, simulation->sim_number,c,
            simulation->config_sim_number, c, desc, c, val);
}

static void outputl(struct fsti_simulation *simulation, char *desc, long val)
{
    char c = simulation->csv_delimiter;
    fprintf(simulation->results_file, "%s%c%d%c%d%c%s%c%ld\n",
            simulation->name, c, simulation->sim_number, c,
            simulation->config_sim_number, c, desc, c, val);
}


void fsti_event_report(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;
    double age_avg = 0.0;
    unsigned infections = 0;
    long num_partners = 0;

    if (simulation->state == DURING && simulation->iteration &&
        simulation->iteration % simulation->report_frequency != 0)
        return;

    outputl(simulation, "POPULATION_ALIVE", simulation->living.len);

    FSTI_FOR_LIVING(*simulation, agent, {
            age_avg += agent->age;
            infections += (bool) agent->infected;
            num_partners += agent->num_partners;
        });

    outputf(simulation, "AVERAGE_AGE", age_avg / simulation->agent_arr.len);
    outputf(simulation, "INFECTION_RATE",
           (double) infections / simulation->agent_arr.len);
    outputl(simulation, "NUM_PARTNERS", num_partners);
}

void fsti_event_flex_report(struct fsti_simulation *simulation)
{
    if (simulation->state == DURING && simulation->iteration &&
        simulation->iteration % simulation->report_frequency != 0)
        return;

    FSTI_FLEX_REPORT;
    FSTI_HOOK_FLEX_REPORT;
}

void fsti_event_write_results_csv_header(struct fsti_simulation *simulation)
{
    // BUG: In multithreaded run, no guarantee that this will be the top line
    // in the agent csv output, although unlikely not to be.
    if (simulation->sim_number == 0)
        FSTI_REPORT_OUTPUT_HEADER(simulation->csv_delimiter);
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
    fsti_simulation_write_agents_ind_csv(simulation, &simulation->living);
}

void fsti_event_write_dead_agents_csv(struct fsti_simulation *simulation)
{
    fsti_simulation_write_agents_ind_csv(simulation, &simulation->dead);
}


void fsti_event_write_agents_csv(struct fsti_simulation *simulation)
{
    fsti_simulation_write_agents_arr_csv(simulation);
}

void fsti_event_write_agents_pretty(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;
    FSTI_FOR_LIVING(*simulation, agent, {
            fsti_agent_print_pretty(simulation->agents_output_file,
                                    simulation->sim_number,
                                    agent);
        });
}

void fsti_event_mating_pool(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;
    fsti_agent_ind_clear(&simulation->mating_pool);
    FSTI_FOR_LIVING(*simulation, agent, {
            if (agent->num_partners == 0) {
                double prob = gsl_rng_uniform(simulation->rng);
                if (prob < simulation->mating_pool_prob)
                    fsti_agent_ind_push(&simulation->mating_pool, agent->id);
            }
        });
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
            fsti_agent_make_partners(agent, partner);
            FSTI_HOOK_AFTER_MATCH(simulation, agent, partner);
        }
    }
}


void fsti_event_stop(struct fsti_simulation *simulation)
{
    if (simulation->iteration >= simulation->num_iterations) {
	simulation->stop = true;
    }  else {
	++simulation->iteration;
    }
}

void fsti_event_no_op(struct fsti_simulation *simulation)
{
    return;
}

void fsti_event_register_events()
{
    static bool initialized_events = false;

    if (initialized_events == false) {
        initialized_events = true;
        fsti_register_add("_READ_AGENTS", fsti_event_read_agents);
        fsti_register_add("_GENERATE_AGENTS", fsti_event_create_agents);
        fsti_register_add("_AGE", fsti_event_age);
        fsti_register_add("_MATING_POOL", fsti_event_mating_pool);
        fsti_register_add("_SHUFFLE_LIVING", fsti_event_shuffle_living);
        fsti_register_add("_SHUFFLE_MATING", fsti_event_shuffle_mating_pool);
        fsti_register_add("_RKPM", fsti_event_knn_match);
        fsti_register_add("_REPORT", fsti_event_report);
        fsti_register_add("_FLEX_REPORT", fsti_event_flex_report);
        fsti_register_add("_WRITE_RESULTS_CSV_HEADER",
                          fsti_event_write_results_csv_header);
        fsti_register_add("_WRITE_AGENTS_CSV_HEADER",
                          fsti_event_write_agents_csv_header);
        fsti_register_add("_WRITE_AGENTS_CSV", fsti_event_write_agents_csv);
        fsti_register_add("_WRITE_LIVING_AGENTS_CSV",
                          fsti_event_write_living_agents_csv);
        fsti_register_add("_WRITE_DEAD_AGENTS_CSV",
                          fsti_event_write_dead_agents_csv);
        fsti_register_add("_WRITE_AGENTS_PRETTY", fsti_event_write_agents_pretty);
        fsti_register_add("_STOP", fsti_event_stop);
        fsti_register_add("_NO_OP", fsti_event_no_op);
        FSTI_HOOK_EVENTS_REGISTER;
    }
}
