#include "fsti-defs.h"
#include "fsti-userdefs.h"
#include "fsti-defaults.h"

#define FSTI_YEAR 1.0
#define FSTI_MONTH 12.0 / 365.0
#define FSTI_DAY 1.0 / 365.0

int fsti_config_set_default(struct fsti_config *config)
{
    FSTI_CONFIG_ADD(config, "NUM_SIMULATIONS",
		    "Number of simulations to execute (default is 1)", 0);
    FSTI_CONFIG_ADD(config, "NUM_THREADS",
		    "Number of threads. "
		    "Default is 1 thread. 0 uses number of cores.", 1);
    FSTI_CONFIG_ADD(config, "TIME_STEP",
		    "Time step for each iteration of simulation",
		    FSTI_DAY);
    FSTI_CONFIG_ADD(config, "NUM_AGENTS", "Number of agents", 1000);
    FSTI_CONFIG_ADD(config, "START_DATE", "Start date of simulation",
		    2018);
    FSTI_CONFIG_ADD(config, "END_DATE", "End date of simulation", 2028);
    FSTI_CONFIG_ADD(config, "STABILIZATION_STEPS",
		    "Number of time steps to run before executing "
		    "various events", 0);
    FSTI_CONFIG_ADD(config, "MATCH_K",
                    "Value for k when using matching algorithms.",
                    100);
    FSTI_CONFIG_ADD(config, "BEFORE_EVENTS",
		    "Events executed before a simulation starts",
		    "_READ_AGENTS");
    FSTI_CONFIG_ADD(config, "DURING_EVENTS",
		    "Events executed on every time step of a simulation",
		    "_AGE");
    {
        struct fsti_variant variant[] = {
            {(union fsti_value) {.str="_REPORT"}, STR},
            {(union fsti_value) {.str="_WRITE_AGENTS"}, STR}
        };
        fsti_config_add_arr(config, "AFTER_EVENTS",
                            "Events executed before a simulation starts",
                            variant,
                            sizeof(variant) /
                            sizeof(struct fsti_variant));
    }
    FSTI_CONFIG_ADD(config, "STOP_EVENT",
		    "Event that decides if simulation should stop",
		    "_STOP");
    FSTI_CONFIG_ADD(config, "AGENT_FILE", "Name of csv file containing agents",
                    "agents_in.csv");
    FSTI_CONFIG_ADD(config, "MUTUAL_CSV_PARTNERS",
                    "After initializing agents, make all partnerships mutual",
                    1);
    FSTI_CONFIG_ADD(config, "REPORT_FREQUENCY",
                    "Frequency that report event is generated", 1);
    FSTI_CONFIG_ADD(config, "RESULTS_FILE",
                    "File name to output results to ('stdout' for stdout)",
                    "stdout");
    FSTI_CONFIG_ADD(config, "AGENTS_OUTPUT_FILE",
                    "File name to output results to ('stdout' for stdout)",
                    "stdout");
    FSTI_CONFIG_ADD(config, "THREADS",
                    "Number of threads (1=no threading, 0=system determined)",
                    0);
    FSTI_ADDITIONAL_CONFIG_VARS(config);

    return 0;
}
