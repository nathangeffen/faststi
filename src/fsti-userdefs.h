/*
 * Users who wish to extend FastSTI code are encouraged to modify this file.
 * It will not be overwritten when fastSTI is upgraded.
 */

#ifndef FSTI_USERDEFS // Don't remove these guards
#define FSTI_USERDEFS

#include "fsti-defs.h"

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
#define FSTI_AGENT_FIELDS                       \
    void *data; // You can keep, rename or remove this field


/*
   Add fields to the struct fsti_simulation definition here. Note the semicolon
   at the end of the last field.  E.g.

#define FSTI_SIMULATION_FIELDS                    \
bool track_agent_partners;                         \
unsigned total_partners;
*/
#define FSTI_SIMULATION_FIELDS                       \
    void *data; // You can keep, rename or remove this field

/*
   Override the maximum number of partners an agent can have concurrently.
*/
#define FSTI_MAX_PARTNERS 3

/* Add your own configuration variables here. */

#define FSTI_ADDITIONAL_CONFIG_VARS(config)                             \
    FSTI_CONFIG_ADD(config, "EXAMPLE_1", "Example configuration field", 0); \
    FSTI_CONFIG_ADD(config, "EXAMPLE_2",                                \
                    "Example configuration field", "String field")

/*
   This is the default code to print an agent in csv format. Change it to print another
   way.
*/

#define FSTI_AGENT_PRINT_CSV(file_handle, id, agent)                  \
    fprintf(file_handle, "%u,%zu,%u,%u,%.3f,%.0f,%ld\n",              \
            id,                                                       \
            agent->id,                                                \
            (unsigned) agent->sex,                                    \
            (unsigned) agent->sex_preferred,                          \
            agent->age,                                               \
            agent->infected,                                          \
            agent->num_partners ? (long) agent->partners[0] : -1)


/*
   Change this macro to pretty print an agent.
*/
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


/*
  Define the format of the CSV input file here.
 */
#define FSTI_CSV_ENTRIES                                        \
    FSTI_CSV_ENTRY(id, fsti_to_size_t)                          \
    FSTI_CSV_ENTRY(age, fsti_to_float)                          \
    FSTI_CSV_ENTRY(infected, fsti_to_float)                     \
    FSTI_CSV_ENTRY(sex, fsti_to_uchar)                          \
    FSTI_CSV_ENTRY(sex_preferred, fsti_to_uchar)                \
    FSTI_CSV_ENTRY(partners[0], fsti_to_partner)

/*
   Define this to call the function that returns true if  an agent
   has a partner, else false. The default function simply looks at the
   first entry in the partner array.
*/
#define FSTI_AGENT_HAS_PARTNER(agent) fsti_agent_has_partner(agent)

/*
  Define this to call a distance function to measure suitability for two agents
  to match. The default version makes people with compatible sexual orientations
  and close ages more likely to match.
*/

#define FSTI_AGENT_DISTANCE(agent_a, agent_b)   \
    fsti_agent_default_distance(agent_a, agent_b)

/*******************************************************************
 * Advanced macros: Please only override these if you have a
 * good understanding of both C and the FastSTI code.
 *******************************************************************/

/* Hooks */

/*
  Hook after copying an agent array.
*/

#define FSTI_HOOK_AGENT_ARR_COPY(dest_arr, src_arr) do {      \
    } while(0)


/*
  Hook before freeing an agent array.
*/

#define FSTI_HOOK_AGENT_ARR_FREE(agent_arr)  do {    \
    } while(0)


/*
  Hook before freeing a simulation
*/

#define FSTI_HOOK_SIMULATION_FREE(simulation)  do {    \
    } while(0)

/*
  Speed optimization hook. Use it to put frequently used config entries into
  simulation variables.
*/

#define FSTI_HOOK_CONFIG_TO_VARS(simulation)  do {        \
    } while(0)


/*
  Hook after an agent is created, but before being put in the agent array.
*/

#define FSTI_HOOK_CREATE_AGENT(simulation, agent) do {    \
    } while(0)


/*
  Hook to register events.
*/

#define FSTI_HOOK_EVENTS_REGISTER do {    \
    } while(0)

#endif
