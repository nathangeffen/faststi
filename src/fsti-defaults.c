#include "fsti-defs.h"
#include "fsti-userdefs.h"
#include "fsti-defaults.h"
#include "fsti-simulation.h"



int fsti_config_set_default(struct fsti_config *config)
{
    FSTI_CONFIG_ADD(config, num_simulations,
                     "Number of simulations to execute (default is 1)", 1);
    FSTI_CONFIG_ADD(config, num_threads,
                     "Number of threads. "
                     "Default is 1 thread. 0 uses number of cores.", 1);
    FSTI_CONFIG_ADD(config, time_step,
		    "Time step for each iteration of simulation in minutes"
                    "(default 1440 minutes == 1 day)",
		    FSTI_DAY);
    FSTI_CONFIG_ADD(config, start_date, "Start date of simulation "
                          "(yyyy;mm;dd)", 2018,1,1);
    FSTI_CONFIG_ADD(config, num_time_steps,
                    "Number of iterations simulation should run for "
                    "(10 years)", (FSTI_YEAR * 10) / FSTI_DAY);
    FSTI_CONFIG_ADD(config, age_input_time_step,
                    "Denomination of age in input files "
                    "(usually year or 5-year age groups - defaults to year)",
                    FSTI_YEAR);
    FSTI_CONFIG_ADD(config, stabilization_steps,
		    "Number of time steps to run before executing "
		    "various events", 0);
    FSTI_CONFIG_ADD(config, num_agents, "Number of agents", 20000);
    FSTI_CONFIG_ADD(config, match_k,
                    "Value for k when using matching algorithms",
                    100);
    FSTI_CONFIG_ADD(config, initial_mating_prob,
                    "Probability of entering initial mating pool",
                    0.65);
    FSTI_CONFIG_ADD(config, mating_prob,
                    "Probability of entering mating pool on time step",
                    0.05);
    FSTI_CONFIG_ADD(config, prob_male,
                     "Probability an agent is male.", 0.5);
    FSTI_CONFIG_ADD(config, prob_msm,
                    "Probability a male is MSM.", 0.05);
    FSTI_CONFIG_ADD(config, prob_wsw,
                    "Probability a female is WSW.", 0.05);
    FSTI_CONFIG_ADD(config, age_min,
                    "Lower end of uniformly distributed age for new or "
                    "generated agents", 15.0 * FSTI_YEAR);
    FSTI_CONFIG_ADD(config, age_max,
                    "Upper end of uniformly distributed age for "
                    "generated agents",  50.0 * FSTI_YEAR);
    FSTI_CONFIG_ADD(config, age_alpha,
                    "Alpha parameter for age distribution of new agents", 0.3);
    FSTI_CONFIG_ADD(config, age_beta,
                    "Beta parameter for age distribution of new agents", 1.0);


    FSTI_CONFIG_ADD(config, age_range,
                    "Uniformly distributed size of age range for new agents.",
                    25.0);
    FSTI_CONFIG_ADD(config, initial_infection_rate,
                    "Initial infection rate for the population",  0.01);
    FSTI_CONFIG_ADD(config, initial_treated_rate,
                    "Initial treatment rate for infected population",  0.3);
    FSTI_CONFIG_ADD(config, initial_resistant_rate,
                    "Initial resistant rate for treated population",  0.05);
    FSTI_CONFIG_ADD(config, initial_single_rate,
                    "Initial proportion of population that is single",  0.5);
    FSTI_CONFIG_ADD_STR(config, before_events,
		    "Events executed before a simulation starts",
		    "_generate_agents");
    FSTI_CONFIG_ADD_STR(config, during_events,
		    "Events executed on every time step of a simulation",
		    "_age");
    FSTI_CONFIG_ADD_STR(config, after_events,
                         "Events executed before a simulation starts",
                        "_report", "_write_agents_csv");
    FSTI_CONFIG_ADD_STR(config, agents_input_file,
                    "Name of csv file containing agents", "agents_in.csv");
    FSTI_CONFIG_ADD(config, mutual_csv_partners,
                    "After initializing agents, make all partnerships mutual",
                    1);
    FSTI_CONFIG_ADD(config, report_frequency,
                    "Frequency that report event is generated", 1);
    FSTI_CONFIG_ADD_STR(config, results_file,
                    "File name to output results to (empty string for stdout)",
                    "");
    FSTI_CONFIG_ADD_STR(config, agents_output_file,
                    "File name to output results to (empty string for stdout)",
                    "");
    FSTI_CONFIG_ADD_STR(config, partnerships_file,
                        "File name to output partnerships (empty string for stdout)",
                        "");
    FSTI_CONFIG_ADD(config, output_matches,
                    "Whether to output matches to the partnership file", 0);
    FSTI_CONFIG_ADD(config, output_breakups,
                    "Whether to output breakups to the partnership file", 0);
    FSTI_CONFIG_ADD(config, output_infections,
                    "Whether to output infections to the partnership file", 0);

    FSTI_CONFIG_ADD(config, threads,
                    "Number of threads (1=no threading, 0=system determined)",
                    0);
    FSTI_CONFIG_ADD_STR(config, csv_delimiter,
                    "Character that separates CSV fields", ";");
    FSTI_CONFIG_ADD(config, agent_csv_header,
                    "Whether or not the agent csv input file has a header", 1);

    FSTI_CONFIG_ADD(config, infection_risk,
                    "Risk of infection for msm, msw, wsm, wsw respectively",
                    0.01,0.005,0.0075,0.001);

    FSTI_CONFIG_ADD(config, initial_infect_stage,
                    "When infected this is the integer to set infected to",  2);
    FSTI_CONFIG_ADD(config, treatment_infect_stage,
                    "When treated this is the integer to set infected to", 1);
    FSTI_CONFIG_ADD(config, max_stage,
                    "Maximum infection stage (e.g. 6 for HIV "
                    "1=virally suppressed, 2=primary 3-6=WHO 1-4)" , 6);
    FSTI_CONFIG_ADD(config, "TREATMENT_SUCCESS_STAGE",
                    "Infection stage to which agent goes if treatment "
                    "is a success", 6);

    // Datasets
    FSTI_CONFIG_ADD_STR(config, dataset_mortality,
                        "CSV file of values to determine agent deaths",
                        FSTI_NO_OP);
    FSTI_CONFIG_ADD_STR(config, dataset_single_period,
                    "CSV file of values to determine period agent is single",
                    FSTI_NO_OP);
    FSTI_CONFIG_ADD_STR(config, dataset_rel_period,
                    "CSV file of values to determine period agent is in "
                    "relationship", FSTI_NO_OP);
    FSTI_CONFIG_ADD_STR(config, dataset_infect_stage,
                    "CSV file of values to determine when agent advances "
                    "to next stage of infection",
                    FSTI_NO_OP);
    FSTI_CONFIG_ADD_STR(config, dataset_infect,
                    "CSV file of values to determine if agent has been infected "
                    "by a sexual partner", FSTI_NO_OP);

    FSTI_CONFIG_ADD_STR(config, dataset_gen_sex,
                    "CSV file of values to set sex of generated agent",
                    FSTI_NO_OP);
    FSTI_CONFIG_ADD_STR(config, dataset_gen_sex_preferred,
                    "CSV file of values to set sex preferred of generated agent",
                    FSTI_NO_OP);
    FSTI_CONFIG_ADD_STR(config, dataset_gen_infect,
                    "CSV file of values to set infection stage of "
                    "generated agent", FSTI_NO_OP);
    FSTI_CONFIG_ADD_STR(config, dataset_gen_treated,
                    "CSV file of values to set infected agents treatment status",
                    FSTI_NO_OP);
    FSTI_CONFIG_ADD_STR(config, dataset_gen_resistant,
                    "CSV file of values to set treated agents resistant status",
                    FSTI_NO_OP);
    FSTI_CONFIG_ADD_STR(config, dataset_gen_mating,
                    "CSV file of values to select agents for initial mating "
                    "pool", FSTI_NO_OP);

    FSTI_CONFIG_ADD_STR(config, dataset_birth_infect,
                    "CSV file of values to set infection stage of "
                    "new agent", FSTI_NO_OP);
    FSTI_CONFIG_ADD_STR(config, dataset_birth_treated,
                    "CSV file of values to set new infected agents "
                    "treatment status", FSTI_NO_OP);
    FSTI_CONFIG_ADD_STR(config, dataset_birth_resistant,
                    "CSV file of values to set treated agents resistant status",
                    FSTI_NO_OP);

    FSTI_CONFIG_ADD_STR(config, dataset_coinfect,
                    "CSV file of values to determine if agent has coinfection",
                    FSTI_NO_OP);

    FSTI_CONFIG_ADD(config, birth_event_every_n,
                    "Every n iterations that birth event should execute", 73);
    FSTI_CONFIG_ADD(config, birth_rate,
                    "Birth rate for the period of BIRTH_EVENT_EVERY_N",
                    0.003968379);
    FSTI_CONFIG_ADD(config, prob_birth_male,
                    "Probability a new agent is male", 0.5);
    FSTI_CONFIG_ADD(config, prob_birth_msw,
                    "Probability a new male agent is msw", 0.95);
    FSTI_CONFIG_ADD(config, prob_birth_wsm,
                    "Probability a new female agent is wsm", 0.95);

    FSTI_CONFIG_ADD(config, prob_gen_male,
                    "Probability a generated agent is male", 0.5);
    FSTI_CONFIG_ADD(config, prob_gen_msw,
                    "Probability a generated male agent is msw", 0.95);
    FSTI_CONFIG_ADD(config, prob_gen_wsm,
                    "Probability a generated female agent is wsm", 0.95);

    FSTI_CONFIG_ADD(config, prob_birth_infected_msm,
                    "Probability a new msm agent is infected", 0.001);
    FSTI_CONFIG_ADD(config, prob_birth_infected_msw,
                    "Probability a new msw agent is infected", 0.0001);
    FSTI_CONFIG_ADD(config, prob_birth_infected_wsm,
                    "Probability a new wsm agent is infected", 0.0005);
    FSTI_CONFIG_ADD(config, prob_birth_infected_wsw,
                    "Probability a new wsw agent is infected", 0.0001);

    FSTI_CONFIG_ADD(config, event_test_freq,
                    "Run test cases in test events every nth iteration", 100);

    FSTI_ADDITIONAL_CONFIG_VARS(config);

    return 0;
}
