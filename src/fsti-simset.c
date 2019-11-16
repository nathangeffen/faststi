/*
  FastSTIi: large simulations of sexually transmitted infection epidemics.

  Copyright (C) 2019 Nathan Geffen

  FastSTI is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FastSTI is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <unistd.h>
#include <glib.h>
#include "fsti-simset.h"
#include "fsti-error.h"
#include "fsti-agent.h"
#include "fsti-defaults.h"
#include "fsti-procini.h"
#include "fsti-events.h"
#include "fsti-config.h"

static GMutex thread_mutex;
static GCond thread_cond;
static unsigned thread_no = 0;

static
void init(struct fsti_simset *simset)
{
    fsti_config_init(&simset->config);
    fsti_agent_arr_init(&fsti_saved_agent_arr);
    simset->config_strings = NULL;
    simset->more_configs = true;
    simset->config_num_simulations = 0;
    simset->config_sim_number = 0;
    simset->sim_number = 0;
    simset->groups = NULL;
    simset->group_ptr = NULL;
    simset->close_results_file = simset->close_agents_output_file =
        simset->close_partnerships_file = false;
    fsti_dataset_hash_init(&simset->dataset_hash);
    fsti_event_register_events();
    simset->key_file = g_key_file_new();
    FSTI_ASSERT(simset->key_file, FSTI_ERR_KEY_FILE_OPEN, "\n");
}

void
fsti_simset_init_with_config(struct fsti_simset *simset,
                             const struct fsti_config *config)
{
    init(simset);
    simset->more_configs = true;
    fsti_config_copy(&simset->config, config);
}

void
fsti_simset_init(struct fsti_simset *simset)
{
    init(simset);
    fsti_config_set_default(&simset->config);
}

void
fsti_simset_free(struct fsti_simset *simset)
{
    fsti_dataset_hash_free(&simset->dataset_hash);
    fsti_config_free(&simset->config);
    g_strfreev(simset->groups);
    g_key_file_free(simset->key_file);
    fsti_agent_arr_free(&fsti_saved_agent_arr);
    fsti_register_free();
}

void
fsti_simset_load_config_file(struct fsti_simset *simset,
                             const char *filename)
{
    fsti_read_ini(filename, simset->key_file);

    simset->groups = g_key_file_get_groups(simset->key_file, NULL);
    simset->group_ptr = simset->groups;
    simset->more_configs = true;
}

void
fsti_simset_load_config_strings(struct fsti_simset *simset,
                                char **config_strings)
{
    simset->config_strings = config_strings;
}

void
fsti_simset_set_keys(struct fsti_simset *simset)
{
    GError *error = NULL;
    char **keys = g_key_file_get_keys(simset->key_file, *simset->group_ptr,
                                      NULL, &error);
    FSTI_ASSERT(error == NULL, FSTI_ERR_KEY_FILE_SYNTAX,
                error->message);
    FSTI_ASSERT(keys && keys[0], FSTI_ERR_KEY_FILE_GROUP_EMPTY,
                FSTI_MSG("At group ", *simset->group_ptr));
    for (char **key = keys; *key != NULL; ++key) {
        char *values = g_key_file_get_value(simset->key_file,
                                            *simset->group_ptr,
                                            *key, &error);
        FSTI_ASSERT(error == NULL, FSTI_ERR_KEY_FILE_SYNTAX,
                    *simset->group_ptr);
        fsti_config_replace_values(&simset->config,
                                   *key, values);
        free(values);
    }
    g_strfreev(keys);
    simset->config_num_simulations = (size_t)
        fsti_config_at0_long(&simset->config,"num_simulations");
    FSTI_ASSERT(simset->config_num_simulations > 0,FSTI_ERR_INVALID_VALUE,NULL);
}

static
void set_output_files(struct fsti_simset *simset,
                      struct fsti_simulation *simulation)
{
    char *results_file_name,  *agents_file_name, *partnerships_file_name;

    if (simset->sim_number == 0) {
        results_file_name = fsti_config_at0_str(&simulation->config,
                                                "results_file");

        if(strcmp(results_file_name, "") && strcmp(results_file_name, "stdout")) {
            simset->results_file =
                simulation->results_file = fopen(results_file_name, "w");
            FSTI_ASSERT(simulation->results_file, FSTI_ERR_FILE, strerror(errno));
            simset->close_results_file = true;
        } else {
            simset->results_file = simulation->results_file = stdout;
        }

        agents_file_name = fsti_config_at0_str(&simulation->config,
                                               "agents_output_file");
        if(strcmp(agents_file_name, "") && strcmp(agents_file_name, "stdout")) {
            simset->agents_output_file =
                simulation->agents_output_file = fopen(agents_file_name, "w");
            FSTI_ASSERT(simulation->agents_output_file, FSTI_ERR_FILE,
                        strerror(errno));
            simset->close_agents_output_file = true;
        } else {
            simset->agents_output_file = simulation->agents_output_file = stdout;
        }

        partnerships_file_name = fsti_config_at0_str(&simulation->config,
                                                     "partnerships_file");
        if(strcmp(partnerships_file_name, "")) {
            simset->partnerships_file =
                simulation->partnerships_file = fopen(partnerships_file_name, "w");
            FSTI_ASSERT(simulation->partnerships_file, FSTI_ERR_FILE,
                        strerror(errno));
            simset->close_partnerships_file = true;
        } else {
            simset->partnerships_file = simulation->partnerships_file = stdout;
        }
    } else {
        simulation->results_file = simset->results_file;
        simulation->agents_output_file = simset->agents_output_file;
        simulation->partnerships_file = simset->partnerships_file;
    }

}

void fsti_simset_update_config(struct fsti_simset *simset)
{
    if (simset->config_sim_number == 0) {
        fsti_simset_set_keys(simset);
    } else if (simset->config_sim_number >= simset->config_num_simulations) {
        ++simset->group_ptr;
        simset->config_sim_number = 0;
        if (*simset->group_ptr) {
            fsti_simset_update_config(simset);
        }
    }
}

void fsti_simset_setup_simulation(struct fsti_simset *simset,
                                  struct fsti_simulation *simulation)
{
    fsti_simulation_init(simulation, &simset->config,
                         simset->sim_number,
                         simset->config_sim_number);
    set_output_files(simset, simulation);

    simulation->name = *simset->group_ptr;
    fsti_simulation_load_datasets(&simset->config, &simset->dataset_hash);
    fsti_dataset_hash_copy(&simulation->dataset_hash, &simset->dataset_hash);
}

static
void *threaded_sim(void *simulation)
{
    fsti_simulation_run(simulation);
    fsti_simulation_free(simulation);
    free(simulation);
    g_mutex_lock(&thread_mutex);
    thread_no--;
    g_cond_signal (&thread_cond);
    g_mutex_unlock(&thread_mutex);
    return NULL;
}

void
fsti_simset_exec(struct fsti_simset *simset)
{
    struct fsti_simulation *simulation;
    unsigned max_threads;
    GThread *thread;

    if (simset->group_ptr == NULL || *simset->group_ptr == NULL) return;
    fsti_simset_set_keys(simset);

    while (*simset->group_ptr) {
        max_threads = (unsigned) fsti_config_at0_long(&simset->config,"threads");
        if (max_threads == 0) max_threads = g_get_num_processors();

        simulation = malloc(sizeof(*simulation));
        FSTI_ASSERT(simulation, FSTI_ERR_NOMEM, NULL);
        fsti_simset_setup_simulation(simset, simulation);

        g_mutex_lock(&thread_mutex);
        while (thread_no >= max_threads)
            g_cond_wait (&thread_cond, &thread_mutex);
        thread = g_thread_new(NULL, threaded_sim, simulation);
        g_thread_unref(thread);
        thread_no++;
        g_mutex_unlock(&thread_mutex);

        ++simset->config_sim_number;
        ++simset->sim_number;
        fsti_simset_update_config(simset);
    }

    g_mutex_lock(&thread_mutex);
    while (thread_no > 0) g_cond_wait (&thread_cond, &thread_mutex);
    g_mutex_unlock(&thread_mutex);

    if (simset->close_results_file) fclose(simset->results_file);
    if (simset->close_agents_output_file) fclose(simset->agents_output_file);
    if (simset->close_partnerships_file) fclose(simset->partnerships_file);

}

void
fsti_simset_test(struct test_group *tg,
                 bool valgrind)
{
    // Identical code in fsti-pythoncalls.c so placed in common file
    #include "fsti-testcode.h"
}
