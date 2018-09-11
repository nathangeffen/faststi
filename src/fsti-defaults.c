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
    FSTI_CONFIG_ADD(config, "BEFORE_EVENTS",
		    "Events executed before a simulation starts",
		    "_READ_AGENTS");
    FSTI_CONFIG_ADD(config, "DURING_EVENTS",
		    "Events executed on every time step of a simulation",
		    "_AGE");
    FSTI_CONFIG_ADD(config, "AFTER_EVENTS",
		    "Events executed before a simulation starts",
		    "_REPORT");
    FSTI_CONFIG_ADD(config, "STOP_EVENT",
		    "Event that decides if simulation should stop",
		    "_STOP");
    FSTI_CONFIG_ADD(config, "AGENT_FILE", "Name of csv file containing agents",
                    "agents.csv");
    FSTI_CONFIG_ADD(config, "REPORT_FREQUENCY",
                    "Frequency that report event is generated", 1);
    FSTI_CONFIG_ADD(config, "THREADS",
                    "Number of threads (1=no threading, 0=system determined)",
                    0);

    return 0;
}
