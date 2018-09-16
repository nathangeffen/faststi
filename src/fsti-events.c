#include <glib.h>
#include <stddef.h>
#include "fsti-defs.h"
#include "fsti-events.h"

void fsti_to_float(void *to, const struct fsti_variant *from,
                   struct fsti_simulation *simulation)
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
                   struct fsti_simulation *simulation)
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
                  struct fsti_simulation *simulation)
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
                      struct fsti_simulation *simulation)
{
    unsigned v;
    switch(from->type) {
    case DBL: v = from->value.dbl; break;
    case LONG: v = from->value.longint; break;
    default: FSTI_ASSERT(0, FSTI_ERR_INVALID_VALUE, NULL);
    }
    memcpy(to, &v, sizeof(unsigned));
}

void fsti_to_uchar(void *to, const struct fsti_variant *from,
                   struct fsti_simulation *simulation)
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
                     struct fsti_simulation *simulation)
{
    unsigned id;
    struct fsti_agent **agent, **first, **last;

    switch(from->type) {
    case DBL: id = (unsigned) from->value.dbl; break;
    case LONG: id = (unsigned) from->value.longint; break;
    default: FSTI_ASSERT(0, FSTI_ERR_INVALID_VALUE, NULL);
    }

    memset(to, 0, sizeof(struct fsti_agent *));
    if (id && simulation->agent_arr.len) {
        first = simulation->agent_arr.agents;
        last = simulation->agent_arr.agents + simulation->agent_arr.len - 1;

        for (agent = last; agent >= first; --agent) {
            if ( (*agent)->id == id) {
                memcpy(to, agent, sizeof(struct fsti_agent *));
                return;
            }
        }
    }
}

static void
process_cell(struct fsti_simulation *simulation, char *cell, void *to,
             fsti_transform_func transformer)
{
    struct fsti_variant variant;

    variant = fsti_identify_token(cell);
    transformer(to, &variant, simulation);
}

static void make_partnerships_mutual(struct fsti_agent_arr *arr)
{
    struct fsti_agent **agent;
    for(agent = arr->agents; agent != arr->agents + arr->len; ++agent)
        if ((*agent)->partners[0])
            (*agent)->partners[0]->partners[0] = *agent;
}

static void read_agents(struct fsti_simulation *simulation)
{
    size_t i, j;
    bool process_partners;
    FILE *f;
    struct csv cs;
    struct fsti_agent *agent;
    const char *filename;

    filename = fsti_config_at0_str(&simulation->config, "AGENT_FILE");
    process_partners = fsti_config_at0_long(&simulation->config,
                                            "MUTUAL_CSV_PARTNERS");
    f = fopen(filename, "r");
    FSTI_ASSERT(f, FSTI_ERR_AGENT_FILE, filename);

    cs = csv_read(f, true, ',');
    FSTI_ASSERT(errno == 0, FSTI_ERR_AGENT_FILE, filename);

    for (i = 0; i < cs.len; ++i) {
        for (j = 0; j < cs.rows[i].len; ++j) {
            assert(j < simulation->csv->num_entries);
            process_cell(simulation, cs.rows[i].cells[j],
                         simulation->csv->entries[j].dest,
                         simulation->csv->entries[j].transformer);
        }
        agent = fsti_agent_arr_new_agent(&simulation->agent_arr);
        *agent = *simulation->csv->agent;
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
        fsti_agent_arr_deep_copy(&fsti_saved_agent_arr,
                                 &simulation->agent_arr);
        initialized_agents = true;
        g_mutex_unlock (&mutex);
    } else {
        g_mutex_unlock (&mutex);
        fsti_agent_arr_deep_copy(&simulation->agent_arr,
                                 &fsti_saved_agent_arr);
    }
}

void fsti_event_shuffle(struct fsti_simulation *simulation)
{
    if (simulation->agent_arr.len > 1) {
        for (size_t i = simulation->agent_arr.len - 1; i > 0; i--) {
            size_t j = (size_t) gsl_rng_uniform_int(simulation->rng, i + 1);
            struct fsti_agent *t = simulation->agent_arr.agents[j];
            simulation->agent_arr.agents[j] = simulation->agent_arr.agents[i];
            simulation->agent_arr.agents[i] = t;
        }
    }
}

void fsti_event_age(struct fsti_simulation *simulation)
{
    for (size_t i = 0; i < simulation->agent_arr.len; i++) {
        simulation->agent_arr.agents[i]->age += simulation->time_step;
    }
}

static void output(struct fsti_simulation *simulation, char *desc, double val)
{
    fprintf(simulation->results_file, "%s,%d,%d,%s,%f\n",
            simulation->name, simulation->sim_number,
            simulation->config_sim_number, desc, val);
}

void fsti_event_report(struct fsti_simulation *simulation)
{
    struct fsti_agent **agent;
    double age_avg = 0.0;
    unsigned infections = 0;

    if (simulation->iteration % simulation->report_frequency == 0)
        output(simulation, "POPULATION",(double) simulation->agent_arr.len);

    for (agent = simulation->agent_arr.agents;
         agent < (simulation->agent_arr.agents + simulation->agent_arr.len);
         agent++) {
        age_avg += (*agent)->age;
        infections += (bool) (*agent)->infected;
    }

    output(simulation, "AVERAGE_AGE", age_avg / simulation->agent_arr.len);
    output(simulation, "INFECTION_RATE",
           (double) infections / simulation->agent_arr.len);
}

void fsti_event_write_agents_csv(struct fsti_simulation *simulation)
{
    struct fsti_agent **agent;
    for(agent = simulation->agent_arr.agents;
        agent != simulation->agent_arr.agents + simulation->agent_arr.len;
        ++agent)
        fsti_agent_print_csv(simulation->agents_output_file,
                             simulation->sim_number,
                             *agent);
}

void fsti_event_write_agents_pretty(struct fsti_simulation *simulation)
{
    struct fsti_agent **agent;
    for(agent = simulation->agent_arr.agents;
        agent != simulation->agent_arr.agents + simulation->agent_arr.len;
        ++agent)
        fsti_agent_print_pretty(simulation->agents_output_file,
                                simulation->sim_number,
                                *agent);
}

void fsti_event_stop(struct fsti_simulation *simulation)
{
    if (simulation->iteration >= simulation->num_iterations)
	simulation->stop = true;
    else
	++simulation->iteration;
}

void fsti_event_register_standard_events()
{
    static bool initialized_events = false;

    if (initialized_events == false) {
        initialized_events = true;
        fsti_register_add("_READ_AGENTS", fsti_event_read_agents);
        fsti_register_add("_AGE", fsti_event_age);
        fsti_register_add("_SHUFFLE", fsti_event_shuffle);
        fsti_register_add("_REPORT", fsti_event_report);
        fsti_register_add("_WRITE_AGENTS_CSV", fsti_event_write_agents_csv);
        fsti_register_add("_WRITE_AGENTS_PRETTY", fsti_event_write_agents_pretty);
        fsti_register_add("_STOP", fsti_event_stop);
    }
}
