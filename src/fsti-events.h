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

#ifndef FSTI_EVENTS_H
#define FSTI_EVENTS_H

#include <stdint.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#include "fsti-simulation.h"
#include "fsti-defs.h"
#include "fsti-report.h"
#include "fsti-agent.h"
#include "fsti-userdefs.h"

void fsti_event_read_agents(struct fsti_simulation *simulation);
void fsti_event_create_agents(struct fsti_simulation *simulation);
void fsti_event_age(struct fsti_simulation *simulation);
void fsti_event_initial_mating_pool(struct fsti_simulation *simulation);
void fsti_event_mating_pool(struct fsti_simulation *simulation);
void fsti_event_report(struct fsti_simulation *simulation);
void fsti_event_shuffle_living(struct fsti_simulation *simulation);
void fsti_event_shuffle_mating(struct fsti_simulation *simulation);
void fsti_event_write_results_csv_header(struct fsti_simulation *simulation);
void fsti_event_write_agents_csv_header(struct fsti_simulation *simulation);
void fsti_event_write_agents_csv(struct fsti_simulation *simulation);
void fsti_event_write_living_agents_csv(struct fsti_simulation *simulation);
void fsti_event_write_dead_agents_csv(struct fsti_simulation *simulation);
void fsti_event_write_agents_pretty(struct fsti_simulation *simulation);
void fsti_event_initial_mating_pool(struct fsti_simulation *simulation);
void fsti_event_initial_relchange(struct fsti_simulation *simulation);

void fsti_generate_age(struct fsti_simulation *simulation,
                    struct fsti_agent *a);
void fsti_generate_sex(struct fsti_simulation *simulation,
                    struct fsti_agent *a);
void fsti_generate_sex_preferred(struct fsti_simulation *simulation,
                              struct fsti_agent *a);
void fsti_generate_infected(struct fsti_simulation *simulation,
                            struct fsti_agent *a);
void fsti_generate_treated(struct fsti_simulation *simulation,
                           struct fsti_agent *a);
void fsti_generate_resistant(struct fsti_simulation *simulation,
                             struct fsti_agent *a);


void fsti_birth_age(struct fsti_simulation *simulation,
                    struct fsti_agent *agent);
void fsti_birth_sex(struct fsti_simulation *simulation,
                    struct fsti_agent *agent);
void fsti_birth_sex_preferred(struct fsti_simulation *simulation,
                              struct fsti_agent *agent);
void fsti_birth_infected(struct fsti_simulation *simulation,
                         struct fsti_agent *agent);
void fsti_birth_treated(struct fsti_simulation * simulation,
                        struct fsti_agent *agent);
void fsti_birth_resistant(struct fsti_simulation * simulation,
                          struct fsti_agent *agent);

void fsti_set_single_period(struct fsti_simulation *simulation,
                            struct fsti_agent *agent);
void fsti_set_rel_period(struct fsti_simulation *simulation,
                         struct fsti_agent *agent_a,
                         struct fsti_agent *agent_b);

void fsti_event_birth(struct fsti_simulation *simulation);
void fsti_event_death(struct fsti_simulation *simulation);
void fsti_event_breakup(struct fsti_simulation *simulation);
void fsti_event_mating_pool(struct fsti_simulation *simulation);
void fsti_event_knn_match(struct fsti_simulation *simulation);
void fsti_event_pair(struct fsti_simulation *simulation);
void fsti_event_generate_and_pair(struct fsti_simulation *simulation);
void fsti_event_infect(struct fsti_simulation *simulation);
void fsti_event_no_op(struct fsti_simulation *simulation);
void fsti_event_register_events();

extern struct fsti_agent fsti_global_agent;
extern struct test_group *fsti_events_tg;

#endif
