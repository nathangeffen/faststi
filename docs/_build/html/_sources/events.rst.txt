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


This tells |PROJECT| that before each simulation is run, it must:

- Write the header line for the output agents csv file
  (_write_agents_csv_header)
- Write the header line for the output results file (_write_results_csv_header)
- Generate and initialize properly a new set of agents and put some of them into
  relationships (_generate_and_pair)
- Write the set of initialised agents to a CSV file.
- Write some stats (e.g. prevalence, agents alive, agents dead etc) to the
  results file (_report).

On every time step of a simulation, it tells |PROJECT| to:

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

At the end of each simulation it tells |PROJECT| to:

- Write the final state of the agents to a CSV file (_write_agents_csv).

The events provided by |PROJECT| (which include those in the example above) are
prefixed with an underscore (_), to differentiate them from other 3rd-party
events or ones that you choose to implement. Please don't name your events with
a leading underscore.

If the events provided by |PROJECT| are not all you need, then you are encouraged
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

This is one of the simplest events provided by |PROJECT|, and so it makes a nice
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

************
_mating_pool
************

Iterates through the living agents and places the single ones into the mating
pool if they are due for a relationship status change. The relchange property of
each single agent determines whether it is to be placed in the mating pool.

Note that placing agents in the mating pool is necessary but not sufficient to
pair them into relationships. This event is typically followed by shuffling the
mating pool (_shuffle_mating) and then placing them in sexual relationships with
other agents in the mating pool using the pairing algorithm (_rkpm).

All of these events are included in the composite event _breakup_and_pair.

The C code for this event is simple and instructive:

.. code-block:: C
   :linenos:

      void
      fsti_event_mating_pool(struct fsti_simulation *simulation)
      {
          struct fsti_agent *agent;
          fsti_agent_ind_clear(&simulation->mating_pool);
          FSTI_FOR_LIVING(*simulation, agent, {
              if (agent->num_partners == 0) {
                  if (agent->relchange[0] < simulation->iteration)
                      fsti_agent_ind_push(&simulation->mating_pool, agent->id);
             }
          });
      }

All events are declared like this, i.e. a void function that takes one parameter: a pointer to the simulation itself.

On line 4 we declare a pointer to an agent on line three. When we iterate through the living agents, this will be a pointer to the current agent the code acts upon.

The FSTI_FOR_LIVING macro implements a for loop over the living agents.

The code inside the macro’s curly brackets first checks that the agent is single
(i.e. it has zero partners). If it is it checks if the relchange property is
less than the current iteration. If it is, it places the agent in the mating
pool.

You may have noticed that agent->relchange on line 8 is subscripted with a 0
index. This is because |PROJECT|'s data structures support agents having multiple
concurrent partners. relchange[0] refers to the status of the first partner. By
default, up to three partners are supported and this is determined by
FSTI_MAX_PARTNERS set in fsti-defaults.h. To override this value, either with a
bigger or smaller maximum number of partners, simply define an alternative value
for FSTI_MAX_PARTNERS in fsti-userdefs.h. For example:

.. code-block:: C

    #define FSTI_MAX_PARTNERS 1

But although the |PROJECT| data structures support concurrent partnerships, all of
the default events, including this one, do not support agents having more than
one partner. This may and probably should change in the future. Of course you
are also welcome to implement your own events that do account for concurrent
partnerships.

Datasets: None

See also: *_breakup_and_pair*.

********
_breakup
********

Iterates through the living agents, and breaks up some of those who are in
relationships.

The event looks at the relchange properties of each agent in a relationship. If
relchange is less than the current iteration, it's time for the relationship to
end.

Datasets: None

See also: *_breakup_and_pair*.

***************
_shuffle_living
***************

Shuffles the living agents array. This is useful if an event is biased by the
order in which it processes the agents. If the agents are shuffled before the
event is run, over the long run this may remove the bias.

Datasets: None

***************
_shuffle_mating
***************

Shuffles the mating pool. This is important to run before many pair-matching
algorithms including the _rkpm event provided by |PROJECT|. Stochastic
pair-matching events tend to assign better matches to agents at the beginning of
an array. By shuffling the mating pool, this bias may be mitigated against over
the long run.

Datasets: None

See also: *_breakup_and_pair*.

*****
_rkpm
*****

This event runs the pair matching algorithm provided by FastSTI. For most
purposes it's good and flexible.

RKPM stands for Random-k Pair-Matching. It may also have been called a k-Nearest
Neighbour algorithm. It looks at the k agents adjacent (to the right if you
think of an array as a set of objects arranged left to right) of the current
agent in the mating pool, and selects the best match based on a distance
measure.

The value of k is determined by the *match_k* parameter. Its default value
is 100.

If k is set to 1 in the configuration input file, then in effect agents in the
mating pool are randomly matched with each other. This is a very fast way to
match agents but will not generate realistic matches.

If k is set to a large number greater than or equal to the possible number of
agents that will enter the mating pool (e.g. set it to 1,000,000,000 to ensure
it's bigger than any practically conceivable mating pool) , then in effect
agents in the mating pool are matched using a brute force algorithm, i.e. the
entire remainder of the mating pool is searched for a mate for the current
agent. This usually obtains a set of matches that resemble the population being
studied (assuming the distance measure is well implemented) but it can be very
slow.

The distance measure is written in C. It can be easily modified be redefining
the FSTI_AGENT_DISTANCE macro in fsti-userdefs.h and recompiling |PROJECT|.

This is how the default FSTI_AGENT_DISTANCE macro is defined:

.. code-block:: C
   :linenos:

    #ifndef FSTI_AGENT_DISTANCE
    #define FSTI_AGENT_DISTANCE(agent_a, agent_b)   \
         fsti_agent_default_distance(agent_a, agent_b)
    #endif

The fsti_agent_default_distance function is defined in fsti-agent.c file and is
very simple.

.. code-block:: C
   :linenos:

   float fsti_agent_default_distance(const struct fsti_agent *a,
                                     const struct fsti_agent *b)
   {
      float result = 0.0;
       if (a->sex_preferred != b->sex)
          result += 25.0;
       if (b->sex_preferred != a->sex)
          result += 25.0;
       result += abs(a->age - b->age);
       return result;
   }

It minimises the distance between ages of compatible sexual orientation and
similar age.

To define your own distance function, write a function to replace this in
fsti-userdefs.c and redefine FSTI_AGENT_DISTANCE to call it.

Datasets: None

*****************
_breakup_and_pair
*****************

This is a composite event that executes the following events in this order:

- _breakup
- _mating
- _shuffle_mating
- _rkpm

******************
_generate_and_pair
******************

This is a composite event that executes the following events in this order:

- _generate_agents
- _initial_mating
- _shuffle_mating
- _rkpm
- _initial_rel


TO DO

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
