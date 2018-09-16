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
    fprintf(file_handle, "%u,%u,%u,%u,%.3f,%.0f,%u\n",                \
            id,                                                       \
            agent->id,                                                \
            (unsigned) agent->sex,                                    \
            (unsigned) agent->sex_preferred,                          \
            agent->age,                                               \
            agent->infected,                                          \
            (agent->partners[0] ? agent->partners[0]->id : 0))


/*
   Change this macro to pretty print an agent.
*/
#define FSTI_AGENT_PRINT_PRETTY(file_handle, id, agent)              \
    fprintf(file_handle, "Sim %u, Agent %u: Age: %.0f, %s %s %s\n",     \
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
            agent->partners[0] ? "Relationship" : "Single")


/*
  Define the format of the CSV input file here.
 */
#define FSTI_CSV_ENTRIES                                        \
    FSTI_CSV_ENTRY(id, fsti_to_unsigned)                        \
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
  Define this to calle a distance function to measure suitability for two agents
  to match. The default version makes people with compatible sexual orientations
  and close ages more likely to match.
*/

#define FSTI_AGENT_DISTANCE(agent_a, agent_b)   \
    fsti_agent_default_distance(agent_a, agent_b)

/*******************************************************************
 * Advanced macros: Please only override these if you have a
 * good understanding of both C and the FastSTI code.
 *******************************************************************/

/*
  ADVANCED
  Put additonal code needed to successfully deep copy an agent array here.
*/

#define FSTI_DEEP_COPY(dest, src) do {      \
    } while(0)


/*
  ADVANCED
  Put additonal code needed to successfully free an agent.
*/

#define FSTI_AGENT_FREE(agent_arr, index)  do {    \
    } while(0)


/*
  ADVANCED
  Put additonal code needed to successfully free a simulation
*/

#define FSTI_SIMULATION_FREE(simulation)  do {    \
    } while(0)

/*
  ADVANCED
  For optimal speed, put frequently used config entries into simulation
  variables.
*/

#define FSTI_ADDITIONAL_CONFIG_TO_VARS(simulation)  do {        \
    } while(0)

#endif
