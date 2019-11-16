#include "fsti.h"

struct fsti_simset * fsti_py_simset_init()
{
    struct fsti_simset *simset = malloc(sizeof(struct fsti_simset));
    FSTI_ASSERT(simset, FSTI_ERR_NOMEM, NULL);
    fsti_simset_init(simset);
    return simset;
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

void
fsti_py_simulation_free(struct fsti_simulation *simulation)
{
    fsti_simulation_free(simulation);
    free(simulation);
}

void
fsti_py_simulations_exec(int n, struct fsti_simulation *sim_arr[])
{
    for (int i = 0; i < n; i++) {
        fsti_simulation_run(sim_arr[i]);
        fsti_py_simulation_free(sim_arr[i]);
        sim_arr[i] = 0;
    }
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
    #include "fsti-testcode.h"
}
