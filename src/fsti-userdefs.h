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

#ifndef FSTI_RECORD_INFECTIONS
#define FSTI_RECORD_INFECTIONS
#endif


/* LO = low-risk agent. HI = high-risk agent */

#define LO 0
#define HI 1

/* We have 5 infection stages */

#define STAGE_UNINFECTED 0
#define STAGE_TREATED 1
#define STAGE_PRIMARY 2
#define STAGE_CHRONIC 3
#define STAGE_SICK 4

/* Parameters to change */

#define PROB_MALE 0.5
#define PROB_LO 0.5

/* Infection probabilities when agents are generated */
#define LO_STAGE_UNINFECTED 0.99
#define LO_STAGE_TREATED 0.995
#define LO_STAGE_PRIMARY 0.996
#define LO_STAGE_CHRONIC 0.999

#define HI_STAGE_UNINFECTED 0.6
#define HI_STAGE_TREATED 0.7
#define HI_STAGE_PRIMARY 0.71
#define HI_STAGE_CHRONIC 0.95


/* Probability of single agent entering mating pool (per day) */
#define HI_SINGLE_TO_MATING 0.9
#define LO_SINGLE_TO_MATING 0.01

/* Probability of partnered relationship becoming single (per day) */
#define HI_MATING_TO_SINGLE 0.9
#define LO_MATING_TO_SINGLE 0.01

/* Infection risks for discordant partners (per day) */
#define RISK_INFECTION_MALE_TO_FEMALE 0.1
#define RISK_INFECTION_FEMALE_TO_MALE 0.05


/* Probabilities of getting tested (per day) */
#define PROB_TEST_MALE_STAGE_0 0.001
#define PROB_TEST_MALE_STAGE_1 0.0
#define PROB_TEST_MALE_STAGE_2 0.01
#define PROB_TEST_MALE_STAGE_3 0.001
#define PROB_TEST_MALE_STAGE_4 0.01

#define PROB_TEST_FEMALE_STAGE_0 0.002
#define PROB_TEST_FEMALE_STAGE_1 0.0
#define PROB_TEST_FEMALE_STAGE_2 0.02
#define PROB_TEST_FEMALE_STAGE_3 0.002
#define PROB_TEST_FEMALE_STAGE_4 0.02

/* Probability of last partner being traced in testing event */
#define PROB_TRACE_SUCCESS 0.9

/* Probability of advancing stages (per day) */
#define PRIMARY_TO_CHRONIC 0.08
#define CHRONIC_TO_SICK 0.0004

/* Maximum number of iterations back to consider previous partner (in days).
   In other words if agent being tested broke up with previous partner
   more than or equal this number of days ago, don't trace.
*/
#define MAX_ITERATIONS_BACK 100

/* Undefine fsti's default setting of single period */
#define FSTI_SET_SINGLE_PERIOD(simulation, agent)

/* Undefine fsti's setting of relationship period */
#define FSTI_SET_REL_PERIOD(simulation, agent_a, agent_b)


/* Code to generate an agent */
#define FSTI_AGENT_GENERATE(simulation, agent) \
    recents_generate(simulation, agent)

/* Define hook to register new events */
#define FSTI_HOOK_EVENTS_REGISTER  recents_events_register()

/* Customised distance function */
#define FSTI_AGENT_DISTANCE(agent_a, agent_b)   \
    recents_distance(agent_a, agent_b)

/* New agent fields. We need to record the last partner
   an agent had and the number of days since breakup.
*/
#define FSTI_AGENT_FIELDS                       \
    uint32_t last_partner;                      \
    int32_t partner_breakup_iter;

/* Put new fields into struct fsti_simulation */
#define FSTI_SIMULATION_FIELDS                  \
    bool trace_partners;                        \
    uint32_t traces;

/* Define additional user configurable parameters */
#define FSTI_ADDITIONAL_CONFIG_VARS(config)                     \
    FSTI_CONFIG_ADD(config, trace_partners,                     \
                    "Trace partners of infected agents", 0)

/* Call function to initialize new struct fsti_simulation fields */

#define FSTI_HOOK_CONFIG_TO_VARS(simulation)            \
    recents_simulation_config_to_vars(simulation)

/* Print the number of traced partners in the report event   */
#define FSTI_HOOK_REPORT                                                \
    FSTI_REPORT_OUTPUT_PREC(FSTI_SIM_CONST, , traces,                   \
                            "TRACES", "%.0f")

/* Prototypes */

void recents_generate(struct fsti_simulation *simulation,
                      struct fsti_agent *agent);
void recents_events_register();
double recents_distance(struct fsti_agent *a, struct fsti_agent *b);
void recents_simulation_config_to_vars(struct fsti_simulation *simulation);

#endif
