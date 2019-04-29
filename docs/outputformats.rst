###################
Output file formats
###################

***************
Results reports
***************

Whenever the *_report* is executed, it writes output in CSV format either to the
standard output device (default) or an output file as specified by the
*results_file* key in the configuration. This output is meant to be
post-processed by you either in a spreadsheet, or using a scripting language
like R or Python.

.. note:: The *_report* event prints out quite basic information. If you feel
          comfortable writing C code, you can enhance it in the fsti_userdefs.h
          file by defining the *FSTI_HOOK_REPORT* macro. See the *FSTI_REPORT*
          macro in fsti-defaults.h.

Here is an example output from a set of simulations:

.. code-block:: none
   :linenos:

      name;sim;num;date;description;value
      Simulation_0;0;0;2018-01-01;MIN_AGE_ALIVE;15.000000
      Simulation_0;0;0;2018-01-01;MAX_AGE_ALIVE;49.000000
      Simulation_0;0;0;2018-01-01;MEAN_AGE_ALIVE;23.000000
      Simulation_0;0;0;2018-01-01;MEDIAN_AGE_ALIVE;18.000000
      Simulation_0;0;0;2018-01-01;INFECT_RATE_ALIVE;0.077900
      Simulation_0;0;0;2018-01-01;POP_ALIVE;20000
      Simulation_0;0;0;2018-01-01;NUM_PARTNERS;3573
      Simulation_0;0;0;2018-01-01;MIN_AGE_DEAD;nan
      Simulation_0;0;0;2018-01-01;MAX_AGE_DEAD;nan
      Simulation_0;0;0;2018-01-01;MEAN_AGE_DEAD;nan
      Simulation_0;0;0;2018-01-01;INFECT_RATE_DEAD;-nan
      Simulation_0;0;0;2018-01-01;POP_DEAD;0
      Simulation_0;0;0;2018-01-01;INITIAL_INFECTIONS;1558
      Simulation_0;0;0;2018-01-01;SIMULATION_INFECTIONS;0
      Simulation_0;0;0;2018-01-01;INITIAL_MATCHES;3573
      Simulation_0;0;0;2018-01-01;SIMULATION_MATCHES;0
      Simulation_0;0;0;2018-01-01;BREAKUPS;0
      Simulation_0;0;0;2018-01-01;TIME_TAKEN;1
      Simulation_0;0;0;2028-01-01;MIN_AGE_ALIVE;15.000000
      Simulation_0;0;0;2028-01-01;MAX_AGE_ALIVE;59.000000
      Simulation_0;0;0;2028-01-01;MEAN_AGE_ALIVE;30.000000
      Simulation_0;0;0;2028-01-01;MEDIAN_AGE_ALIVE;26.000000
      Simulation_0;0;0;2028-01-01;INFECT_RATE_ALIVE;0.142421
      Simulation_0;0;0;2028-01-01;POP_ALIVE;23592
      Simulation_0;0;0;2028-01-01;NUM_PARTNERS;8442
      Simulation_0;0;0;2028-01-01;MIN_AGE_DEAD;15.000000
      Simulation_0;0;0;2028-01-01;MAX_AGE_DEAD;58.000000
      Simulation_0;0;0;2028-01-01;MEAN_AGE_DEAD;31.000000
      Simulation_0;0;0;2028-01-01;INFECT_RATE_DEAD;0.738192
      Simulation_0;0;0;2028-01-01;POP_DEAD;741
      Simulation_0;0;0;2028-01-01;INITIAL_INFECTIONS;1558
      Simulation_0;0;0;2028-01-01;SIMULATION_INFECTIONS;2349
      Simulation_0;0;0;2028-01-01;INITIAL_MATCHES;3573
      Simulation_0;0;0;2028-01-01;SIMULATION_MATCHES;145574
      Simulation_0;0;0;2028-01-01;BREAKUPS;140193
      Simulation_0;0;0;2028-01-01;TIME_TAKEN;6
      Simulation_0;1;1;2018-01-01;MIN_AGE_ALIVE;15.000000
      Simulation_0;1;1;2018-01-01;MAX_AGE_ALIVE;49.000000
      Simulation_0;1;1;2018-01-01;MEAN_AGE_ALIVE;23.000000
      Simulation_0;1;1;2018-01-01;MEDIAN_AGE_ALIVE;18.000000
      Simulation_0;1;1;2018-01-01;INFECT_RATE_ALIVE;0.080300
      Simulation_0;1;1;2018-01-01;POP_ALIVE;20000
      Simulation_0;1;1;2018-01-01;NUM_PARTNERS;3601
      Simulation_0;1;1;2018-01-01;MIN_AGE_DEAD;nan
      Simulation_0;1;1;2018-01-01;MAX_AGE_DEAD;nan
      Simulation_0;1;1;2018-01-01;MEAN_AGE_DEAD;nan
      Simulation_0;1;1;2018-01-01;INFECT_RATE_DEAD;-nan
      Simulation_0;1;1;2018-01-01;POP_DEAD;0
      Simulation_0;1;1;2018-01-01;INITIAL_INFECTIONS;1606
      Simulation_0;1;1;2018-01-01;SIMULATION_INFECTIONS;0
      Simulation_0;1;1;2018-01-01;INITIAL_MATCHES;3601
      Simulation_0;1;1;2018-01-01;SIMULATION_MATCHES;0
      Simulation_0;1;1;2018-01-01;BREAKUPS;0
      Simulation_0;1;1;2018-01-01;TIME_TAKEN;6
      Simulation_0;1;1;2028-01-01;MIN_AGE_ALIVE;15.000000
      Simulation_0;1;1;2028-01-01;MAX_AGE_ALIVE;59.000000
      Simulation_0;1;1;2028-01-01;MEAN_AGE_ALIVE;30.000000
      Simulation_0;1;1;2028-01-01;MEDIAN_AGE_ALIVE;26.000000
      Simulation_0;1;1;2028-01-01;INFECT_RATE_ALIVE;0.142754
      Simulation_0;1;1;2028-01-01;POP_ALIVE;23516
      Simulation_0;1;1;2028-01-01;NUM_PARTNERS;8432
      Simulation_0;1;1;2028-01-01;MIN_AGE_DEAD;15.000000
      Simulation_0;1;1;2028-01-01;MAX_AGE_DEAD;55.000000
      Simulation_0;1;1;2028-01-01;MEAN_AGE_DEAD;31.000000
      Simulation_0;1;1;2028-01-01;INFECT_RATE_DEAD;0.768553
      Simulation_0;1;1;2028-01-01;POP_DEAD;795
      Simulation_0;1;1;2028-01-01;INITIAL_INFECTIONS;1606
      Simulation_0;1;1;2028-01-01;SIMULATION_INFECTIONS;2362
      Simulation_0;1;1;2028-01-01;INITIAL_MATCHES;3601
      Simulation_0;1;1;2028-01-01;SIMULATION_MATCHES;146416
      Simulation_0;1;1;2028-01-01;BREAKUPS;141010
      Simulation_0;1;1;2028-01-01;TIME_TAKEN;11
      Simulation_1;2;0;2018-01-01;MIN_AGE_ALIVE;15.000000
      Simulation_1;2;0;2018-01-01;MAX_AGE_ALIVE;49.000000
      Simulation_1;2;0;2018-01-01;MEAN_AGE_ALIVE;23.000000
      Simulation_1;2;0;2018-01-01;MEDIAN_AGE_ALIVE;18.000000
      Simulation_1;2;0;2018-01-01;INFECT_RATE_ALIVE;0.080600
      Simulation_1;2;0;2018-01-01;POP_ALIVE;20000
      Simulation_1;2;0;2018-01-01;NUM_PARTNERS;3569
      Simulation_1;2;0;2018-01-01;MIN_AGE_DEAD;nan
      Simulation_1;2;0;2018-01-01;MAX_AGE_DEAD;nan
      Simulation_1;2;0;2018-01-01;MEAN_AGE_DEAD;nan
      Simulation_1;2;0;2018-01-01;INFECT_RATE_DEAD;-nan
      Simulation_1;2;0;2018-01-01;POP_DEAD;0
      Simulation_1;2;0;2018-01-01;INITIAL_INFECTIONS;1612
      Simulation_1;2;0;2018-01-01;SIMULATION_INFECTIONS;0
      Simulation_1;2;0;2018-01-01;INITIAL_MATCHES;3569
      Simulation_1;2;0;2018-01-01;SIMULATION_MATCHES;0
      Simulation_1;2;0;2018-01-01;BREAKUPS;0
      Simulation_1;2;0;2018-01-01;TIME_TAKEN;5
      Simulation_1;2;0;2028-01-01;MIN_AGE_ALIVE;15.000000
      Simulation_1;2;0;2028-01-01;MAX_AGE_ALIVE;59.000000
      Simulation_1;2;0;2028-01-01;MEAN_AGE_ALIVE;30.000000
      Simulation_1;2;0;2028-01-01;MEDIAN_AGE_ALIVE;26.000000
      Simulation_1;2;0;2028-01-01;INFECT_RATE_ALIVE;0.136983
      Simulation_1;2;0;2028-01-01;POP_ALIVE;23638
      Simulation_1;2;0;2028-01-01;NUM_PARTNERS;8423
      Simulation_1;2;0;2028-01-01;MIN_AGE_DEAD;15.000000
      Simulation_1;2;0;2028-01-01;MAX_AGE_DEAD;59.000000
      Simulation_1;2;0;2028-01-01;MEAN_AGE_DEAD;32.000000
      Simulation_1;2;0;2028-01-01;INFECT_RATE_DEAD;0.717507
      Simulation_1;2;0;2028-01-01;POP_DEAD;754
      Simulation_1;2;0;2028-01-01;INITIAL_INFECTIONS;1612
      Simulation_1;2;0;2028-01-01;SIMULATION_INFECTIONS;2167
      Simulation_1;2;0;2028-01-01;INITIAL_MATCHES;3569
      Simulation_1;2;0;2028-01-01;SIMULATION_MATCHES;145976
      Simulation_1;2;0;2028-01-01;BREAKUPS;140568
      Simulation_1;2;0;2028-01-01;TIME_TAKEN;10
      Simulation_1;3;1;2018-01-01;MIN_AGE_ALIVE;15.000000
      Simulation_1;3;1;2018-01-01;MAX_AGE_ALIVE;49.000000
      Simulation_1;3;1;2018-01-01;MEAN_AGE_ALIVE;23.000000
      Simulation_1;3;1;2018-01-01;MEDIAN_AGE_ALIVE;18.000000
      Simulation_1;3;1;2018-01-01;INFECT_RATE_ALIVE;0.083850
      Simulation_1;3;1;2018-01-01;POP_ALIVE;20000
      Simulation_1;3;1;2018-01-01;NUM_PARTNERS;3550
      Simulation_1;3;1;2018-01-01;MIN_AGE_DEAD;nan
      Simulation_1;3;1;2018-01-01;MAX_AGE_DEAD;nan
      Simulation_1;3;1;2018-01-01;MEAN_AGE_DEAD;nan
      Simulation_1;3;1;2018-01-01;INFECT_RATE_DEAD;-nan
      Simulation_1;3;1;2018-01-01;POP_DEAD;0
      Simulation_1;3;1;2018-01-01;INITIAL_INFECTIONS;1677
      Simulation_1;3;1;2018-01-01;SIMULATION_INFECTIONS;0
      Simulation_1;3;1;2018-01-01;INITIAL_MATCHES;3550
      Simulation_1;3;1;2018-01-01;SIMULATION_MATCHES;0
      Simulation_1;3;1;2018-01-01;BREAKUPS;0
      Simulation_1;3;1;2018-01-01;TIME_TAKEN;5
      Simulation_1;3;1;2028-01-01;MIN_AGE_ALIVE;15.000000
      Simulation_1;3;1;2028-01-01;MAX_AGE_ALIVE;59.000000
      Simulation_1;3;1;2028-01-01;MEAN_AGE_ALIVE;30.000000
      Simulation_1;3;1;2028-01-01;MEDIAN_AGE_ALIVE;26.000000
      Simulation_1;3;1;2028-01-01;INFECT_RATE_ALIVE;0.139352
      Simulation_1;3;1;2028-01-01;POP_ALIVE;23595
      Simulation_1;3;1;2028-01-01;NUM_PARTNERS;8456
      Simulation_1;3;1;2028-01-01;MIN_AGE_DEAD;15.000000
      Simulation_1;3;1;2028-01-01;MAX_AGE_DEAD;58.000000
      Simulation_1;3;1;2028-01-01;MEAN_AGE_DEAD;31.000000
      Simulation_1;3;1;2028-01-01;INFECT_RATE_DEAD;0.728205
      Simulation_1;3;1;2028-01-01;POP_DEAD;780
      Simulation_1;3;1;2028-01-01;INITIAL_INFECTIONS;1677
      Simulation_1;3;1;2028-01-01;SIMULATION_INFECTIONS;2179
      Simulation_1;3;1;2028-01-01;INITIAL_MATCHES;3550
      Simulation_1;3;1;2028-01-01;SIMULATION_MATCHES;146061
      Simulation_1;3;1;2028-01-01;BREAKUPS;140609
      Simulation_1;3;1;2028-01-01;TIME_TAKEN;10

- The first column (name) is the simulation group name as specified in the configuration
  file.
- The second column (sim) is the unique simulation number. In every execution of
  FastSTI, each simulation will have its own unique id, starting from 0.
- The third column (num) is the number of the simulation within the simulation
  group. Here each of the two simulation groups (Simulation_0 and Simulation_1)
  executed two simulations each, numbered 0 and 1.
- The fourth column is the date within the simulation that this snapshot report
  is for. In these simulations, the start date was 1 January 2018, and the end
  date was 1 January 2028. The *_report* event ran before and after the
  simulation (it can run as often as you like during the simulation as well).
- The fifth column is the description of what's being reported. These are
  described below.
- The sixth column is the value of what's being reported. E.g. 10 for TIME_TAKEN
  means the simulation ran in 10 seconds up to that point.

.. warning::
   The output in the example above is all in chronological order. This is
   because it was a single-threaded sequential execution. Normally you will do a
   multithreaded execution and the report lines will likely **not** come out in
   order. When analysing them in a spreadsheet, R or Python, remember to sort
   the output by the first four columns.


What the descriptions mean
~~~~~~~~~~~~~~~~~~~~~~~~~~

MIN_AGE_ALIVE
  is the youngest age, in years, of a currently living agent

MAX_AGE_ALIVE
  is the oldest age, in years of a currently living agent

MEAN_AGE_ALIVE
  is the mean age, in years, of the living population

MEDIAN_AGE_ALIVE
  is the median age, in years, of the living population

INFECT_RATE_ALIVE
  is the infection rate of the living population

POP_ALIVE
  is the number of living agents

NUM_PARTNERS
  is the number of agents in relationships

MIN_AGE_DEAD
  is the age of the youngest agent who has died

MAX_AGE_DEAD
  is the age of the oldest agent who has died

INFECT_RATE_DEAD
  is the infection rate in the dead population

POP_DEAD
  is the number of dead agents

INITIAL_INFECTIONS
  is the number of infected agents at the start of the simulation

SIMULATION_INFECTIONS
  is the number of agents who became infected during the simulation

INITIAL_MATCHES
  is the number of pairs of agents in relationships at the start of the
  simulation

SIMULATION_MATCHES
  is the number of pairs of agents made during the simulation

BREAKUPS
  is the number of pairs that broke up during the simulation

TIME_TAKEN
  is the time taken by the simulation in seconds

************
Agent output
************

The *_write_agents_csv_header* prints out the header line in an output agent csv
file. The *_write_agents_csv* event actually writes out all the agents (living
and dead), and the *_write_living_agents_csv* and *_write_dead_agents_csv* write
the living and dead agents respectively.

Here is an extract from sample output of the *_write_agents_csv_header* and
*_write_agents_csv* events.

.. code-block:: none
   :linenos:

      sim;date;id;age;sex;sex_preferred;infected;treated;resistant;date_death;partner;change_date
      1;2028-01-01;1547;25;0;1;0;0;0;0000-00-00;515;2035-10-24
      1;2028-01-01;1548;41;0;1;4;0;0;2023-10-11;-1;2025-10-30
      1;2028-01-01;1549;28;0;1;0;0;0;0000-00-00;6620;2028-04-22
      1;2028-01-01;1550;25;1;0;0;0;0;0000-00-00;3910;2029-09-04
      1;2028-01-01;1551;31;1;0;1;1;0;0000-00-00;22910;2028-01-13
      1;2028-01-01;1552;30;1;0;0;0;0;0000-00-00;18055;2028-01-03
      1;2028-01-01;1553;36;1;0;0;0;0;0000-00-00;7448;2028-10-14
      1;2028-01-01;1554;33;1;0;0;0;0;0000-00-00;694;2028-04-25
      1;2028-01-01;1555;28;0;0;0;0;0;0000-00-00;984;2028-03-23
      1;2028-01-01;1556;28;0;1;0;0;0;0000-00-00;4467;2030-03-27
      1;2028-01-01;1557;45;0;1;0;0;0;0000-00-00;15614;2035-01-22
      1;2028-01-01;1558;27;1;0;0;0;0;0000-00-00;3367;2030-02-17
      1;2028-01-01;1559;29;0;1;0;0;0;0000-00-00;10884;2030-01-10
      1;2028-01-01;1560;52;1;0;0;0;0;0000-00-00;19043;2028-05-24
      1;2028-01-01;1561;58;1;0;0;0;0;0000-00-00;13638;2028-07-01
      1;2028-01-01;1562;30;1;0;0;0;0;0000-00-00;14842;2030-02-19
      1;2028-01-01;1563;57;0;1;0;0;0;0000-00-00;17535;2029-03-13
      1;2028-01-01;1564;37;1;0;1;1;1;0000-00-00;15586;2031-10-26
      1;2028-01-01;1565;29;1;0;0;4;0;2026-11-30;-1;2027-08-30
      1;2028-01-01;1566;46;1;0;0;0;0;0000-00-00;-1;2030-03-21
      1;2028-01-01;1567;26;1;0;0;0;0;0000-00-00;16097;2029-03-04

The first column is the simulation number. The second column is the current
simulation date. The third column is the unique agent id. The fourth to ninth
columns are the agent's sex, preferred sex, infection stage, treatment regimen,
and resistance status respectively. The tenth column is 0000-00-00 if the agent
is alive, else the date the agent died. The eleventh column is the unique id of
the agent's partner (-1 if the agent is single). The last column is the date the
agent's relationship status will change, either to single if it's in a
relationship, or to a relationship if it's single.

.. note:: FastSTI's agent output event assumes an agent has at most one
          partner. You can extend it if you're implementing concurrency.

.. note:: If you feel comfortable writing C code, you can modify or extend the
          output of the *_write_agents_csv*, *_write_living_agents_csv* and
          *_write_dead_agents_csv* events by redefining the
          FSTI_AGENT_PRINT_CSV_HEADER and FSTI_AGENT_PRINT_CSV macros. Put your
          redefinition in *src/fsti-userdefs.h*. The default implementation is in
          *src/fsti-defaults.h*.
