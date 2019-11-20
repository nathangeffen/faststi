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

#ifndef FSTI_SIMSET_H
#define FSTI_SIMSET_H

#include <stdbool.h>
#include <glib.h>

#include "fsti-config.h"
#include "fsti-simulation.h"
#include "fsti-dataset.h"

struct fsti_simset {
    struct fsti_config config;
    GKeyFile *key_file;
    char **config_strings;
    bool more_configs;
    int sim_number;
    int config_num_simulations;
    int config_sim_number;
    // A key file consists of groups of simulations
    char **groups;
    // We need a ptr to track stepping through groups
    char **group_ptr;
    bool close_results_file;
    FILE *results_file;
    bool close_agents_output_file;
    FILE *agents_output_file;
    bool close_partnerships_file;
    FILE *partnerships_file;

    struct fsti_dataset_hash dataset_hash;
};

void fsti_simset_init_with_config(struct fsti_simset *simset,
                                  const struct fsti_config *config);
void fsti_simset_load_config_file(struct fsti_simset *simset,
                                  const char *filename);
void fsti_simset_load_config_strings(struct fsti_simset *simset,
				     char **config_strings);
void fsti_simset_init(struct fsti_simset *simset);
void fsti_simset_set_keys(struct fsti_simset *simset);
void fsti_simset_setup_simulation(struct fsti_simset *simset,
                                  struct fsti_simulation *simulation);
void fsti_simset_update_config(struct fsti_simset *simset);
void fsti_simset_exec(struct fsti_simset *simset);
void fsti_simset_free(struct fsti_simset *simset);
void fsti_simset_setup_config(char **config_text, int valgrind);
void fsti_simset_test(struct test_group *tg, bool valgrind);

#endif
