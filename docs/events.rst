.. _event-ref:

######
Events
######

The events to run for a simulation are specified in these parameters:
- before_events,
- during_events, and
-  after_events.

These, respectively, are the events run once at the start of each simulation
(before_events), on every iteration of the simulation (during_events), and once
at the end of the simulation (after_events).

Here is a way to specify a comprehensive simulation:

.. code-block:: ini
   :linenos:

    before_events = _write_agents_csv_header; _write_results_csv_header; _generate_and_pair; _write_agents_csv; _report
    during_events = _age; _breakup_and_pair; _infect; _stage; _birth; _death; _report
    after_events = _write_agents_csv


This tells FastSTI that before each simulation is run, it must:

- Write the header line for the output agents csv file
  (_write_agents_csv_header)
- Write the header line for the output results file (_write_results_csv_header)
- Generate and initialize properly a new set of agents and put some of them into
  relationships (_generate_and_pair)
- Write the set of initialised agents to a CSV file.
- Write some stats (e.g. prevalence, agents alive, agents dead etc) to the
  results file (_report).

On every time step of a simulation, it tells FastSTI to:

- Increment each living agent's by the time step (_age).
- Iterate through the living agents and put some of the single ones into
  relationships and break up some of those that are in relationships
  (_breakup_and_pair).
- Iterate through the agents with partners and infect some of those in
  sero-discordant relationships (_infect).
- Iterate through the infected agents and move some of them to a new disease
  stage (e.g. treated, resistant, ill - whatever you specify actually).
- Add some new agents to the simulation at the minimum age (_birth).
- Kill some of the agents (_death).
- Write some stats (e.g. prevalence, agents alive, agents dead etc) to the
  results file (_report).

At the end of each simulation it tells FastSTI to:

- Write the final state of the agents to a CSV file (_write_agents_csv).

The events provided by FastSTI (which include those in the example above) are
prefixed with an underscore (_), to differentiate them from other 3rd-party
events or ones that you choose to implement. Please don't name your events with
a leading underscore.

If the events provided by FastSTI are not all you need, then you are encouraged
to code your own events in C in the source code files fsti-userdefs.h and
fsti-userdefs.c.

************
_read_agents
************

Reads in a csv file of agents at the beginning of a simulation. The first
time it is executed, it saves the agents in memory so that on subsequent
simulations, it doesn't have to process the file again.

If you don't have a file of agents to read in, consider using the
_generate_agents event.

Datasets: None

****************
_generate_agents
****************

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

If you wish to set additional agent properties, you must provide hook in by
defining a macro called FSTI_HOOK_GENERATE_AGENT in fsti-userdefs.h. The macro
takes two parameters: simulation (a pointer to the current simulation) and
agent (a pointer to the current agent whose property you wish to set).

.. tip:: Instead of using this event, it will usually make more sense to use the
         _generate_and_pair event which generates the initial set of agents,
         places them in a mating pool, shuffles the mating pool, mates the
         agents in the mating pool and then sets the number of time steps each
         agent will stay in its current relationship or stay single. This event
         is in fact called by _generate_and_pair.

See also: *_generate_and_pair*

****
_age
****

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

******
_death
******

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

***************
_initial_mating
***************

Before a simulation starts but after agents have been generated or read in
from a file, it is possible that none of the agents are in sexual
relationships.

This event is responsible for creating the initial mating pool of agent sexual
relationships. It is typically only run once per simulation, and only if the
agent input file doesn't specify relationships. It is set as an event to run
in the before_events parameter.

Note that it doesn't actually put the agents into relationships, only into a
mating pool. An agent pairing event, such as _rkpm must then be executed in
order to actually place agents in relationships with each other.

.. tip:: Instead of using this event, it will usually make more sense to use the
         _generate_and_pair event which generates the initial set of agents,
         places them in a mating pool, shuffles the mating pool, mates the
         agents in the mating pool and then sets the number of time steps each
         agent will stay in its current relationship or stay single. This event
         is in fact called by _generate_and_pair.

Datasets: dataset_gen_mating

Here is an example of this dataset. The first column is age in five-year
periods. So for example line 5 corresponds to the probability of a person aged
15 to just shy of 20 being in a relationship (which in this example is 0.3 or
30\%).

.. code-block:: none
   :linenos:

     age|5-YEAR;0
     0;0.0
     1;0.0
     2;0.0
     3;0.3
     4;0.35
     5;0.4
     6;0.45
     7;0.5
     8;0.5
     9;0.5
     10;0.5
     11;0.5
     12;0.4
     13;0.4
     14;0.35
     15;0.3
     16;0.25
     17;0.2
     18;0.15
     19;0.1
     20;0.5
     21;0

See also: *_generate_and_pair*

************
_initial_rel
************

For each living agent make a correction to the duration (number of time steps)
its current relationship, or if the agent is single, set the period it will stay
single.

This event assumes the relchange (the date/time in the future at which it's
current relationship or single status changes) property of agents in
relationships has been set. It multiplies it by a uniform random number between
0 and 1. If the agent is single it sets the single period and also multiples it
by a uniform random number between 0 and 1.

Why use this event? Because the simulation starts at an arbitrary time point in
which people are already in the middle of relationships or a period of being
being single. This event will on average halve the value of relchange. Whether
that's a valid assumption at the beginning of a simulation is unclear to us.

.. tip:: Instead of using this event, it will usually make more sense to use the
         _generate_and_pair event which generates the initial set of agents,
         places them in a mating pool, shuffles the mating pool, mates the
         agents in the mating pool and then sets the number of time steps each
         agent will stay in its current relationship or stay single. This event
         is in fact called by _generate_and_pair.

Datasets: None

See also: *_generate_and_pair*, *_breakup* and *_rkpm*. The latter two also set
the relchange property.

TO DO FROM HERE


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
