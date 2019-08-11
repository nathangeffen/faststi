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

If you wish to record all the breakups, set the record_breakups parameter to 1
and the partnerships_file parameter to the name of the file to output to. But
note that the number of breakups in a large simulation can be huge.

Parameters: record_breakups, partnerships_file

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

If you wish to record all the matches in a file, set the record_matches
parameter to 1 and the partnerships_file parameter to the name of the file to
output to. But note that the number of partnerships in a large simulation can be
huge.

Parameters: record_matches, partnerships_file

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

*******
_infect
*******

Iterates the living agent and infects some of those in sero-discordant
relationships.

Whether an agent becomes infected depends on both its own characteristics and
those of its partner. See :ref:`two-agent-dataset-ref` for details.

If an agent becomes infected, the initial value of its infect property is set to
the initial_infect_stage parameter. The default is 2. Although the infection
stages are entirely user-defined, in the default settings of FastSTI, the
following default setup is assumed for the infect property:

- 0: The agent is uninfected (this should be the case for any simulation)
- 1: The agent is infected but on treatment
- 2: The agent is in a primary infection stage
- 3: The agent is in a chronic infection stage
- 4: The agent is sick or in an end-stage of infection

This can be specified differently. But you must make sure that your stages are
consistent across the simulation, else nonsensical things will happen. If you
want a different infection stage setup, make sure your datasets and
initial_infect_stage parameter are consistent with each other.

If you wish to record all the infections, set the record_infections parameter to
1 and the partnerships_file parameter to the name of the file to output to.

Parameters: record_infections, partnerships_file

Dataset: dataset_infect

******
_stage
******

Iterates over the living agents and changes the infection stage of some of the
infected agents. It can also change the agent's treatment and resistant properties.

How many stages there are for the infection is entirely user-defined, but you
have to make sure that the stages are consistent across events. Also, the
possible values for the treatment and and resistant agent properties are also
user-defined, but we think the default values FastSTI has been set up with are
sensible for many models.

Although the infection stages are entirely user-defined, in the default settings
of FastSTI, the following default setup is assumed for the infect property:

- 0: The agent is uninfected (this should be the case for any simulation)
- 1: The agent is infected but on treatment
- 2: The agent is in a primary infection stage
- 3: The agent is in a chronic infection stage
- 4: The agent is sick or in an end-stage of infection

This can be specified differently. But you must make sure that your stages are
consistent across the simulation, else nonsensical things will happen. If you
want a different infection stage setup, make sure your datasets and
initial_infect_stage parameter are consistent with each other.


Dataset: dataset_infect_stage

This is quite a complicated dataset and is best understood by looking at the
commented example in the data directory called dataset_infect_stage.csv. For
convenience here it is:

.. code-block:: none
   :linenos:

        # Dataset used by _stage event (defined in fsti-events.c as fsti_event_stage)
        #
        # The first three columns are used for matching and correspond to agent fields
        # or properties.
        #
        # The next six columns are instructions on how and when to change the stage.
        #
        # Columns:
        #
        #  1. infected - the infection stage of the agent (0 is uninfected)
        #  2. treated - the treatment regimen of the agent. This particular file
        #               allows for 3 treatment regimens.
        #  3. resistant - 0 if the agent is drug-susceptible to this treatment regimen
        #                 1 if the agent is drug-resistant to this treatment regimen
        #  4. prob_stage_change - probability that the infection stage changes for
        #                         this time step (1 day)
        #  5. stage_incr - if a uniformly generated random number < prob_stage_change
        #                  then change the infect property by this increment
        #  6. prob_treatment_change - probability treatment changes
        #  7. treatment_incr - if a uniformly rand number < prob_treatment_change
        #                      then change the treatment property by this increment
        #  8. prob_resistant - probability resistance status changes
        #  9. resistant_incr - if a uniformly random number < prob_resistant_change
        #                      change the resistant value by this amount
        #
        # Note the |6 after resistant_incr means that there are six columns at the end
        # of each line that are not agent properties.
        #
        # Infection stages:;;;;;;;;
        #  0 = uninfected;;;;;;;;
        #  1 = virally suppressed (usually on treatment);;;;;;;;
        #  2 = primary infection (highly infectious);;;;;;;;
        #  3 = chronic infection;;;;;;;;
        #  4 = Final stage;;;;;;;;
        #
        # HEADER ROW FOLLOWS
        infected;treated;resistant;prob_stage_change;stage_incr;prob_treatment_change;treatment_incr;prob_resistant;resistant_incr|6
        0;0;0;0;0;0;0;0;0
        0;0;1;0;0;0;0;0;0
        0;1;0;0;0;0;0;0;0
        0;1;1;0;0;0;0;0;0
        0;2;0;0;0;0;0;0;0
        0;2;1;0;0;0;0;0;0
        0;3;0;0;0;0;0;0;0
        0;3;1;0;0;0;0;0;0
        1;0;0;0.1;1;0.0001;1;0;0
        1;0;1;0.1;1;0.0001;1;0;0
        1;1;0;0.00001;1;0.00001;1;0.00001;1
        1;1;1;0.1;1;0.0001;1;0;0
        1;2;0;0.00001;1;0.00001;1;0.00001;1
        1;2;1;0.1;1;0.0001;1;0;0
        1;3;0;0.00001;1;0;0;0.0001;1
        1;3;1;0.1;1;0;0;0;0
        2;0;0;0.1;1;0.001;1;0;0
        2;0;1;0.1;1;0.001;1;0;0
        2;1;0;0.1;-1;0;0;0.0001;1
        2;1;1;0.1;1;0.001;1;0;0
        2;2;0;0.1;-1;0;0;0.0001;1
        2;2;1;0.1;1;0.001;1;0;0
        2;3;0;0.1;-1;0;0;0.0001;1
        2;3;1;0.1;1;0;0;0;0
        3;0;0;0.004;1;0.0001;1;0;0
        3;0;1;0.001;1;0.0001;1;0;0
        3;1;0;0.1;-1;0;0;0.0001;1
        3;1;1;0.002;1;0.005;1;0;0
        3;2;0;0.1;-1;0;0;0.0001;1
        3;2;1;0.002;1;0.001;1;0;0
        3;3;0;0.1;-1;0;0;0.0001;1
        3;3;1;0.002;1;0;0;0;0
        4;0;0;0;0;0.005;1;0;0
        4;0;1;0;0;0.005;1;0;0
        4;1;0;0.1;-1;0;0;0.0001;1
        4;1;1;0;0;0.01;1;0;0
        4;2;0;0.1;-1;0;0;0.0001;1
        4;2;1;0;0;0.01;1;0;0
        4;3;0;0.1;-1;0;0;0.0001;1
        4;3;1;0;0;0;0;0;0

*********
_coinfect
*********

This is a very simple event that iterates over the living agents and sets the
agent coinfected property to 1 for some agents. Users who want to model more
sophisticated coinfection (e.g. TB for HIV) will likely have to write their own
coinfection event.

Dataset: dataset_coinfect

******
_birth
******

Creates new agents with their ages set to the age_min property. (So if the
minimum age of the simulation is, say, 15, then agents are not born as infants
but as instant adolescents.)

Unless the simulation population is huge, creating agents daily (assuming the
time_step is set to a day) makes no sense. For example consider a population of
10,000 agents with a daily time step over 20 years. On any given day, a sensible
continuous random distribution of births will generate a fraction of births. But
FastSTI is a discrete simulation framework and there is no such thing as a
fractional agent. So instead the *birth_event_every_n* parameter must be set to
how frequently the _birth event should be executed.

The event only creates agents every n\ :sub:`th` time step.  The *birth_rate*
parameter is set to the birth rate and the GNU Scientific Library's
gsl_ran_poisson function is used to determine the number of births.

Besides age, the default implementation sets the following agent properties:
sex, sex_preferred, infected, treated and resistant.

- sex is set, with the proportion of males determined by the prob_birth_male
  parameter.

- sex_preferred is set using the prob_birth_msw (where msw stands for men who
  sex with women), prob_birth_wsm (where wsm stands for women who have sex with
  men) parameters. If the agent's sex is male, the prob_birth_msw value is used
  to determine the probability that the agent's preferred sexual partner is a
  female. Likewise if the agent's sex is female the prob_birth_wsm value is used
  to determine the probability that the agent's preferred sexual partner is a
  male.

- infected (i.e. the infection stage of the agent with 0 by convention meaning
  the agent isn't infected) is set via the dataset dataset_birth_infect. (See
  data/dataset_birth_infect.csv for an example.)

- treated (i.e. which treatment line, if any, the agent is on) is set via the
  dataset dataset_birth_treated. (See data/dataset_birth_treated.csv for an
  example.)

- resistant (i.e. which treatment regimens the agent is resistant to, if any) is
  set via the dataset dataset_birth_resistant (See
  data/dataset_birth_resistant.csv for an example.)

If you wish to set additional agent properties, you must provide hook in by
defining a macro called FSTI_HOOK_BIRTH_AGENT in fsti-userdefs.h. The macro
takes two parameters: simulation (a pointer to the current simulation) and
agent (a pointer to the current agent whose property you wish to set).

Parameters: birth_event_every_n, prob_birth_male, prob_birth_msw, prob_birth_wsm,

Datasets: dataset_birth_infect, dataset_birth_treated and dataset_birth_resistant

*******
_report
*******

Reports statistics on the current state of the simulation.

This event can, and typically is, executed before, during and after a simulation.
If it is executed during the simulation, the report_frequency parameter is used
to determine how often. E.g. if set to 365, it will execute approximately once a
year.

In addition to the statistics the event executes (defined in FSTI_REPORT in
fsti-defaults.h which can be redefined in fsti-userdefs.h) you can define
additional statistics using the FSTI_HOOK_REPORT macro.

The results_file parameter determines the name of the output file. By default
this is left blank which means results are written to standard output.

Note that if multiple simulations are run in parallel, the output will be
interleaved. At the end of the simulation you can simply sort these into the
right order using the first three fields which are the simulation name, id and
date of the report. You can use nearly any data manipulation tool to do this
including R, Python, a spreadsheet program, or standard Posix utilities such as
sort and awk.

Parameters: report_frequency

Datasets: None

*************************
_write_results_csv_header
*************************

Simply writes a header for the results csv file. You would only place this in
the before_events parameter. It's clever enough to figure out that it should
only write itself once to a results file.

Parameters: None

Datasets: None

*****************
_write_agents_csv
*****************

Writes the current state of every agent to an agent output csv file.

This event can be executed before, during and after a simulation.  If it is
executed during the simulation, the report_frequency parameter is used to
determine how often. E.g. if set to 365, it will execute approximately once a
year.

How an agent is written is determined by the FSTI_AGENT_PRINT_CSV macro. To
write out agents differently redefine this macro in fsti-userdefs.h.

The agents_output_file parameter determines the name of the output file. By
default this is left blank which means agents are written to standard output.

Note that if multiple simulations are run in parallel, the output will be
interleaved. At the end of the simulation you can simply sort these into the
right order using the first four fields which are the simulation name, id of the
simulation, current date of the simulation and agent id. You can use nearly any
data manipulation tool to do this including R, Python, a spreadsheet program, or
standard Posix utilities such as sort and awk.

Also note that this file can get very large. If you have a million agents and
you are running 100 simulations, it will write 100 million lines every time it
is executed before, during and after each simulation. You could quickly run out
of disk space. All this output also slows down simulations, so use this event sparingly.

Parameters: report_frequency

Datasets: None

************************
_write_agents_csv_header
************************

Simply writes a header for the agents csv file. You would only place this in
the before_events parameter. It's clever enough to figure out that it should
only write itself once to an agent output file.

Parameters: None

Datasets: None

************************
_write_living_agents_csv
************************

Exactly like _write_agents but only writes the living agents.

Parameters: report_frequency

Datasets: None

**********************
_write_dead_agents_csv
**********************

Exactly like _write_agents but only writes the dead agents.

Parameters: report_frequency

Datasets: None


******************************
_write_partnerships_csv_header
******************************

If the record_matches, record_breakups or record_infections parameters are set
to 1, then it may be useful to write a header for the output csv file.

Parameters: record_matches, record_breakups, record_infections,
partnerships_file

Datasets: None
