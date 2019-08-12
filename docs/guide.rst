##############################
Tutorial: Creating simulations
##############################

When you create an agent-based model, there are generally three things to
consider: creating an initial population, agent behaviour during the simulation
and reporting.

******************
Initial population
******************

|PROJECT| allows you to generate the initial population or to read agents in
from a file.

The :ref:`before_events <before_events_ref>` parameter tells |PROJECT| how to
initialize the agents.


Generating the population
-------------------------

The easiest way to generate a population is to use the
:ref:`generate_and_pair_ref` event. E.g. in the simulation configuration file
there would be a line like this: ::

  before_events = _generate_and_pair; <other events to run before the simulation>

The _generate_and_pair event is a composite event which calls five other events:
:ref:`generate_agents_ref`, :ref:`initial_mating_ref`,
:ref:`shuffle_mating_ref`, :ref:`rkpm_ref` and :ref:`initial_rel_ref` events.

Together these events create the initial agents in the simulation, and place
some of them into relationships. Click on each of these events to see how they
work and how you can specify the number of agents, the ages of the agents, their
sexes, sexual preferences and the distribution of the relationships in the set
of initial agents.

Reading an agent file
---------------------

Reading in an agent file perhaps gives greater flexibility than the built-in
agent generating functionality of |PROJECT|.

The file would typically be created in a scripting language like R or Python or
even be hand-crafted.

The :ref:`read_agents_ref` event is used to read in the agents. In the
simulation configuration file there would be a line like this: ::

  before_events = _read_agents; <other events to run before the simulation>

Read the :ref:`section on agent input formats <agent_input_ref>` to find out how
to set up the agent input CSV file.

***************
Agent behaviour
***************

In |PROJECT| agent behaviour takes place in events. There are predefined events
that provide for agents to :ref:`age <age_ref>`, :ref:`form sexual relationships
with other agents and break-up with them <breakup_and_pair_ref>`, :ref:`become
infected <infect_ref>`, `start treatment <stage_ref>`, `change their infection
status <stage_ref>` (e.g. become sicker or become better), be :ref:`born
<birth_ref>`, and :ref:`die <death_ref>`.

Most of these events are usually executed using the :ref:`during_events
<during_events_ref>` parameter. Although it's conceivable you may also need to
execute agent behaviour before and after a simulation using the
:ref:`before_events <before_events_ref>` and :ref:`after_events
<after_events_ref>` parameters respectively.

If these events don't meet your needs the |PROJECT| framework allows you to
:ref:`code your own events <extending_ref>` in C.

*********
Reporting
*********

Usually, you need statistics about the simulation before, during and after the
simulation has run. |PROJECT| has a reporting facility to do write out
demographic and statistical information at any stage of a simulation.

Reports are written to a single output file in CSV format. If you're running
multiple simulations, the report lines from each simulation will be interleaved
with each other throughout the file. But each report line has enough information
for you to know which simulation produced it.

You'll usually need to use a spreadsheet program or a scripting language like R
or Python to process the output CSV file. See the scripts directory for examples
of report processing code that you make using |PROJECT| much easier for you.

Reporting is just another event in |PROJECT|. There are a few provided:
:ref:`report_ref` - which outputs statistics, :ref:`write_agents_csv_ref` -
which outputs every agent in the simulation, :ref:`write_living_agents_ref` -
which outputs every living agent in the simulation, and
:ref:`write_dead_agents_ref` - which outputs every dead agent in the
simulation. There are also events to write the headers for the output CSV files.

The :ref:`report_frequency <report_frequency_ref>` parameter specifies how
frequently to run these events during a simulation.

Use the events that output agents carefully. If you're executing
:ref:`write_agents_csv_ref` during the simulation, having a report_frequency of
1 will slow your simulation down and use huge amounts of hard drive space.

A typical report setup for a simulation looks something like this: ::

  results_file = results.csv
  agents_output_file = agents_out.csv
  report_frequency = 365
  before_events =  _write_agents_csv_header; _write_results_csv_header; <other
  events>; _report
  during_events = <other events>; _report;  _write_living_agents_csv
  after_events = <other events>; _report; write_agents_csv

If you don't need to know details about individual agents, then it's best not to
execute the events that output agents. Your simulation will be faster and use
much less disk space. Then you can also have much more frequent report
writing. E.g. ::

  results_file = results.csv
  report_frequency = 10
  before_events =   _write_results_csv_header; <other
  events>; _report
  during_events = <other events>; _report
  after_events = <other events>; _report
