#include <glib.h>
#include <stddef.h>
#include "fsti-defs.h"
#include "fsti-events.h"

static void
process_cell(struct fsti_simulation *simulation, char *cell, void *addr,
             enum fsti_type type)
{
    struct fsti_variant variant;
    bool b;
    char c;
    unsigned char uc;
    int i;
    long l;
    float f;
    double d;
    unsigned u;
    variant = fsti_identify_token(cell);

    switch(type) {
    case BOOL:
        b = (bool) variant.value.longint;
        memcpy(addr, &b, sizeof(b));
        break;
    case CHAR:
        c = (char) variant.value.longint;
        memcpy(addr, &c, sizeof(c));
        break;
    case UCHAR:
        uc = (unsigned char) variant.value.longint;
        memcpy(addr, &uc, sizeof(uc));
        break;
    case INT:
        i = variant.value.longint;
        memcpy(addr, &i, sizeof(i));
        break;
    case UNSIGNED:
        u = variant.value.longint;
        memcpy(addr, &u, sizeof(u));
        break;
    case LONG:
        l = variant.value.longint;
        memcpy(addr, &l, sizeof(l));
        break;
    case FLOAT:
        f = (float) variant.value.dbl;
        DBG("FLOAT %f", f);
        memcpy(addr, &f, sizeof(f));
        break;
    case DBL:
        d = variant.value.dbl;
        DBG("DOUBLE %f", d);
        memcpy(addr, &d, sizeof(d));
        break;
    default:
        assert(0);
    }
}

static void read_agents(struct fsti_simulation *simulation)
{
    size_t i, j;
    FILE *f;
    struct csv cs;
    struct fsti_agent *agent;
    void *addr;
    const char *filename;

    filename = fsti_config_at0_str(&simulation->config, "AGENT_FILE");
    f = fopen(filename, "r");
    FSTI_ASSERT(filename, FSTI_ERR_AGENT_FILE, filename);

    cs = csv_read(f, true, ',');
    FSTI_ASSERT(errno == 0, FSTI_ERR_AGENT_FILE, filename);

    for (i = 0; i < cs.len; ++i) {
        agent = fsti_agent_arr_new_agent(&simulation->agent_arr);
        for (j = 0; j < cs.rows[i].len; ++j) {
            assert(j < simulation->num_csv_entries);
            addr = (void *) agent + simulation->csv_entries[j].offset;
            DBG("%zu: %p %p %zu %zu", j, agent, addr, offsetof(struct fsti_agent, age),
                (size_t) addr - (size_t) agent);
            process_cell(simulation, cs.rows[i].cells[j], addr,
                simulation->csv_entries[j].type);
        }
        DBG("%d %f", agent->id, agent->age);
    }
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

void fsti_event_stop(struct fsti_simulation *simulation)
{
    if (simulation->iteration >= simulation->num_iterations)
	simulation->stop = true;
    else
	++simulation->iteration;

}

void fsti_event_age(struct fsti_simulation *simulation)
{
    for (size_t i = 0; i < simulation->agent_arr.len; i++) {
        simulation->agent_arr.agents[i]->age += simulation->time_step;
    }
}

static void output(struct fsti_simulation *simulation, char *desc, double val)
{
    fprintf(simulation->report, "%s,%d,%d,%s,%f\n",
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
        DBG("%f", (*agent)->age);
        age_avg += (*agent)->age;
        infections += (bool) (*agent)->infected;
    }

    output(simulation, "AVERAGE_AGE", age_avg / simulation->agent_arr.len);
    output(simulation, "INFECTION_RATE",
           (double) infections / simulation->agent_arr.len);
}

void fsti_event_register_standard_events()
{
    static bool initialized_events = false;

    if (initialized_events == false) {
        initialized_events = true;
        fsti_register_add("_AGE", fsti_event_age);
        fsti_register_add("_STOP", fsti_event_stop);
        fsti_register_add("_REPORT", fsti_event_report);
        fsti_register_add("_READ_AGENTS", fsti_event_read_agents);
    }
}
