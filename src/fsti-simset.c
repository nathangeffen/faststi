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

static void init(struct fsti_simset *simset)
{
    fsti_config_init(&simset->config);
    fsti_agent_arr_init(&fsti_saved_agent_arr);
    simset->config_strings = NULL;
    simset->more_configs = true;
    simset->config_num_sims = 0;
    simset->config_sim_number = 0;
    simset->sim_number = 0;
    simset->groups = NULL;
    simset->group_ptr = NULL;
    simset->close_results_file = simset->close_agents_output_file = false;
    fsti_dataset_hash_init(&simset->dataset_hash);
    fsti_event_register_events();
    simset->key_file = g_key_file_new();
    FSTI_ASSERT(simset->key_file, FSTI_ERR_KEY_FILE_OPEN, "\n");
}

void fsti_simset_init_with_config(struct fsti_simset *simset,
                                  const struct fsti_config *config)
{
    init(simset);
    simset->more_configs = true;
    fsti_config_copy(&simset->config, config);
}

void fsti_simset_init(struct fsti_simset *simset)
{
    init(simset);
    fsti_config_set_default(&simset->config);
}

void fsti_simset_free(struct fsti_simset *simset)
{
    fsti_config_free(&simset->config);
    g_strfreev(simset->groups);
    g_key_file_free(simset->key_file);
    fsti_agent_arr_free(&fsti_saved_agent_arr);
    fsti_register_free();
}

void fsti_simset_load_config_file(struct fsti_simset *simset,
                                  const char *filename)
{
    fsti_read_ini(filename, simset->key_file);

    simset->groups = g_key_file_get_groups(simset->key_file, NULL);
    simset->group_ptr = simset->groups;
    simset->more_configs = true;
}

void fsti_simset_load_config_strings(struct fsti_simset *simset,
				     char **config_strings)
{
    simset->config_strings = config_strings;
}

static void set_keys(struct fsti_simset *simset)
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
    simset->config_num_sims = (size_t)
        fsti_config_at0_long(&simset->config,"NUM_SIMULATIONS");
    FSTI_ASSERT(simset->config_num_sims > 0, FSTI_ERR_INVALID_VALUE, NULL);
}

static void set_output_files(struct fsti_simset *simset,
                             struct fsti_simulation *simulation)
{
    char *results_file_name,  *agents_file_name;

    if (simset->sim_number == 0) {
        results_file_name = fsti_config_at0_str(&simulation->config,
                                                "RESULTS_FILE");

        if(strcmp(results_file_name, "")) {
            simset->results_file =
                simulation->results_file = fopen(results_file_name, "w");
            FSTI_ASSERT(simulation->results_file, FSTI_ERR_FILE, strerror(errno));
            simset->close_results_file = true;
        } else {
            simset->results_file = simulation->results_file = stdout;
        }

        agents_file_name = fsti_config_at0_str(&simulation->config,
                                               "AGENTS_OUTPUT_FILE");
        if(strcmp(agents_file_name, "")) {
            simset->agents_output_file =
                simulation->agents_output_file = fopen(agents_file_name, "w");
            FSTI_ASSERT(simulation->agents_output_file, FSTI_ERR_FILE,
                        strerror(errno));
            simset->close_agents_output_file = true;
        } else {
            simset->agents_output_file = simulation->agents_output_file = stdout;
        }
    } else {
        simulation->results_file = simset->results_file;
        simulation->agents_output_file = simset->agents_output_file;
    }
}

static void update_config(struct fsti_simset *simset)
{
    if (simset->config_sim_number == 0) {
        set_keys(simset);
    } else if (simset->config_sim_number >= simset->config_num_sims) {
        ++simset->group_ptr;
        simset->config_sim_number = 0;
        if (*simset->group_ptr) {
            update_config(simset);
        }
    }
}

static struct fsti_dataset_hash *load_datasets(struct fsti_simset *simset)
{
    struct fsti_config_entry *entry;
    size_t i;
    char delim;

    delim = fsti_config_at0_str(&simset->config, "CSV_DELIMITER")[0];
    for (i = 0; i < FSTI_HASHSIZE; i++) {
        entry = simset->config.entry[i];
        while (entry) {
            if (strncmp(entry->key, "DATASET_", sizeof("DATASET_")-1) == 0) {
                if (strcmp(entry->variants[0].value.str, FSTI_NO_OP)) {
                    if (entry->variants[0].type == STR)
                        fsti_dataset_hash_add(&simset->dataset_hash,
                                              entry->variants[0].value.str,
                                              delim);
                }
            }
            entry = entry->next;
        }
    }
    return &simset->dataset_hash;
}

static void setup_simulation(struct fsti_simset *simset,
                             struct fsti_simulation *simulation)
{
    fsti_simulation_init(simulation, &simset->config,
                         simset->sim_number,
                         simset->config_sim_number);
    set_output_files(simset, simulation);
    simulation->name = *simset->group_ptr;
    simulation->dataset_hash = load_datasets(simset);
}

static void *threaded_sim(void *simulation)
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

void fsti_simset_exec(struct fsti_simset *simset)
{
    struct fsti_simulation *simulation;
    unsigned max_threads;
    GThread *thread;

    if (simset->group_ptr == NULL || *simset->group_ptr == NULL) return;
    set_keys(simset);

    while (*simset->group_ptr) {
        max_threads = (unsigned) fsti_config_at0_long(&simset->config,"THREADS");
        if (max_threads == 0) max_threads = g_get_num_processors();

        simulation = malloc(sizeof(*simulation));
        FSTI_ASSERT(simulation, FSTI_ERR_NOMEM, NULL);
        setup_simulation(simset, simulation);

        g_mutex_lock(&thread_mutex);
        while (thread_no >= max_threads)
            g_cond_wait (&thread_cond, &thread_mutex);
        thread = g_thread_new(NULL, threaded_sim, simulation);
        g_thread_unref(thread);
        thread_no++;
        g_mutex_unlock(&thread_mutex);

        ++simset->config_sim_number;
        ++simset->sim_number;
        update_config(simset);
    }

    g_mutex_lock(&thread_mutex);
    while (thread_no > 0) g_cond_wait (&thread_cond, &thread_mutex);
    g_mutex_unlock(&thread_mutex);

    if (simset->close_results_file) fclose(simset->results_file);
    if (simset->close_agents_output_file) fclose(simset->agents_output_file);
}

void fsti_simset_test(struct test_group *tg)
{
    struct fsti_simset simset;
    FILE *agents_in_file;
    char *agents_in_filename = "fsti_test_agents_in_1234.csv";
    FILE *config_file;
    char *config_filename = "fsti_test_config_1234.csv";
    const size_t num_agents = 101;

    const char *config_text =
        "[Simulation_0]\n"
        "NUM_SIMULATIONS=1\n"
        "BEFORE_EVENTS=_READ_AGENTS;_WRITE_AGENTS_CSV_HEADER;"
        "_WRITE_RESULTS_CSV_HEADER\n"
        "DURING_EVENTS=_AGE\n"
        "AFTER_EVENTS=_FLEX_REPORT;_WRITE_AGENTS_CSV\n"
        "AGENTS_INPUT_FILE=fsti_test_agents_in_1234.csv\n"
        "AGENTS_OUTPUT_FILE=fsti_test_agents_out_1234.csv\n"
        "RESULTS_FILE=fsti_test_results_1234.csv\n"
        "THREADS=1\n"
        "[Simulation_1]\n"
        "THREADS=3\n"
        "NUM_SIMULATIONS=7\n"
        "DURING_EVENTS=_AGE;_MATING_POOL;_SHUFFLE_MATING;_RKPM\n"
        "MATCH_K=1\n"
        "[Simulation_2]\n"
        "MATCH_K=10\n"
        "[Simulation_3]\n"
        "MATCH_K=200\n";

    // Write an agents file
    agents_in_file = fopen(agents_in_filename, "w");
    assert(agents_in_file);
    fprintf(agents_in_file, "id;age;infected;sex;sex_preferred;partners_0\n");
    for (size_t i = 0; i < num_agents; i++) {
        double age = (double) rand() / RAND_MAX * 30.0 + 20.0;
        int infected = ( ( (double) rand()/RAND_MAX)  < 0.3) ? 1 : 0;
        int sex = ( ( (double) rand()/RAND_MAX) <0.5) ? FSTI_MALE : FSTI_FEMALE;
        int sex_preferred = ( ( (double) rand()/RAND_MAX)  < 0.06) ? sex : !sex;
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

    fsti_simset_init(&simset);
    fsti_simset_load_config_file(&simset, config_filename);
    fsti_simset_exec(&simset);
    TESTEQ(simset.sim_number, 22, *tg);

    fsti_simset_free(&simset);
    remove(agents_in_filename);
    remove(config_filename);
    remove("fsti_test_agents_out_1234.csv");
    remove("fsti_test_results_1234.csv");
}
