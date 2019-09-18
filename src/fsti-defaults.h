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

#ifndef FSTI_DEFAULTS_H
#define FSTI_DEFAULTS_H
#include "fsti-config.h"


#include "fsti-defs.h"
#include "fsti-userdefs.h"

/*
   Add fields to the struct fsti_agent definition here. Note the semicolon at
   the end of the last field.
   E.g.

#define FSTI_AGENT_FIELDS                    \
float date_last_had_sex;                     \
unsigned char profession;                    \
unsigned char socioeconomic_bracket;         \
bool employed;

*/
#ifndef FSTI_AGENT_FIELDS
#define FSTI_AGENT_FIELDS
#endif

/*
   Add fields to the struct fsti_simulation definition here. Note the semicolon
   at the end of the last field.  E.g.

#define FSTI_SIMULATION_FIELDS                    \
bool track_agent_partners;                         \
unsigned total_partners;
*/
#ifndef FSTI_SIMULATION_FIELDS
#define FSTI_SIMULATION_FIELDS
#endif

/*
   Override the maximum number of partners an agent can have concurrently.
*/
#ifndef FSTI_MAX_PARTNERS
#define FSTI_MAX_PARTNERS 3
#endif

/*
   Override the number of infection risks (by default: MSM, MSW, WSM, WSW).
*/

#ifndef FSTI_INFECTION_RISKS
#define FSTI_INFECTION_RISKS 4
#endif


/* Add your own configuration variables here. */

#ifndef FSTI_ADDITIONAL_CONFIG_VARS
#define FSTI_ADDITIONAL_CONFIG_VARS(config)                             \
    FSTI_CONFIG_ADD(config, EXAMPLE_1, "Example configuration field", 0); \
    FSTI_CONFIG_ADD_STR(config, EXAMPLE_2,                              \
                        "Example configuration field", "String field")
#endif

#ifndef FSTI_AGENT_PRINT_CSV_HEADER
#define FSTI_AGENT_PRINT_CSV_HEADER(file_handle, delim)                 \
    fprintf(file_handle, "%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s\n", \
            "sim", delim,                                               \
            "date", delim,                                              \
            "id", delim,                                                \
            "age", delim,                                               \
            "sex", delim,                                               \
            "sex_preferred", delim,                                     \
            "risk", delim,                                              \
            "infected", delim,                                          \
            "treated", delim,                                           \
            "resistant", delim,                                         \
            "iter_death", delim,                                        \
            "partner", delim,                                           \
            "change_date")
#endif

/*
   This is the default code to print an agent in csv format. Change it to print another
   way.
*/

#ifndef FSTI_AGENT_PRINT_CSV
#define FSTI_AGENT_PRINT_CSV(simulation, agent, delim) do {             \
        char _current_date[FSTI_DATE_LEN];                              \
        char _death_date[FSTI_DATE_LEN];                                \
        char _relchange_date[FSTI_DATE_LEN];                            \
        fsti_time_add_sprint(simulation->start_date,                    \
                             simulation->iteration,                     \
                             simulation->time_step,                     \
                             _current_date);                            \
        fsti_time_add_sprint(simulation->start_date,                    \
                             agent->iter_death,                         \
                             simulation->time_step,                     \
                             _death_date);                              \
        fsti_time_add_sprint(simulation->start_date,                    \
                             agent->relchange[0],                       \
                             simulation->time_step,                     \
                             _relchange_date);                          \
        fprintf(simulation->agents_output_file,                         \
                "%u%c%s%c%u%c%u%c%u%c%u%c%u%c%u%c%u%c%u%c%s%c%ld%c%s\n",    \
                simulation->sim_number, delim,                          \
                _current_date, delim,                                   \
                agent->id, delim,                                       \
                fsti_time_in_years(agent->age), delim,                  \
                (unsigned) agent->sex, delim,                           \
                (unsigned) agent->sex_preferred, delim,                 \
                (unsigned) agent->risk, delim,                          \
                agent->infected, delim,                                 \
                (unsigned) agent->treated, delim,                       \
                (unsigned) agent->resistant, delim,                     \
                _death_date , delim,                                    \
                agent->num_partners ? (long) agent->partners[0] : -1,   \
                delim, _relchange_date);                                \
    } while(0)
#endif

/*
   Change this macro to pretty print an agent.
*/
#ifndef FSTI_AGENT_PRINT_PRETTY
#define FSTI_AGENT_PRINT_PRETTY(file_handle, id, agent)              \
    fprintf(file_handle, "Sim %u, Agent %u: Age: %u, %s %s %s %ld\n", \
            id,                                                         \
            agent->id,                                                  \
            fsti_time_in_years(agent->age),                             \
            (agent->sex == FSTI_MALE &&                                 \
             agent->sex_preferred == FSTI_FEMALE) ? "MSW" :             \
            ((agent->sex == FSTI_MALE &&                                \
              agent->sex_preferred == FSTI_MALE) ? "MSM" :              \
             ((agent->sex == FSTI_FEMALE &&                             \
               agent->sex_preferred == FSTI_MALE) ? "WSM" : "WSW")),    \
            agent->infected ? "Positive" : "Negative",                  \
            agent->num_partners ? "Relationship" : "Single",             \
            agent->num_partners ? (long) agent->partners[0] : -1)
#endif

/*
   Default report event output. Can be appended to with the
   FST_HOOK_REPORT hook below as well.
*/

#ifndef FSTI_REPORT
#define FSTI_REPORT do {                                           \
        FSTI_REPORT_OUTPUT_POST(FSTI_MIN, living, age, "MIN_AGE_ALIVE", \
                                FSTI_TIME_IN_YEARS);                    \
        FSTI_REPORT_OUTPUT_POST(FSTI_MAX, living, age, "MAX_AGE_ALIVE", \
                                FSTI_TIME_IN_YEARS);                    \
        FSTI_REPORT_OUTPUT_POST(FSTI_MEAN, living, age, "MEAN_AGE_ALIVE", \
                           FSTI_TIME_IN_YEARS);                         \
        FSTI_REPORT_OUTPUT_POST(FSTI_MEDIAN, living, age, "MEDIAN_AGE_ALIVE", \
                           FSTI_TIME_IN_YEARS);                         \
        FSTI_REPORT_OUTPUT(FSTI_MEAN_COUNT, living, infected,           \
                           "INFECT_RATE_ALIVE");                        \
        FSTI_REPORT_OUTPUT_PREC(FSTI_SIZE, living, , "POP_ALIVE", "%.0f"); \
        FSTI_REPORT_OUTPUT_POST_PREC(FSTI_SUM, living, num_partners,    \
                                     "NUM_PARTNERS", FSTI_HALF, "%.0f"); \
        FSTI_REPORT_OUTPUT_POST(FSTI_MIN, dead, age, "MIN_AGE_DEAD",    \
                           FSTI_TIME_IN_YEARS);                         \
        FSTI_REPORT_OUTPUT_POST(FSTI_MAX, dead, age, "MAX_AGE_DEAD",    \
                           FSTI_TIME_IN_YEARS);                         \
        FSTI_REPORT_OUTPUT_POST(FSTI_MEAN, dead, age, "MEAN_AGE_DEAD",  \
                           FSTI_TIME_IN_YEARS);                         \
        FSTI_REPORT_OUTPUT(FSTI_MEAN_COUNT, dead, infected,             \
                           "INFECT_RATE_DEAD");                         \
        FSTI_REPORT_OUTPUT_PREC(FSTI_SIZE, dead, , "POP_DEAD", "%.0f"); \
        FSTI_REPORT_OUTPUT_PREC(FSTI_SIM_CONST, , initial_infections,    \
                                "INITIAL_INFECTIONS", "%.0f");          \
        FSTI_REPORT_OUTPUT_PREC(FSTI_SIM_CONST, ,infections,            \
                                "SIMULATION_INFECTIONS", "%.0f");        \
        FSTI_REPORT_OUTPUT_PREC(FSTI_SIM_CONST, , initial_matches,      \
                                "INITIAL_MATCHES", "%.0f");              \
        FSTI_REPORT_OUTPUT_PREC(FSTI_SIM_CONST, , matches,              \
                                "SIMULATION_MATCHES", "%.0f");           \
        FSTI_REPORT_OUTPUT_PREC(FSTI_SIM_CONST, , breakups,             \
                                "BREAKUPS", "%.0f");                     \
        FSTI_REPORT_OUTPUT_PREC(FSTI_TIME_TAKEN, , , "TIME_TAKEN", "%.0f"); \
    } while(0)
#endif

/* Define additional struct fsti_agent elements here.
   End the list with a comma (,) else there will be compile errors.
   Note that adding additional elements will break the standard test run.
*/

#ifndef FSTI_AGENT_ADDITIONAL_ELEMS
#define FSTI_AGENT_ADDITIONAL_ELEMS
#endif

/* The members of the fsti_agent structure are defined here so that
   agent csv files and datasets can refer to them. In the default
   version, the names are entered alphabetically, but this is not essential.
   The system sorts them before using them the first time.
*/
#ifndef FSTI_AGENT_ELEM
#define FSTI_AGENT_ELEM {                                               \
        FSTI_AGENT_ADDITIONAL_ELEMS                                     \
        {"age", offsetof(struct fsti_agent, age), UINT, fsti_to_age},   \
        FSTI_AGENT_ELEM_ENTRY(birthday),                                \
        FSTI_AGENT_ELEM_ENTRY(cause_of_death),                          \
        FSTI_AGENT_ELEM_ENTRY(coinfected),                              \
        FSTI_AGENT_ELEM_ENTRY(id),                                      \
        FSTI_AGENT_ELEM_ENTRY(infected),                                \
        FSTI_AGENT_ELEM_ENTRY(iter_cured),                              \
        FSTI_AGENT_ELEM_ENTRY(iter_death),                              \
        FSTI_AGENT_ELEM_ENTRY(num_partners),                            \
        {"partners_0", offsetof(struct fsti_agent, partners),           \
         UINT, fsti_to_partner},                                        \
        {"partners_1", offsetof(struct fsti_agent, partners[1]),        \
         UINT, NULL},                                                   \
        {"partners_2", offsetof(struct fsti_agent, partners[2]),        \
         UINT, NULL},                                                   \
        {"partners_3", offsetof(struct fsti_agent, partners[3]),        \
         UINT, NULL},                                                   \
        {"relchange_0", offsetof(struct fsti_agent, relchange),         \
         UINT, fsti_to_uint32_t},                                       \
        {"relchange_1", offsetof(struct fsti_agent, relchange[1]),      \
         UINT, fsti_to_uint32_t},                                       \
        {"relchange_2", offsetof(struct fsti_agent, relchange[2]),      \
         UINT, fsti_to_uint32_t},                                       \
        {"relchange_3", offsetof(struct fsti_agent, relchange[3]),      \
         UINT, fsti_to_uint32_t},                                       \
        FSTI_AGENT_ELEM_ENTRY(resistant),                               \
        FSTI_AGENT_ELEM_ENTRY(risk),                                    \
        FSTI_AGENT_ELEM_ENTRY(sex),                                     \
        FSTI_AGENT_ELEM_ENTRY(sex_preferred),                           \
        FSTI_AGENT_ELEM_ENTRY(treated)                                  \
    }
#endif

/*
   Define this to call the function that returns true if  an agent
   has a partner, else false. The default function simply looks at the
   first entry in the partner array.
*/
#ifndef FSTI_AGENT_HAS_PARTNER
#define FSTI_AGENT_HAS_PARTNER(agent) fsti_agent_has_partner(agent)
#endif

/*
  Define this to call a distance function to measure suitability for two agents
  to match. The default version makes people with compatible sexual orientations
  and close ages more likely to match.
*/
#ifndef FSTI_AGENT_DISTANCE
#define FSTI_AGENT_DISTANCE(agent_a, agent_b)   \
    fsti_agent_default_distance(agent_a, agent_b)
#endif

/*
   Modify this to change the generation of new agents into the simulation.
   ("Birth" is not strictly speaking an accurate description, because
   in a simulation with a minimum age, of say 15, this would be the routine
   that generates new 15-year olds.)
 */

#ifndef FSTI_AGENT_GENERATE
#define FSTI_AGENT_GENERATE(simulation, agent)                          \
    do {                                                                \
        fsti_generate_age(simulation, agent);                           \
        fsti_generate_sex(simulation, agent);                           \
        fsti_generate_sex_preferred(simulation, agent);                 \
        fsti_generate_infected(simulation, agent);                      \
        fsti_generate_treated(simulation, agent);                       \
        fsti_generate_resistant(simulation, agent);                     \
        FSTI_HOOK_GENERATE_AGENT(simulation, agent)                     \
    } while(0)
#endif



/*
   Modify this to change the generation of new agents into the simulation.
   ("Birth" is not strictly speaking an accurate description, because
   in a simulation with a minimum age, of say 15, this would be the routine
   that generates new 15-year olds.)
 */

#ifndef FSTI_AGENT_BIRTH
#define FSTI_AGENT_BIRTH(simulation, agent)                             \
    do {                                                                \
        fsti_birth_age(simulation, agent);                              \
        fsti_birth_sex(simulation, agent);                              \
        fsti_birth_sex_preferred(simulation, agent);                    \
        fsti_birth_infected(simulation, agent);                         \
        fsti_birth_treated(simulation, agent);                          \
        fsti_birth_resistant(simulation, agent);                        \
        FSTI_HOOK_AGENT_BIRTH(simulation, agent)                        \
    } while(0)
#endif


/*
  Macros that are by default undefined.
*/

#ifdef FSTI_RECORD_INFECTIONS
#endif

#ifdef FSTI_RECORD_PARTNERS
#endif

#ifdef FSTI_RECORD_BREAKUPS
#endif

/*******************************************************************
 * Advanced macros: Please only override these if you have a
 * good understanding of both C and the FastSTI code.
 *******************************************************************/

/* Code for setting the iteration until which an agent remains single.
   The default function uses a Weibull function with user defined shape
   and scale.
*/

#ifndef FSTI_SET_SINGLE_PERIOD
#define FSTI_SET_SINGLE_PERIOD(simulation, agent) \
    fsti_set_single_period(simulation, agent)
#endif

/* Code for setting the iteration until which two paired agents stay together.
   The default function uses a Weibull function with user defined shape
   and scale.
*/

#ifndef FSTI_SET_REL_PERIOD
#define FSTI_SET_REL_PERIOD(simulation, agent_a, agent_b) do {  \
        fsti_set_rel_period(simulation, agent_a, agent_b);      \
    } while(0)
#endif


/* Hooks */

/*
   Hook for flex report event. Use this to add additional report lines.
*/

#ifndef FSTI_HOOK_REPORT
#define FSTI_HOOK_REPORT
#endif

/*
  Hook after copying an agent array.
*/

#ifndef FSTI_HOOK_AGENT_ARR_COPY
#define FSTI_HOOK_AGENT_ARR_COPY(dest_arr, src_arr) do {      \
    } while(0)
#endif

/*
  Hook before freeing an agent array.
*/
#ifndef FSTI_HOOK_AGENT_ARR_FREE
#define FSTI_HOOK_AGENT_ARR_FREE(agent_arr)
#endif

/*
  Hook after two agents have been made partners
*/
#ifndef FSTI_HOOK_AFTER_MAKE_PARTNERS
#define FSTI_HOOK_AFTER_MAKE_PARTNERS(a, b)
#endif

/*
  Hook after two agents have been made partners in a pair-matching event
*/
#ifndef FSTI_HOOK_AFTER_MATCH
#define FSTI_HOOK_AFTER_MATCH(simulation, a, b)
#endif


/*
  Hook before freeing a simulation
*/
#ifndef FSTI_HOOK_SIMULATION_FREE
#define FSTI_HOOK_SIMULATION_FREE(simulation)
#endif

/*
  Speed optimization hook. Use it to put frequently used config entries into
  simulation variables.
*/
#ifndef FSTI_HOOK_CONFIG_TO_VARS
#define FSTI_HOOK_CONFIG_TO_VARS(simulation)
#endif

/*
  Hook after an agent is created, but before being put in the agent array.
*/
#ifndef FSTI_HOOK_CREATE_AGENT
#define FSTI_HOOK_CREATE_AGENT(simulation, agent)
#endif

/*
  Hook after when an agent is generated at beginning of simulation.
*/
#ifndef FSTI_HOOK_GENERATE_AGENT
#define FSTI_HOOK_GENERATE_AGENT(simulation, agent)
#endif


/*
  Hook after when a birth takes place.
   ("Birth" is not strictly speaking an accurate description, because
   in a simulation with a minimum age, of say 15, this would be the routine
   that generates new 15-year olds.)
*/
#ifndef FSTI_HOOK_AGENT_BIRTH
#define FSTI_HOOK_AGENT_BIRTH(simulation, agent)
#endif

/*
  Hook just before an agent dies (partnerships still exist)
*/

#ifndef FSTI_HOOK_PRE_DEATH
#define FSTI_HOOK_PRE_DEATH(simulation, agent)
#endif

/*
  Hook after an agent dies (partnerships don't exist)
*/

#ifndef FSTI_HOOK_POST_DEATH
#define FSTI_HOOK_POST_DEATH(simulation, agent)
#endif


/*
  Hook to register events.
*/

#ifndef FSTI_HOOK_EVENTS_REGISTER
#define FSTI_HOOK_EVENTS_REGISTER
#endif

/* Prototypes */

int fsti_config_set_default(struct fsti_config *config);

#endif
