/*
 * Users who wish to extend FastSTI code are encouraged to modify this file.
 * It will not be overwritten when fastSTI is upgraded.
 */


#ifndef FSTI_USERDEFS // Don't remove these guards
#define FSTI_USERDEFS



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
#define FSTI_AGENT_FIELDS

/*
   Override the maximum number of partners an agent can have concurrently.
*/
#define FSTI_MAX_PARTNERS 3

#endif
