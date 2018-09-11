/*
 * Users who wish to extend FastSTI code are encouraged to modify this file.
 * It will not be overwritten when fastSTI is upgraded.
 */

// Add members to the simulation struct here. Separate them with
// semicolons, but don't put a semicolon after the last element

#define FSTI_USER_SIMULATION_VARS

// EXAMPLE

/* #define FSTI_USER_SIMULATION_VARS	\
     int num_hiv_infections;                  \
     double avg_hiv_infection_period;         \
     int num_deaths
*/

#define FSTI_USER_SIMULATION_INIT_VARS

#define FSTI_USER_AGENT_VARS			\
	double age

void fsti_event_age(struct fsti_simulation *simulation);
void fsti_event_report(struct fsti_simulation *simulation);
void fsti_event_stop(struct fsti_simulation *simulation);
