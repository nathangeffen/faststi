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

To do


_age

_death

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
