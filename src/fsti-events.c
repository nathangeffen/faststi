#include <glib.h>
#include <stddef.h>
#include "fsti-defs.h"
#include "fsti-events.h"

void fsti_to_float(void *to, const struct fsti_variant *from,
                   struct fsti_agent *agent)
{
    float v;
    switch(from->type) {
    case DBL: v = from->value.dbl; break;
    case LONG: v = from->value.longint; break;
    default: FSTI_ASSERT(0, FSTI_ERR_INVALID_VALUE, NULL);
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
    default: FSTI_ASSERT(0, FSTI_ERR_INVALID_VALUE, NULL);
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
    default: FSTI_ASSERT(0, FSTI_ERR_INVALID_VALUE, NULL);
    }
    memcpy(to, &v, sizeof(int));
}

void fsti_to_unsigned(void *to, const struct fsti_variant *from,
                      struct fsti_agent *agent)
{
    unsigned v;
    switch(from->type) {
    case DBL: v = from->value.dbl; break;
    case LONG: v = from->value.longint; break;
    default: FSTI_ASSERT(0, FSTI_ERR_INVALID_VALUE, NULL);
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
    default: FSTI_ASSERT(0, FSTI_ERR_INVALID_VALUE, NULL);
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
    default: FSTI_ASSERT(0, FSTI_ERR_INVALID_VALUE, NULL);
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
    default: FSTI_ASSERT(0, FSTI_ERR_INVALID_VALUE, NULL);
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

static void make_partnerships_mutual(struct fsti_agent_arr *agent_arr)
{
    struct fsti_agent *a, *b;
    size_t i;
    FSTI_LOOP_AGENTS(*agent_arr, a, {
            for (i = 0; i < a->num_partners; i++) {
                b = fsti_agent_partner_at(agent_arr, a, i);
                FSTI_ASSERT(b->num_partners < FSTI_MAX_PARTNERS,
                            FSTI_ERR_OUT_OF_BOUNDS, NULL);
                b->partners[b->num_partners++] = a->id;
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

    filename = fsti_config_at0_str(&simulation->config, "AGENT_FILE");
    process_partners = fsti_config_at0_long(&simulation->config,
                                            "MUTUAL_CSV_PARTNERS");
    f = fopen(filename, "r");
    FSTI_ASSERT(f, FSTI_ERR_AGENT_FILE, filename);

    cs = csv_read(f, true, ',');
    FSTI_ASSERT(errno == 0, FSTI_ERR_AGENT_FILE, filename);

    fsti_agent_arr_init_n(&simulation->agent_arr, cs.len, NULL);
    for (i = 0; i < cs.len; ++i) {
        memset(simulation->csv->agent, 0, sizeof(struct fsti_agent));
        for (j = 0; j < cs.rows[i].len; ++j) {
            assert(j < simulation->csv->num_entries);
            process_cell(simulation->csv->agent,
                         cs.rows[i].cells[j],
                         simulation->csv->entries[j].dest,
                         simulation->csv->entries[j].transformer);
        }
        *(simulation->agent_arr.agents + i) = *simulation->csv->agent;
        (simulation->agent_arr.agents + i)->id = i;
    }
    if (process_partners)
        make_partnerships_mutual(&simulation->agent_arr);

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
                            &simulation->agent_arr);
        initialized_agents = true;
        g_mutex_unlock (&mutex);
    } else {
        g_mutex_unlock (&mutex);
        fsti_agent_arr_copy(&simulation->agent_arr,
                            &fsti_saved_agent_arr);
        fsti_agent_arr_add_dependency(&simulation->agent_arr,
                                      &simulation->mating_pool);
    }
}

void fsti_event_shuffle(struct fsti_simulation *simulation)
{
    if (simulation->agent_arr.len > 1) {
        for (size_t i = simulation->agent_arr.len - 1; i > 0; i--) {
            size_t j = (size_t) gsl_rng_uniform_int(simulation->rng, i + 1);
            size_t t = simulation->agent_arr.indices[j];
            simulation->agent_arr.indices[j] = simulation->agent_arr.indices[i];
            simulation->agent_arr.indices[i] = t;
        }
    }
}

void fsti_event_age(struct fsti_simulation *simulation)
{
    struct fsti_agent *a;
    FSTI_LOOP_SIMULATION_AGENTS(*simulation, a,
                                {
                                    a->age += simulation->time_step;
                                });
}

static void outputf(struct fsti_simulation *simulation, char *desc, double val)
{
    fprintf(simulation->results_file, "%s,%d,%d,%s,%f\n",
            simulation->name, simulation->sim_number,
            simulation->config_sim_number, desc, val);
}

static void outputl(struct fsti_simulation *simulation, char *desc, long val)
{
    fprintf(simulation->results_file, "%s,%d,%d,%s,%ld\n",
            simulation->name, simulation->sim_number,
            simulation->config_sim_number, desc, val);
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

    outputl(simulation, "POPULATION", simulation->agent_arr.len);

    FSTI_LOOP_SIMULATION_AGENTS(*simulation, agent, {
            age_avg += agent->age;
            infections += (bool) agent->infected;
            num_partners += agent->num_partners;
        });

    outputf(simulation, "AVERAGE_AGE", age_avg / simulation->agent_arr.len);
    outputf(simulation, "INFECTION_RATE",
           (double) infections / simulation->agent_arr.len);
    outputl(simulation, "NUM_PARTNERS", num_partners);
}

void fsti_event_write_agents_csv(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;
    FSTI_LOOP_SIMULATION_AGENTS(*simulation, agent, {
            fsti_agent_print_csv(simulation->agents_output_file,
                                 simulation->sim_number,
                                 agent);
        });
}

void fsti_event_write_agents_pretty(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;
    FSTI_LOOP_SIMULATION_AGENTS(*simulation, agent, {
            fsti_agent_print_pretty(simulation->agents_output_file,
                                    simulation->sim_number,
                                    agent);
        });
}

void fsti_event_mating_pool(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;
    fsti_agent_arr_clear(&simulation->mating_pool);
    FSTI_LOOP_SIMULATION_AGENTS(*simulation, agent, {
            if (agent->num_partners == 0) {
                double prob = gsl_rng_uniform(simulation->rng);
                if (prob < simulation->mating_pool_prob)
                    fsti_agent_arr_push_index(&simulation->mating_pool,
                                              agent->id);
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

    if (num_agents < 2) return; // Too few agents to bother
    if (num_agents % 2) --num_agents; // Can only match even number of agents

    begin = fsti_agent_arr_begin(&simulation->mating_pool);
    end = fsti_agent_arr_end(&simulation->mating_pool);

    for (it = begin; it < end - 1; it++) {
        agent = fsti_agent_arr_at(&simulation->mating_pool, it);
        if (FSTI_AGENT_HAS_PARTNER(agent)) continue;
        start = it + 1;
        n = (start + k) < end ? (start + k) : end;
        best_dist = FLT_MAX;
        partner = NULL;
        for (jt = start; jt < n; jt++) {
            candidate = fsti_agent_arr_at(&simulation->mating_pool, jt);
            if (FSTI_AGENT_HAS_PARTNER(candidate)) continue;
            d = FSTI_AGENT_DISTANCE(agent, candidate);
            if (d < best_dist) {
                best_dist = d;
                partner = candidate;
            }
        }
        if (partner) fsti_agent_make_partners(agent, partner);
    }
}


void fsti_event_stop(struct fsti_simulation *simulation)
{
    if (simulation->iteration >= simulation->num_iterations)
	simulation->stop = true;
    else
	++simulation->iteration;
}

void fsti_event_register_events()
{
    static bool initialized_events = false;

    if (initialized_events == false) {
        initialized_events = true;
        fsti_register_add("_READ_AGENTS", fsti_event_read_agents);
        fsti_register_add("_AGE", fsti_event_age);
        fsti_register_add("_SHUFFLE", fsti_event_shuffle);
        fsti_register_add("_MATING_POOL", fsti_event_mating_pool);
        fsti_register_add("_RKPM", fsti_event_knn_match);
        fsti_register_add("_REPORT", fsti_event_report);
        fsti_register_add("_WRITE_AGENTS_CSV", fsti_event_write_agents_csv);
        fsti_register_add("_WRITE_AGENTS_PRETTY", fsti_event_write_agents_pretty);
        fsti_register_add("_STOP", fsti_event_stop);
        FSTI_ADDITIONAL_EVENTS_REGISTER;
    }
}
