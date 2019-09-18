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
   Lookup matrix used to determine the probability that an agent gets tested.
*/
static const double prob_test[2][5] = {
    {
        PROB_TEST_MALE_STAGE_0,
        PROB_TEST_MALE_STAGE_1,
        PROB_TEST_MALE_STAGE_2,
        PROB_TEST_MALE_STAGE_3,
        PROB_TEST_MALE_STAGE_4
    },
    {
        PROB_TEST_FEMALE_STAGE_0,
        PROB_TEST_FEMALE_STAGE_1,
        PROB_TEST_FEMALE_STAGE_2,
        PROB_TEST_FEMALE_STAGE_3,
        PROB_TEST_FEMALE_STAGE_4
    }
};

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
        } else if (r < LO_STAGE_CHRONIC) {
            agent->infected = STAGE_CHRONIC;
        } else {
            agent->infected = STAGE_SICK;
        }
    } else {
        if (r < HI_STAGE_UNINFECTED) {
            agent->infected = STAGE_UNINFECTED;
        } else if (r < HI_STAGE_TREATED) {
            agent->infected = STAGE_TREATED;
        } else if (r < HI_STAGE_PRIMARY) {
            agent->infected = STAGE_PRIMARY;
        } else if (r < HI_STAGE_CHRONIC) {
            agent->infected = STAGE_CHRONIC;
        } else {
            agent->infected = STAGE_SICK;
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
    return (a->risk == b->risk && a->sex == b->sex) ? 0.0 : 1.0;
}

/*
   Places some of the single agents in the mating pool
 */
void
recents_event_mating_pool(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;
    double rnd;
    double risk;
    fsti_agent_ind_clear(&simulation->mating_pool);
    FSTI_FOR_LIVING(*simulation, agent, {
            if (agent->num_partners == 0) {
                rnd = gsl_rng_uniform(simulation->rng);
                if (agent->risk == LO)
                    risk = LO_SINGLE_TO_MATING;
                else
                    risk = HI_SINGLE_TO_MATING;
                if (rnd < risk)
                    fsti_agent_ind_push(&simulation->mating_pool, agent->id);
            }
        });
}

/*
  Breaks up some of the agent partnerships.
 */
void
recents_event_breakup(struct fsti_simulation *simulation)
{
    struct fsti_agent *a, *b;
    double rnd;
    double risk;
    fsti_agent_ind_clear(&simulation->mating_pool);
    FSTI_FOR_LIVING(*simulation, a, {
            if (a->num_partners > 0) {
                rnd = gsl_rng_uniform(simulation->rng);
                if (a->risk == LO)
                    risk = LO_MATING_TO_SINGLE;
                else
                    risk = HI_MATING_TO_SINGLE;
                if (rnd < risk) {
                    b = fsti_agent_partner_get0(&simulation->agent_arr, a);
                    fsti_agent_break_partners(a, b);
                    a->last_partner = b->id;
                    b->last_partner = a->id;
                    a->partner_breakup_iter = simulation->iteration;
                    b->partner_breakup_iter = a->partner_breakup_iter;
                    simulation->breakups++;
                    if (simulation->record_breakups)
                        fsti_output_partnership(simulation, a, b,
                                                FSTI_BREAKUP);
                }
            }
        });
}


/*
  Tests some of the agents for the disease. If the simulation->trace_partners
  parameter is set to true, it tries to find the previous partner and
  get him/her tested too.
*/
void
recents_event_get_tested(struct fsti_simulation *simulation)
{
    struct fsti_agent *a;
    double prob, rnd;
    struct fsti_agent *partner;

    FSTI_FOR_LIVING(*simulation, a, {
            prob = prob_test[a->sex][a->infected];
            rnd = gsl_rng_uniform(simulation->rng);
            if (rnd < prob) {
                if (a->infected > STAGE_TREATED)
                    a->infected = STAGE_TREATED;
            }
            if (simulation->trace_partners) {
                partner = fsti_agent_arr_at(&simulation->agent_arr,
                                            a->last_partner);
                if (partner && ( (int32_t) simulation->iteration -
                                a->partner_breakup_iter <
                                MAX_ITERATIONS_BACK) ) {
                    rnd = gsl_rng_uniform(simulation->rng);
                    if (rnd < PROB_TRACE_SUCCESS) {
                        if (partner->infected > STAGE_TREATED) {
                            partner->infected = STAGE_TREATED;
                            ++simulation->traces;
                        }
                    }
                }
            }
        });
}


/*
  Advances stage of infected agents.
 */

void
recents_event_advance(struct fsti_simulation *simulation)
{
    struct fsti_agent *agent;
    double r;

    FSTI_FOR_LIVING(*simulation, agent, {
            if (agent->infected == STAGE_PRIMARY) {
                r = gsl_rng_uniform(simulation->rng);
                if (r < PRIMARY_TO_CHRONIC)
                    agent->infected = STAGE_CHRONIC;
            } else if (agent->infected == STAGE_CHRONIC) {
                r = gsl_rng_uniform(simulation->rng);
                if (r < CHRONIC_TO_SICK)
                    agent->infected = STAGE_SICK;
            }
        });
}

/*
  This initializes the new variables added to struct fsti_simulation.
 */
void
recents_simulation_config_to_vars(struct fsti_simulation *simulation)
{
    simulation->traces = 0;
    simulation->trace_partners = fsti_config_at0_long(&simulation->config,
                                                     "trace_partners");
}

/*
  This registers the custom events.
 */
void
recents_events_register()
{
    fsti_register_add("mating_pool", recents_event_mating_pool);
    fsti_register_add("breakup", recents_event_breakup);
    fsti_register_add("get_tested", recents_event_get_tested);
    fsti_register_add("advance", recents_event_advance);
}
