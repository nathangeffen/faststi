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

#ifndef FSTI_PYTHONCALLS_H
#define FSTI_PYTHONCALLS_H

#include "utils/test.h"

struct fsti_variant* fsti_py_config_get(struct fsti_simulation *simulation,
                                        const char *key, size_t index);
struct fsti_simulation ** fsti_py_simulations_get(struct fsti_simset *simset);
uint32_t fsti_py_simulation_id(const struct fsti_simulation *simulation);
void fsti_py_simulations_exec(int n, struct fsti_simulation *sim_arr[]);
void fsti_py_simulations_free(struct fsti_simulation *sim_arr[]);
void fsti_py_test(struct test_group *tg, bool valgrind);
#endif
