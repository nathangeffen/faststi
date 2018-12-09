#include "fsti-defs.h"
#include "fsti-userdefs.h"
#include "fsti-defaults.h"

int fsti_config_set_default(struct fsti_config *config)
{
    FSTI_CONFIG_ADD(config, "NUM_SIMULATIONS",
		    "Number of simulations to execute (default is 1)", 0);
    FSTI_CONFIG_ADD(config, "NUM_THREADS",
		    "Number of threads. "
		    "Default is 1 thread. 0 uses number of cores.", 1);
    FSTI_CONFIG_ADD(config, "TIME_STEP",
		    "Time step for each iteration of simulation in minutes"
                    "(default 1440 minutes == 1 day)",
		    FSTI_DAY);
    fsti_config_add(config, "START_DATE", "Start date of simulation "
                    "(julian format yyyy;d, where 2018;0 = 1 Jan 2018)",
		    "2018;0");
    FSTI_CONFIG_ADD(config, "NUM_TIME_STEPS",
                    "Number of iterations simulation should run for "
                    "(10 years)", FSTI_YEAR * 10 / FSTI_DAY);
    FSTI_CONFIG_ADD(config, "AGE_INPUT_TIME_STEP",
                    "Denomination of age in input files "
                    "(usually year or 5-year age groups - defaults to year)",
                    FSTI_YEAR);
    FSTI_CONFIG_ADD(config, "STABILIZATION_STEPS",
		    "Number of time steps to run before executing "
		    "various events", 0);
    FSTI_CONFIG_ADD(config, "NUM_AGENTS", "Number of agents", 1000);
    FSTI_CONFIG_ADD(config, "MATCH_K",
                    "Value for k when using matching algorithms.",
                    100);
    FSTI_CONFIG_ADD(config, "INITIAL_MATING_PROB",
                    "Probability of entering initial mating pool.",
                    0.65);
    FSTI_CONFIG_ADD(config, "MATING_PROB",
                    "Probability of entering mating pool on time step.",
                    0.05);
    FSTI_CONFIG_ADD(config, "PROB_MALE",
                    "Probability an agent is males.", 0.5);
    FSTI_CONFIG_ADD(config, "PROB_MSM",
                    "Probability a male is MSM.", 0.05);
    FSTI_CONFIG_ADD(config, "PROB_WSW",
                    "Probability a female is WSW.", 0.05);
    FSTI_CONFIG_ADD(config, "AGE_MIN",
                    "Lower end of uniformly distributed age for new agents.",
                    15.0);
    FSTI_CONFIG_ADD(config, "AGE_RANGE",
                    "Uniformly distributed size of age range for new agents.",
                    25.0);
    FSTI_CONFIG_ADD(config, "INITIAL_INFECTION_RATE",
                    "Initial infection rate for the population.",  0.01);
    FSTI_CONFIG_ADD(config, "INITIAL_SINGLE_RATE",
                    "Initial proportion of population that is single.",  0.5);
    FSTI_CONFIG_ADD(config, "BEFORE_EVENTS",
		    "Events executed before a simulation starts",
		    "_GENERATE_AGENTS");
    FSTI_CONFIG_ADD(config, "DURING_EVENTS",
		    "Events executed on every time step of a simulation",
		    "_AGE");
    fsti_config_add(config, "AFTER_EVENTS",
                    "Events executed before a simulation starts",
                    "_REPORT;_WRITE_AGENTS_CSV");
    FSTI_CONFIG_ADD(config, "AGENTS_INPUT_FILE",
                    "Name of csv file containing agents", "agents_in.csv");
    FSTI_CONFIG_ADD(config, "MUTUAL_CSV_PARTNERS",
                    "After initializing agents, make all partnerships mutual",
                    1);
    FSTI_CONFIG_ADD(config, "REPORT_FREQUENCY",
                    "Frequency that report event is generated", 1);
    FSTI_CONFIG_ADD(config, "RESULTS_FILE",
                    "File name to output results to (empty string for stdout)",
                    "");
    FSTI_CONFIG_ADD(config, "AGENTS_OUTPUT_FILE",
                    "File name to output results to (empty string for stdout)",
                    "");
    FSTI_CONFIG_ADD(config, "THREADS",
                    "Number of threads (1=no threading, 0=system determined)",
                    0);
    FSTI_CONFIG_ADD(config, "CSV_DELIMITER",
                    "Character that separates CSV fields", ";");
    FSTI_CONFIG_ADD(config, "AGENT_CSV_HEADER",
                    "Whether or not the agent csv input file has a header", 1);
    FSTI_CONFIG_ADD(config, "DATASET_MORTALITY",
                    "CSV file of values to determine agent deaths", FSTI_NO_OP);
    FSTI_CONFIG_ADD(config, "DATASET_MATING_POOL",
                    "CSV file of values to determine agents entering mating pool",
                    FSTI_NO_OP);
    fsti_config_add(config, "A.age", "Agent age field",
                    "AGE;1;1.23;15;50");
    fsti_config_add(config, "A.sex",
                   "Agent sex, sex_preference, infection fields",
                   "SEX_SEXOR_INFECTION;0.5;0.05;0.005;0.005;0.005;0.005");
    FSTI_ADDITIONAL_CONFIG_VARS(config);

    return 0;
}
