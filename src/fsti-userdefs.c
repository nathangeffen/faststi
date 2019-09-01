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
 * fsti-userdefs.h.
 * It will not be modified in FastSTI upgrades.
 *
 */

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "fsti.h"

/*
  Generates the properties of a new agent
 */

void
recents_generate(struct fsti_simulation *simulation,
                 struct fsti_agent *agent)
{
    agent->sex = (gsl_rng_uniform(simulation->rng) < PROB_MALE)
        ? FSTI_MALE : FSTI_FEMALE;
    agent->risk = (gsl_rng_uniform(simulation->rng) < PROB_LO) ?
        LO : HI;
    double r = gsl_rng_uniform(simulation->rng);
    if (agent->risk == LO) {
        if (r < LO_STAGE_UNINFECTED) {
            agent->infected = STAGE_UNINFECTED;
        } else if (r < LO_STAGE_TREATED) {
            agent->infected = STAGE_TREATED;
        } else if (r < LO_STAGE_PRIMARY) {
            agent->infected = STAGE_PRIMARY;
        } else {
            agent->infected = STAGE_CHRONIC;
        }
    } else {
        if (r < HI_STAGE_UNINFECTED) {
            agent->infected = STAGE_UNINFECTED;
        } else if (r < HI_STAGE_TREATED) {
            agent->infected = STAGE_TREATED;
        } else if (r < HI_STAGE_PRIMARY) {
            agent->infected = STAGE_PRIMARY;
        } else {
            agent->infected = STAGE_CHRONIC;
        }
    }
}

/*
  Calculates the suitability of two agents for matching
 */

double
recents_distance(struct fsti_agent *a,
                 struct fsti_agent *b)
{
    if (a->risk == b->risk && a->sex == b->sex)
        return 0.0;
    else
        return 1.0;
}


void recents_events_register()
{
    //
}
