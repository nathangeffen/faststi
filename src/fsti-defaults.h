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

/* Add your own configuration variables here. */

#ifndef FSTI_ADDITIONAL_CONFIG_VARS
#define FSTI_ADDITIONAL_CONFIG_VARS(config)                             \
    FSTI_CONFIG_ADD(config, "EXAMPLE_1", "Example configuration field", 0); \
    FSTI_CONFIG_ADD(config, "EXAMPLE_2",                                \
                    "Example configuration field", "String field")
#endif

/*
   This is the default code to print an agent in csv format. Change it to print another
   way.
*/

#ifndef FSTI_AGENT_PRINT_CSV
#define FSTI_AGENT_PRINT_CSV(file_handle, id, agent)                  \
    fprintf(file_handle, "%u,%zu,%u,%u,%.3f,%u,%ld\n",              \
            id,                                                       \
            agent->id,                                                \
            (unsigned) agent->sex,                                    \
            (unsigned) agent->sex_preferred,                          \
            agent->age,                                               \
            agent->infected,                                          \
            agent->num_partners ? (long) agent->partners[0] : -1)
#endif

/*
   Change this macro to pretty print an agent.
*/
#ifndef FSTI_AGENT_PRINT_PRETTY
#define FSTI_AGENT_PRINT_PRETTY(file_handle, id, agent)              \
    fprintf(file_handle, "Sim %u, Agent %zu: Age: %.0f, %s %s %s %ld\n", \
            id,                                                         \
            agent->id,                                                  \
            agent->age,                                                 \
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
  Define the format of the CSV input file here.
 */
#ifndef FSTI_CSV_ENTRIES
#define FSTI_CSV_ENTRIES                                        \
    FSTI_CSV_ENTRY(id, fsti_to_size_t)                          \
    FSTI_CSV_ENTRY(age, fsti_to_float)                          \
    FSTI_CSV_ENTRY(infected, fsti_to_float)                     \
    FSTI_CSV_ENTRY(sex, fsti_to_uchar)                          \
    FSTI_CSV_ENTRY(sex_preferred, fsti_to_uchar)                \
    FSTI_CSV_ENTRY(partners[0], fsti_to_partner)
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

/*******************************************************************
 * Advanced macros: Please only override these if you have a
 * good understanding of both C and the FastSTI code.
 *******************************************************************/

/* Hooks */

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
#define FSTI_HOOK_AFTER_MATCH(a, b)
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
  Hook to register events.
*/

#ifndef FSTI_HOOK_EVENTS_REGISTER
#define FSTI_HOOK_EVENTS_REGISTER
#endif

int fsti_config_set_default(struct fsti_config *config);

#endif
