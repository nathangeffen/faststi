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
    FSTI_CONFIG_ADD(config, "EXAMPLE_1", "Example configuration field", 0); \
    FSTI_CONFIG_ADD(config, "EXAMPLE_2",                                \
                    "Example configuration field", "String field")
#endif

#ifndef FSTI_AGENT_PRINT_CSV_HEADER
#define FSTI_AGENT_PRINT_CSV_HEADER(file_handle, delim)                 \
    fprintf(file_handle, "%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s\n",    \
            "sim", delim,                                               \
            "date", delim,                                              \
            "id", delim,                                                \
            "age", delim,                                               \
            "infected", delim,                                          \
            "sex", delim,                                               \
            "sex_preferred", delim,                                     \
            "date_death", delim,                                        \
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
        fsti_time_sprint(&agent->date_death, _death_date);              \
        fsti_time_add_sprint(simulation->start_date,                    \
                             agent->relchange[0],                       \
                             simulation->time_step,                     \
                             _relchange_date);                          \
        fprintf(simulation->agents_output_file,                         \
                "%u%c%s%c%u%c%u%c%u%c%u%c%u%c%s%c%ld%c%s\n",            \
                simulation->sim_number, delim,                          \
                _current_date, delim,                                   \
                agent->id, delim,                                       \
                fsti_time_in_years(agent->age), delim,                  \
                agent->infected, delim,                                 \
                (unsigned) agent->sex, delim,                           \
                (unsigned) agent->sex_preferred, delim,                 \
                _death_date , delim,             \
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
   Default flex report event output. Can be appended to with the
   FST_HOOK_FLEX_REPORT hook below as well.
*/

#ifndef FSTI_FLEX_REPORT
#define FSTI_FLEX_REPORT do {                                           \
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
    } while(0)
#endif

/* If any members of the agent struct need to be associated with a string
   they are defined here in alphabetical order. (Alphabetical order is
   essential because a binary search used to locate the entry.)
   Unfortunately because it's in alphabetical order, if you want to append
   to the definition below, you need to copy and paste the entire
   definition into the fsti-userdefs.h file and insert your appended variable
   names in the correct place alphabetically.
*/
#ifndef FSTI_AGENT_ELEM
#define FSTI_AGENT_ELEM {                                               \
        {"age", offsetof(struct fsti_agent, age), UINT, fsti_to_age},   \
        FSTI_AGENT_ELEM_ENTRY(age_group),                               \
        FSTI_AGENT_ELEM_ENTRY(birth_date),                              \
        FSTI_AGENT_ELEM_ENTRY(birthday),                                \
        FSTI_AGENT_ELEM_ENTRY(cause_of_death),                          \
        FSTI_AGENT_ELEM_ENTRY(coinfected),                              \
        FSTI_AGENT_ELEM_ENTRY(cured),                                   \
        FSTI_AGENT_ELEM_ENTRY(date_death),                              \
        FSTI_AGENT_ELEM_ENTRY(id),                                      \
        FSTI_AGENT_ELEM_ENTRY(infected),                                \
        FSTI_AGENT_ELEM_ENTRY(num_partners),                            \
        {"partners_0", offsetof(struct fsti_agent, partners),           \
         UINT, fsti_to_partner},                                  \
        {"partners_1", offsetof(struct fsti_agent, partners[1]),        \
         UINT, NULL},                                             \
        {"partners_2", offsetof(struct fsti_agent, partners[2]),        \
         UINT, NULL},                                             \
        {"partners_3", offsetof(struct fsti_agent, partners[3]),        \
         UINT, NULL},                                             \
        FSTI_AGENT_ELEM_ENTRY(sex),                                     \
        FSTI_AGENT_ELEM_ENTRY(sex_preferred)                            \
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
        fsti_set_agent_age(simulation, agent);                          \
        fsti_set_agent_sex(simulation, agent);                          \
        fsti_set_agent_sex_preferred(simulation, agent);                \
        fsti_set_agent_infected(simulation, agent);                     \
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
        fsti_set_birth_age(simulation, agent);                          \
        fsti_set_birth_sex(simulation, agent);                          \
        fsti_set_birth_sex_preferred(simulation, agent);                \
        fsti_set_birth_infected(simulation, agent);                     \
        FSTI_HOOK_AGENT_BIRTH(simulation, agent)                        \
    } while(0)
#endif


/*******************************************************************
 * Advanced macros: Please only override these if you have a
 * good understanding of both C and the FastSTI code.
 *******************************************************************/

/* Number of parameters that generator functions can take.
   Six should be more than enough.
*/

#ifndef FSTI_GEN_PARMS
#define FSTI_GEN_PARMS 6
#endif

/* Function for setting the iteration until which an agent remains single.
   The default function uses a Weibull function with user defined shape
   and scale.
*/

#ifndef FSTI_SET_SINGLE_PERIOD
#define FSTI_SET_SINGLE_PERIOD set_single_period
#endif

/*
   List of generator functions (in alphabetical order to support bsearch).
*/

#ifndef FSTI_GENERATOR_MAP
#define FSTI_GENERATOR_MAP                                              \
    {"AGE", fsti_gen_age},                                              \
    {"BETA", fsti_gen_beta},                                            \
    {"CONST", fsti_gen_const},                                          \
    {"SEX_SEXOR", fsti_gen_sex_sexor},                                  \
    {"SEX_SEXOR_INFECTION", fsti_gen_sex_sexor_infection},              \
    {"UNIF", fsti_gen_uniform}
#endif



/* Hooks */

/*
   Hook for flex report event. Use this to add additional report lines.
*/

#ifndef FSTI_HOOK_FLEX_REPORT
#define FSTI_HOOK_FLEX_REPORT
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
