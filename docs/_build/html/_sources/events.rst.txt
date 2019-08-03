.. _event-ref:

######
Events
######

- _read_agents

  Reads in a csv file of agents at the beginning of a simulation. The first
  time it is executed, it saves the agents in memory so that on subsequent
  simulations, it doesn't have to process the file again.

  If you don't have a file of agents to read in, consider using the
  _generate_agents event.

  Datasets: None

- _generate_agents

  Generates agents for a simulation instead of reading them from a file.

  It uses the num_agents parameter to determine the number of agents to
  generate.

  The default implementation sets the following agent properties: age, sex,
  sex_preferred, infected, treated and resistant.

  - age is set via a random beta distribution determined by the parameters
    age_alpha and age_beta.

  - sex is set via the dataset dataset_gen_sex. (See data/dataset_gen_sex.csv
    for an example.)

  - sex_preferred is set via the dataset dataset_gen_sex_preferred. (See
    data/dataset_gen_sex_preferred.csv for an example, which sets the agents to
    prefer the opposite sex with a probability of 95%, but you can change this
    according to the needs of your model.)

  - infected (i.e. the infection stage of the agent with 0 by convention meaning
    the agent isn't infected) is set via the dataset dataset_gen_infect. (See
    data/dataset_gen_infect.csv for an example.)

  - treated (i.e. which treatment line, if any, the agent is on) is set via the
    dataset dataset_gen_treated. (See data/dataset_gen_treated.csv for an
    example.)

  - resistant (i.e. which treatment regimens the agent is resistant to, if
    any. (See data/dataset_gen_resistant.csv for an example.)

  If you wish to set additional agent properties, you must provide hook in
  by defining a macro called FSTI_HOOK_GENERATE_AGENT in fsti-userdefs.h. The
  macro takes two parameters: simulation (a pointer to the current simulation)
  and agent (a pointer to the current agent whose property you wish to set).

- _age

  Iterates through all the living agents, and adds the time increment of the
  simulation to their age.

  This is one of the simplest events provided by FastSTI, and so it makes a nice
  example of how events are implemented. Here is the source code:

  .. code-block:: C
      :linenos:

      void
      fsti_event_age(struct fsti_simulation *simulation)
      {
         struct fsti_agent *agent;
         FSTI_FOR_LIVING(*simulation, agent, {
            agent->age += simulation->time_step;
         });
      }

  All events are declared like this, i.e. a void function that takes one
  parameter: a pointer to the simulation itself.

  On line 4 we declare a pointer to an agent on line three. When we iterate
  through the living agents, this will be a pointer to the current agent the
  code acts upon.

  The FSTI_FOR_LIVING macro implements a for loop over the living agents.
  The code inside the macro's curly brackets simply adds the time step to each
  agent's age.

  Datasets: None

- _death

  Iterates through the living agents and kills some of them.

  Datasets: dataset_mortality

  Here is a simple example of this dataset:

  .. code-block:: none
     :linenos:

        infected;0
        0;0.00000296
        1;0.00000315
        2;0.00000315
        3;0.00000630
        4;0.001

  The first column tells the event to determine the infection stage of the
  agent. The second column is the probability of the agent dying on this time
  step. Here the probabilities are specified per day. If you change the time
  step to, say, a week you have to update the probabilities in this file
  accordingly.

TO DO FROM HERE

_initial_mating

_initial_rel

_mating_pool

_breakup

_shuffle_living

_shuffle_mating

_rkpm

_breakup_and_pair

_generate_and_pair

_infect

_stage

_coinfect

_birth

_report

_write_results_csv_header,

_write_agents_csv_header

_write_partnerships_csv_header

_write_agents_csv

_write_living_agents_csv

_write_dead_agents_csv

_write_agents_pretty
