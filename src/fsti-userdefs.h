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
   Override the maximum number of partners an agent can have concurrently.
*/
#define FSTI_MAX_PARTNERS 3

/* Add your own configuration variables here. */

#define FSTI_USER_CONFIG_VARS(config)                                   \
    FSTI_CONFIG_ADD(config, "EXAMPLE_1", "Example configuration field", 0); \
    FSTI_CONFIG_ADD(config, "EXAMPLE_2",                                \
                    "Example configuration field", "String field")      \


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
             agent->sex_preferred == FSTI_HETEROSEXUAL) ? "MSW" :       \
            ((agent->sex == FSTI_MALE &&                                \
             agent->sex_preferred == FSTI_HOMOSEXUAL) ? "MSM" :         \
             ((agent->sex == FSTI_FEMALE &&                             \
               agent->sex_preferred == FSTI_HETEROSEXUAL) ? "WSM" : "WSW")), \
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

#endif
