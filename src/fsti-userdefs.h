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

/*
 * Users who wish to extend FastSTI code are encouraged to modify this file and
 * fsti-userdefs.c.
 * It will not modified in FastSTI upgrades.
 *
 */

#ifndef FSTI_USERDEFS // Don't remove these guards
#define FSTI_USERDEFS

#define LO 0
#define HI 1

#define STAGE_UNINFECTED 0
#define STAGE_TREATED 1
#define STAGE_PRIMARY 2
#define STAGE_CHRONIC 3

/* Parameters to change */

#define PROB_MALE 0.5
#define PROB_LO 0.5

#define HI_STAGE_UNINFECTED 0.6
#define HI_STAGE_TREATED 0.7
#define HI_STAGE_PRIMARY 0.71

#define LO_STAGE_UNINFECTED 0.99
#define LO_STAGE_TREATED 0.995
#define LO_STAGE_PRIMARY 0.996


#define FSTI_AGENT_GENERATE(simulation, agent) \
    recents_generate(simulation, agent)

#define FSTI_HOOK_EVENTS_REGISTER  recents_events_register()

#define FSTI_AGENT_DISTANCE(agent_a, agent_b) \
    recents_distance(agent_a, agent_b)

void
recents_generate(struct fsti_simulation *simulation,
                 struct fsti_agent *agent);

void
recents_events_register();

double
recents_distance(struct fsti_agent *a,
                 struct fsti_agent *b);

#endif
