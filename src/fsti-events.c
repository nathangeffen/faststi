#include <glib.h>
#include "fsti-defs.h"
#include "fsti-events.h"

static void
process_cell(struct fsti_simulation *simulation, char *cell, void *addr)
{
    struct fsti_variant variant;
    bool b;
    char c;
    int i;
    long l;
    double d;
    unsigned u;
    variant = fsti_identify_token(cell);

    switch(variant.type) {
    case BOOL:
        b = variant.value.longint;
        memcpy(addr, &b, sizeof(b));
        break;
    case CHAR:
        c = variant.value.longint;
        memcpy(addr, &c, sizeof(c));
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
    case DBL:
        d = variant.value.dbl;
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
            process_cell(simulation, cs.rows[i].cells[j], addr);
        }
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
        struct fsti_agent_default_data * data =
            (struct fsti_agent_default_data *)
            simulation->agent_arr.agents[i]->data;
	data->age += simulation->time_step;
    }
}


void fsti_event_report(struct fsti_simulation *simulation)
{
    if (simulation->iteration % simulation->report_frequency == 0)
	fprintf(simulation->report, "%s,%d,%d,%s,%f\n",
		simulation->name, simulation->sim_number,
		simulation->config_sim_number, "POPULATION_SIZE",
		(double) simulation->agent_arr.len);
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
