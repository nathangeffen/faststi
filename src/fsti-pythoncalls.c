#include "fsti.h"

#define NEW 0
#define REPLACE 1
#define NEW_REPLACE 2

void
print_variant(const struct fsti_variant *variant)
{
    switch(variant->type) {
    case STR:
        printf("%s", variant->value.str);
        break;
    case DBL:
        printf("%f", variant->value.dbl);
        break;
    case LONG:
        printf("%ld", variant->value.longint);
        break;
    default:
        printf("Unsupported variant type");
    }
}

struct fsti_variant*
fsti_py_config_get(struct fsti_simulation *simulation,
                   const char *key,
                   size_t index)
{
    struct fsti_config_entry *entry;
    entry = fsti_config_find(&simulation->config, key);
    if (entry == NULL) {
        fprintf(stderr, "Warning in file %s at line %d: "
                "Key %s not found.",
                __FILE__, __LINE__, key);
        return NULL;
    } else {
        if (index > entry->len) {
            fprintf(stderr, "Warning in file %s at line %d: "
                    "Key %s not found.",
                    __FILE__, __LINE__, key);
            return NULL;
        } else {
            return &entry->variants[index];
        }
    }
}

void
fsti_py_config_set(struct fsti_simulation *simulation,
                   const char *key,
                   size_t len,
                   const char *values,
                   int type,
                   const char *description)
{
    struct fsti_config_entry *entry;
    char desc[FSTI_DESC_LEN];

    if (description)
        strncpy(desc, description, FSTI_DESC_LEN);
    else
        strncpy(desc, "Description not set", FSTI_DESC_LEN);

    if (type == NEW) {
        entry = fsti_config_find(&simulation->config, key);
        if (entry) {
            fprintf(stderr, "Warning in file %s at line %d: "
                    "Key %s already exists.",
                    __FILE__, __LINE__, key);
            return;
        }
    } else if (type == REPLACE) {
        entry = fsti_config_find(&simulation->config, key);
        strncpy(desc, entry->description, FSTI_DESC_LEN);
        if (entry == NULL) {
            fprintf(stderr, "Warning in file %s at line %d: "
                    "Key %s not found.",
                    __FILE__, __LINE__, key);
            return;
        }
    }

    fsti_config_add(&simulation->config, key, desc, values);
}



struct fsti_simulation **
fsti_py_simulations_get(struct fsti_simset *simset)
{
    struct fsti_simulation **sim_arr = NULL, *simulation;
    size_t len = 2, n = 1;

    sim_arr = malloc(sizeof(simulation) * len);

    if (simset->group_ptr == NULL || *simset->group_ptr == NULL) return NULL;
    fsti_simset_set_keys(simset);

    while (*simset->group_ptr) {
        simulation = malloc(sizeof(*simulation));
        FSTI_ASSERT(simulation, FSTI_ERR_NOMEM, NULL);
        fsti_simset_setup_simulation(simset, simulation);

        if (n == len) {
            len *= 2;
            sim_arr = realloc(sim_arr, len * sizeof(simulation));
            FSTI_ASSERT(sim_arr, FSTI_ERR_NOMEM, NULL);
        }
        sim_arr[n++] = simulation;
        ++simset->config_sim_number;
        ++simset->sim_number;

        fsti_simset_update_config(simset);
    }

    sim_arr[0] = (void *) n - 1;
    return sim_arr;
}

uint32_t fsti_py_simulation_id(const struct fsti_simulation *simulation)
{
    return simulation->sim_number;
}

void
fsti_py_simulation_free(struct fsti_simulation *simulation)
{
    fsti_simulation_free(simulation);
    free(simulation);
}


struct fsti_simset * fsti_py_simset_init()
{
    struct fsti_simset *simset = malloc(sizeof(struct fsti_simset));
    FSTI_ASSERT(simset, FSTI_ERR_NOMEM, NULL);
    fsti_simset_init(simset);
    return simset;
}

static int SIMS_PER_THREAD;

struct thread_info {
    struct fsti_simulation **sim_arr;
    int lo;
    int hi;
};

static void *
thread_run(void *p)
{
    struct thread_info *t = p;
    int sims = t->hi - t->lo;
    if (sims < SIMS_PER_THREAD || sims == 1) {
        for (int i = t->lo; i < t->hi; i++) {
            if (t->sim_arr[i]) {
                fsti_simulation_run(t->sim_arr[i]);
                fsti_py_simulation_free(t->sim_arr[i]);
                t->sim_arr[i] = 0;
            }
        }
    } else {
        struct thread_info left;
        struct thread_info right;
        left.lo = t->lo;
        left.hi = right.lo = (t->hi + t->lo) / 2;
        left.sim_arr = right.sim_arr = t->sim_arr;
        right.hi = t->hi;
        GThread *thread = g_thread_new(NULL, thread_run, &left);
        thread_run(&right);
        g_thread_join(thread);
    }
    return NULL;
}


void
fsti_py_simulations_exec(int n,
                         struct fsti_simulation *sim_arr[])
{
    struct thread_info t;
    int sims_per_thread;
    int threads;
    t.lo = 0;
    t.hi = n;
    t.sim_arr = sim_arr;

    threads = (int) fsti_config_at0_long(&sim_arr[t.lo]->config, "threads");
    if (threads == 0)
        threads = g_get_num_processors();
    sims_per_thread = n / g_get_num_processors();
    SIMS_PER_THREAD = (sims_per_thread > 0) ? sims_per_thread : 1;
    thread_run(&t);
}

void
fsti_py_simulations_free(struct fsti_simulation *sim_arr[])
{
    size_t n = (size_t) sim_arr[0];
    for (size_t i = 1; i <= n; i++) {
        if (sim_arr[i]) {
            fsti_py_simulation_free(sim_arr[i]);
        }
    }
    free(sim_arr);
}


void
fsti_py_simset_free(struct fsti_simset *simset)
{
    fsti_simset_free(simset);
    free(simset);
}


void
fsti_py_test(struct test_group *tg,
             bool valgrind)
{
    const size_t num_agents = 101;
    struct fsti_simset *simset;
    struct fsti_simulation **simulations;
    FILE *agents_in_file;
    char *agents_in_filename = "fsti_test_agents_in_1234.csv";
    FILE *config_file;
    char *config_filename = "fsti_test_config_1234.csv";
    intptr_t len;
    char *config_text;
    struct fsti_variant *variant;

    // Sets config_text. Used in fsti-simset.c and fsti-pythoncalls.c
    fsti_simset_setup_config(&config_text, valgrind);

    // Write an agents file
    agents_in_file = fopen(agents_in_filename, "w");
    assert(agents_in_file);
    fprintf(agents_in_file, "id;age;infected;sex;sex_preferred;partners_0\n");
    for (size_t i = 0; i < num_agents; i++) {
        double age = (double) rand() / RAND_MAX * 30.0 + 20.0;
        int infected = ( ( (double) rand()/RAND_MAX)  < 0.3) ? 1 : 0;
        int sex = ( ( (double) rand()/RAND_MAX) <0.5)
            ? FSTI_MALE : FSTI_FEMALE;
        int sex_preferred = ( ( (double) rand()/RAND_MAX)  < 0.06)
            ? sex : !sex;
        long partner;
        if (i % 2 == 0) {
            partner = -1;
        } else {
            if ( ( (double) rand() / RAND_MAX) < 0.5)
                partner = i - 1;
            else
                partner = -1;
        }
        fprintf(agents_in_file, "%zu;%.2f;%d;%d;%d;%ld\n",
                i,
                age,
                infected,
                sex,
                sex_preferred,
                partner);
    }
    fclose(agents_in_file);

// Write config file
    config_file = fopen(config_filename, "w");
    assert(config_file);
    fprintf(config_file, "%s", config_text);
    fclose(config_file);

    fsti_events_tg = tg;

    simset = fsti_py_simset_init();
    fsti_simset_load_config_file(simset, config_filename);
    simulations = fsti_py_simulations_get(simset);

    len = (intptr_t) simulations[0];
    variant = fsti_py_config_get(simulations[1], "csv_delimiter", 0);
    TESTEQ(variant->type, STR, *tg);
    TESTEQ(strcmp(variant->value.str, ";"), 0, *tg);
    variant = fsti_py_config_get(simulations[1], "num_agents", 0);
    TESTEQ(variant->type, LONG, *tg);
    TESTEQ(variant->value.longint, 20000, *tg);

    fsti_py_simulations_exec(len , &simulations[1]);

    if (valgrind == false) {
        TESTEQ(simset->sim_number, 8, *tg);
        TESTEQ(len, 8, *tg);
    }
    fsti_py_simset_free(simset);
    free(simulations);

    fsti_remove_file(agents_in_filename);
    fsti_remove_file(config_filename);
    fsti_remove_file("fsti_test_agents_out_1234.csv");
    fsti_remove_file("fsti_test_results_1234.csv");
    fsti_remove_file("fsti_test_partnerships_1234.csv");

}
