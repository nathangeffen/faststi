#include "fsti-defs.h"
#include "fsti-userdefs.h"
#include "fsti-defaults.h"
#include "fsti-simulation.h"



#define FSTI_CONFIG_ADD2(config, elem, desc,  ...)              \
    do {                                                        \
        struct fsti_simulation *_s;                             \
    } while(0)

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
                    "(yyyy;mm;dd)", "2018;1;1");
    FSTI_CONFIG_ADD(config, "NUM_TIME_STEPS",
                    "Number of iterations simulation should run for "
                    "(10 years)", (FSTI_YEAR * 10) / FSTI_DAY);
    FSTI_CONFIG_ADD(config, "AGE_INPUT_TIME_STEP",
                    "Denomination of age in input files "
                    "(usually year or 5-year age groups - defaults to year)",
                    FSTI_YEAR);
    FSTI_CONFIG_ADD(config, "STABILIZATION_STEPS",
		    "Number of time steps to run before executing "
		    "various events", 0);
    FSTI_CONFIG_ADD(config, "NUM_AGENTS", "Number of agents", 20000);
    FSTI_CONFIG_ADD(config, "MATCH_K",
                    "Value for k when using matching algorithms",
                    100);
    FSTI_CONFIG_ADD(config, "INITIAL_MATING_PROB",
                    "Probability of entering initial mating pool",
                    0.65);
    FSTI_CONFIG_ADD(config, "MATING_PROB",
                    "Probability of entering mating pool on time step",
                    0.05);
    FSTI_CONFIG_ADD(config, "PROB_MALE",
                    "Probability an agent is male.", 0.5);
    FSTI_CONFIG_ADD(config, "PROB_MSM",
                    "Probability a male is MSM.", 0.05);
    FSTI_CONFIG_ADD(config, "PROB_WSW",
                    "Probability a female is WSW.", 0.05);
    FSTI_CONFIG_ADD(config, "AGE_MIN",
                    "Lower end of uniformly distributed age for new or "
                    "generated agents", 15.0 * FSTI_YEAR);
    FSTI_CONFIG_ADD(config, "AGE_MAX",
                    "Upper end of uniformly distributed age for "
                    "generated agents",  50.0 * FSTI_YEAR);
    FSTI_CONFIG_ADD(config, "AGE_ALPHA",
                    "Alpha parameter for age distribution of new agents", 0.3);
    FSTI_CONFIG_ADD(config, "AGE_BETA",
                    "Beta parameter for age distribution of new agents", 1.0);


    FSTI_CONFIG_ADD(config, "AGE_RANGE",
                    "Uniformly distributed size of age range for new agents.",
                    25.0);
    FSTI_CONFIG_ADD(config, "INITIAL_INFECTION_RATE",
                    "Initial infection rate for the population",  0.01);
    FSTI_CONFIG_ADD(config, "INITIAL_TREATED_RATE",
                    "Initial treatment rate for infected population",  0.3);
    FSTI_CONFIG_ADD(config, "INITIAL_RESISTANT_RATE",
                    "Initial resistant rate for treated population",  0.05);
    FSTI_CONFIG_ADD(config, "INITIAL_SINGLE_RATE",
                    "Initial proportion of population that is single",  0.5);
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
    FSTI_CONFIG_ADD(config, "PARTNERSHIPS_FILE",
                    "File name to output partnerships (empty string for stdout)",
                    "");
    FSTI_CONFIG_ADD(config, "OUTPUT_MATCHES",
                    "Whether to output matches to the partnership file", 0);
    FSTI_CONFIG_ADD(config, "OUTPUT_BREAKUPS",
                    "Whether to output breakups to the partnership file", 0);
    FSTI_CONFIG_ADD(config, "OUTPUT_INFECTIONS",
                    "Whether to output infections to the partnership file", 0);

    FSTI_CONFIG_ADD(config, "THREADS",
                    "Number of threads (1=no threading, 0=system determined)",
                    0);
    FSTI_CONFIG_ADD(config, "CSV_DELIMITER",
                    "Character that separates CSV fields", ";");
    FSTI_CONFIG_ADD(config, "AGENT_CSV_HEADER",
                    "Whether or not the agent csv input file has a header", 1);

    fsti_config_add(config, "INFECTION_RISK",
                    "Risk of infection for msm, msw, wsm, wsw respectively. ",
                    "0.01;0.005;0.0075;0.001");

    FSTI_CONFIG_ADD(config, "INITIAL_INFECT_STAGE",
                    "When infected this is the integer to set infected to",  2);
    FSTI_CONFIG_ADD(config, "TREATMENT_INFECT_STAGE",
                    "When treated this is the integer to set infected to", 1);
    FSTI_CONFIG_ADD(config, "MAX_STAGE",
                    "Maximum infection stage (e.g. 6 for HIV "
                    "1=virally suppressed, 2=primary 3-6=WHO 1-4)" , 6);
    FSTI_CONFIG_ADD(config, "TREATMENT_SUCCESS_STAGE",
                    "Infection stage to which agent goes if treatment "
                    "is a success", 6);

    // Datasets
    FSTI_CONFIG_ADD(config, "DATASET_MORTALITY",
                    "CSV file of values to determine agent deaths",
                    FSTI_NO_OP);
    FSTI_CONFIG_ADD(config, "DATASET_SINGLE_PERIOD",
                    "CSV file of values to determine period agent is single",
                    FSTI_NO_OP);
    FSTI_CONFIG_ADD(config, "DATASET_REL_PERIOD",
                    "CSV file of values to determine period agent is in "
                    "relationship", FSTI_NO_OP);
    FSTI_CONFIG_ADD(config, "DATASET_INFECT_STAGE",
                    "CSV file of values to determine when agent advances "
                    "to next stage of infection",
                    FSTI_NO_OP);
    FSTI_CONFIG_ADD(config, "DATASET_INFECT",
                    "CSV file of values to determine if agent has been infected "
                    "by a sexual partner", FSTI_NO_OP);

    FSTI_CONFIG_ADD(config, "DATASET_GEN_SEX",
                    "CSV file of values to set sex of generated agent",
                    FSTI_NO_OP);
    FSTI_CONFIG_ADD(config, "DATASET_GEN_SEX_PREFERRED",
                    "CSV file of values to set sex preferred of generated agent",
                    FSTI_NO_OP);
    FSTI_CONFIG_ADD(config, "DATASET_GEN_INFECT",
                    "CSV file of values to set infection stage of "
                    "generated agent", FSTI_NO_OP);
    FSTI_CONFIG_ADD(config, "DATASET_GEN_TREATED",
                    "CSV file of values to set infected agents treatment status",
                    FSTI_NO_OP);
    FSTI_CONFIG_ADD(config, "DATASET_GEN_RESISTANT",
                    "CSV file of values to set treated agents resistant status",
                    FSTI_NO_OP);
    FSTI_CONFIG_ADD(config, "DATASET_GEN_MATING",
                    "CSV file of values to select agents for initial mating "
                    "pool", FSTI_NO_OP);

    FSTI_CONFIG_ADD(config, "DATASET_BIRTH_INFECT",
                    "CSV file of values to set infection stage of "
                    "new agent", FSTI_NO_OP);
    FSTI_CONFIG_ADD(config, "DATASET_BIRTH_TREATED",
                    "CSV file of values to set new infected agents "
                    "treatment status", FSTI_NO_OP);
    FSTI_CONFIG_ADD(config, "DATASET_BIRTH_RESISTANT",
                    "CSV file of values to set treated agents resistant status",
                    FSTI_NO_OP);

    FSTI_CONFIG_ADD(config, "DATASET_COINFECT",
                    "CSV file of values to determine if agent has coinfection",
                    FSTI_NO_OP);


    fsti_config_add(config, "A.age", "Agent age field",
                    "AGE;1;1.23;15;50");
    fsti_config_add(config, "A.sex",
                   "Agent sex, sex_preference, infection fields",
                   "SEX_SEXOR_INFECTION;0.5;0.05;0.005;0.005;0.005;0.005");

    FSTI_CONFIG_ADD(config, "BIRTH_EVENT_EVERY_N",
                    "Every n iterations that birth event should execute", 73);
    FSTI_CONFIG_ADD(config, "BIRTH_RATE",
                    "Birth rate for the period of BIRTH_EVENT_EVERY_N",
                    0.003968379);
    FSTI_CONFIG_ADD(config, "PROB_BIRTH_MALE",
                    "Probability a new agent is male", 0.5);
    FSTI_CONFIG_ADD(config, "PROB_BIRTH_MSW",
                    "Probability a new male agent is msw", 0.95);
    FSTI_CONFIG_ADD(config, "PROB_BIRTH_WSM",
                    "Probability a new female agent is wsm", 0.95);

    FSTI_CONFIG_ADD(config, "PROB_GEN_MALE",
                    "Probability a generated agent is male", 0.5);
    FSTI_CONFIG_ADD(config, "PROB_GEN_MSW",
                    "Probability a generated male agent is msw", 0.95);
    FSTI_CONFIG_ADD(config, "PROB_GEN_WSM",
                    "Probability a generated female agent is wsm", 0.95);

    FSTI_CONFIG_ADD(config, "PROB_BIRTH_INFECTED_MSM",
                    "Probability a new msm agent is infected", 0.001);
    FSTI_CONFIG_ADD(config, "PROB_BIRTH_INFECTED_MSW",
                    "Probability a new msw agent is infected", 0.0001);
    FSTI_CONFIG_ADD(config, "PROB_BIRTH_INFECTED_WSM",
                    "Probability a new wsm agent is infected", 0.0005);
    FSTI_CONFIG_ADD(config, "PROB_BIRTH_INFECTED_WSW",
                    "Probability a new wsw agent is infected", 0.0001);

    FSTI_CONFIG_ADD(config, "EVENT_TEST_FREQ",
                    "Run test cases in test events every nth iteration", 100);

    FSTI_ADDITIONAL_CONFIG_VARS(config);

    return 0;
}
