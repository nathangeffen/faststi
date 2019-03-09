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
#include <stdio.h>

#include "utils/test.h"

#include "fsti-tests.h"
#include "fsti-dataset.h"
#include "fsti-config.h"
#include "fsti-agent.h"
#include "fsti-simset.h"

int fsti_tests(bool valgrind)
{
    struct test_group tg;

    test_group_new(&tg);

    fsti_test_defs(&tg);
    fsti_dataset_test(&tg);
    fsti_config_test(&tg);
    fsti_agent_test(&tg);
    fsti_simulation_test(&tg);
    fsti_simset_test(&tg, valgrind);

    test_group_summary(&tg);

    return tg.failures;
}
