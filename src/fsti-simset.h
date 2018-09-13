#ifndef FSTI_SIMSET_H
#define FSTI_SIMSET_H

#include <stdbool.h>

#include "fsti-config.h"
#include "fsti-simulation.h"

struct fsti_simset {
    struct fsti_config config;
    GKeyFile *key_file;
    char **config_strings;
    bool more_configs;
    int sim_number;
    int config_num_sims;
    int config_sim_number;
    struct fsti_agent_csv_entry *csv_entries;
    size_t num_csv_entries;
    // A key file consists of groups of simulations
    char **groups;
    // We need a ptr to track stepping through groups
    char **group_ptr;
};

void fsti_simset_init_with_config(struct fsti_simset *simset,
                                  const struct fsti_config *config);

void fsti_simset_load_config_file(struct fsti_simset *simset,
                                  const char *filename);


void fsti_simset_load_config_strings(struct fsti_simset *simset,
				     char **config_strings);

void fsti_simset_set_csv(struct fsti_simset *simset,
                         const struct fsti_agent_csv_entry entries[],
                         size_t num_entries);

void fsti_simset_init(struct fsti_simset *simset);

void fsti_simset_exec(struct fsti_simset *simset);

void fsti_simset_free(struct fsti_simset *simset);


#endif
