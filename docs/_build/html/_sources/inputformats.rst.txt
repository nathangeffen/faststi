##################
Input file formats
##################

*************
Configuration
*************

The configuration file, which specifies the simulations, is in Windows ini
format. Here is an example:

.. code-block:: ini
   :linenos:

      # Faststi test configuration file

      [Simulation 0]

      num_simulations=1
      num_agents=10000
      simulation_period=10 YEARS

      agents_output_file=agents_out.csv
      results_file=results.csv

      dataset_gen_sex=dataset_gen_sex.csv
      dataset_gen_sex_preferred=dataset_gen_sex_preferred.csv
      dataset_gen_infect=dataset_gen_infect.csv
      dataset_gen_treated=dataset_gen_treated.csv
      dataset_gen_resistant=dataset_gen_resistant.csv
      dataset_gen_mating=dataset_gen_mating.csv

      dataset_birth_infect=dataset_gen_infect.csv
      dataset_birth_treated=dataset_birth_treated.csv
      dataset_birth_resistant=dataset_birth_resistant.csv

      dataset_rel_period=dataset_rel.csv
      dataset_single_period=dataset_single.csv
      dataset_infect=dataset_infect.csv
      dataset_infect_stage=dataset_infect_stage.csv
      dataset_mortality=dataset_mortality_simple.csv

      before_events=_write_agents_csv_header;_generate_and_pair;_report;_write_agents_csv
      during_events=_age;_breakup_and_pair;_infect;_stage;_birth;_death
      after_events=_write_agents_csv;_report

      match_k=100
      threads=1

      [Simulation 1]

      dataset_mortality=dataset_mortality_complicated.csv
      match_k=300

- Comment lines begin with a #.
- A # in the middle of a line also denotes a comment until the end of the line.
- A simulation group name is enclosed in square brackets *[]*.
- Within a simulation group, parameters are specified as key-vale pairs. A key is
  separated from its value by an equals sign. White space before and after the =
  sign is ignored.
- Some parameters (e.g. *during_events* take multiple values. Each value must be
  separated by a semi-colon *;*. White space before or after the semi-colon is
  ignored.
- Blank lines are ignored.
- Each key must be a predefined parameter. If you use a key that isn't a
  predefined parameter, the |PROJECT| will give an error message and terminate. To
  see all the parameters, run: ::

    .src/faststi -p


- Key-values set in a simulation group are carried through to subsequent groups,
  unless they are specified again in the new group. E,g, in the example above,
  *dataset_mortality* and *match_k* are specified again for the simulation group
  called *Simulation 1*. All the other parameters are identical to *Simulation
  0*.

Besides setting parameters in the input configuration file, you can modify them
on the command line using this format: ::

  ./src/faststi -c=<value>[;<value>]* -f <filename>

.. _dataset-format:

********
Datasets
********

Many events depend on datasets. The format of the dataset file is a little
cumbersome but it is designed for a combination of speed and safety.

Datasets can either be placed in the *data* directory, or in the directory in
which the simulation is being run. Alternately, the *FSTI_DATA* environment
variable can be set to the location where the datasets are located.

Datasets are csv files. The default delimiter is a semi-colon, not a comma. You
can change this by setting the csv_delimiter

A standard dataset consists of 0 or more columns representing the values of
agent properties, followed by 1 or more columns representing probabilities or
other derived values. Events typically match the properties of the current agent
being operated upon to the corresponding row in the dataset in order to obtain
the appropriate probability of the event occurring. Sometimes there

Let's start with the very simplest of the supplied datasets. It is
*dataset_gen_sex.csv* and it is located in the *data* directory. It is used by
the *_generate_agents* event to initialize the sex of agents at the beginning of
simulations.  Here it is:

.. code-block:: ini
   :linenos:

      Probability
      0.5

Line 1 is simply the CSV header. It is called *Probability* here but we could
have named it anything. Line 2 is 0.5, the odds of being male. The event
uses this to set approximately half the agents to male and half to female when
it generates agents.


Here's a more typical dataset, *dataset_gen_infect.csv*, also used by the
*_generate_agents* event to determine the infection stage, if any, of agents
when they are initialized.


.. code-block:: none
   :linenos:

      sex;sex_preferred;age|10-YEAR;1;2;3;4|4
      0;0;0;0;0;0;0
      0;0;1;0;0;0;0
      0;0;2;0.1;0.2;0.3;0.4
      0;0;3;0.1;0.2;0.3;0.4
      0;0;4;0.05;0.1;0.15;0.2
      0;0;5;0.025;0.05;0.075;0.1
      0;1;0;0;0;0;0
      0;1;1;0;0;0;0
      0;1;2;0.05;0.1;0.15;0.2
      0;1;3;0.05;0.1;0.15;0.2
      0;1;4;0.025;0.05;0.075;0.1
      0;1;5;0.0125;0.025;0.0375;0.05
      1;0;0;0;0;0;0
      1;0;1;0;0;0;0
      1;0;2;0.05;0.1;0.15;0.2
      1;0;3;0.05;0.1;0.15;0.2
      1;0;4;0.025;0.05;0.075;0.1
      1;0;5;0.0125;0.025;0.0375;0.05
      1;1;0;0;0;0;0
      1;1;1;0;0;0;0
      1;1;2;0.05;0.1;0.15;0.2
      1;1;3;0.05;0.1;0.15;0.2
      1;1;4;0.025;0.05;0.075;0.1
      1;1;5;0.0125;0.025;0.0375;0.05


In the example HIV model provided, there are five possible values for
*infection*:

- 0 = uninfected
- 1 = virally suppressed (usually on treatment)
- 2 = primary infection (highly infectious)
- 3 = chronic infection (usually lasts several years)
- 4 = Final stage (AIDS)

Let's start with the header (line 1). The last column is *4|4*. The first "4" is
simply the name of the column (representing stage 4 infection), and could have
been called anything. But the "4" after the pipe (|) tells |PROJECT| that the last
four columns all represent probabilities. If a dataset contains more than one
probability column then this must be specified. |PROJECT| then knows that the
first three fields, *sex*, *sex_preferred* and *age* are not probability
columns, and correspond precisely to the names of fields in the fsti_agent data
structure. If they didn't, |PROJECT| would terminate with an error.

The dataset needs an entry (or row) for each combination of sex, sex_preferred
and age. Also the first row of every dataset after the header must start with
every property set to 0, and then cycle incrementally through all combinations
of possible values for the properties. This may sound tiresome, but it ensures
that probabilities can be looked up using a random access search, rather than
having to sequentially search the table.

There is one important short-cut. Notice the column headed "age|10-YEAR". The
pipe followed by either an integer or a time period, tells |PROJECT| to divide the
agent's age by this number, in this case 10 years, in order to get the value to
search for in the dataset. So an agent with age 45 will have its age divided by
10 which gives it a lookup value for its age of 4 (the .5 is dropped - this is
integer division).

What about an agent whose age is 60 or more (because the ages run from 0 to 5)? The
dataset lookup algorithm assumes any agent property greater than the largest
value is equal to the largest value.

Consider an agent who is male, prefers to have sex with females and is 31 years
old. What is the probability they are HIV-positive (in this dataset)? And if
HIV-positive, what infection stage is he likely to be in?

The agent matches line 11, which corresponds to male agents (first column with a
value of 0) whose preferred sexual partner is female (second column with a value
of 1) and agents aged 30 to 39 (third column with a value of 3, i.e. 31 / 3).

To determine if the agent is infected with HIV, the *_generate_agents* event
samples a uniform random number, *r*.

- If *r* is less than 0.05 (the value in column 4 of line 11), the agent is in stage 1.
- If *r* is less than 0.1 (the value in column 5 of line 11), the agent is in stage 2.
- If *r* is less than 0.15 (the value in column 6 of line 11) the agent is in
  stage 3.
- If *r* is less than 0.2 (the value in column 7 of line 11) the agent is in
  stage 4.
- Else if *r* is greater than or equal to 0.2, the agent is uninfected.

With most events, the agent characteristics you use are up to you. You could
create a dataset for generating the initial infection status of agents that
doesn't take into account *sex_preferred* or *age*. Alternately, you could add a
*coinfection* column (because there is a field called coinfection in the |PROJECT|
agent structure), and make the infection probabilities dependent on that.

There is somewhat less flexibility with the probability fields. These are
event-specific. As it happens the code that sets the infection stage expects one
or more user-defined stages, so you can specify fewer or more than the four
stages in the above example.

.. _two-agent-dataset-ref:

Two-agent datasets
~~~~~~~~~~~~~~~~~~

Some events need to make a decision based on two agents. In modelling sexually
transmitted infections, the most obvious example is an event that determines if
an agent becomes infected. |PROJECT|'s supplied *_infect* event does just this. It
iterates over all pairs of agents in sero-discordant sexual relationships, and
determines whether the negative partners becomes infected.

Consider two agents, *a* and *b*. One, *a*, is uninfected, and the other *b* is
infected. If we want the risk of infection to be determined by *a*'s sex and
whether it is in a same-sex or opposite-sex relationship with *b* then we need
some way of specifying this in a dataset. Also, we are interested in what
infection stage *b* is in. If *b* is on treatment, for example, the risk of
infecting *a* may be very low.

The dataset_infect.csv dataset shows how this is handled in |PROJECT|.

.. code-block:: none
   :linenos:

      sex;sex|1|~;infected;probability
      0;0;0;0
      0;0;1;0
      0;0;2;0.02
      0;0;3;0.008
      0;0;4;0.008
      0;1;0;0
      0;1;1;0
      0;1;2;0.01
      0;1;3;0.004
      0;1;4;0.004
      1;0;0;0
      1;0;1;0
      1;0;2;0.012
      1;0;3;0.005
      1;0;4;0.005
      1;1;0;0
      1;1;1;0
      1;1;2;0.0001
      1;1;3;0.0001
      1;1;4;0.0001

The header (line 1) contains two columns named *sex*. The first one corresponds
to the uninfected agent, *a*. The second and third columns are the sex and
infection stage of *b*. How does |PROJECT| know this? Look at the second column
heading: *sex|1|~*.  The first pipe (|) is used to separate sex from the amount the
property must be divided by. Well, unlike age, we don't want the sex to be more
granular, so we specify it as 1. The second pipe is followed by a tilde
(~). The tilde in the column header tells |PROJECT| that this is a two-agent
lookup table and the second agent's properties start in this column. So the
second and third columns belong to agent *b*. The final column, with name
*probability*, is simply the probability of becoming infected. (By default events
are executed daily, so the probability must correspond to this time-step.)

So if agent *a* is a female, and agent *b* is male in stage 2 (primary
infection), what is the risk of *a* becoming infected on this iteration of the
*_infect* event? The answer is given by line 14: 0.012.

******
Agents
******

Instead of generating agents, you can provide an agent file as input to the
simulation. In fact, since the agent generation features of |PROJECT| are
currently quite limited, you'll probably prefer to supply an agent file.

The agents must be specified in a CSV file. The column names in the header row
must correspond to one or more field names in |PROJECT|'s agent structure, which is declared
as *struct fsti_agent* in the source file *src/fsti-agent.h*. The fields are:

- id: unsigned 32 bit integer, unique for each agent (If you do not include this
  field, |PROJECT| automatically provides this value for each agent, starting from
  0.)
- sex: unsigned 8 bit integer (0 is male, 1 is female. Higher values are user-defined.)
- Either sex_preferred or orientation, an unsigned 8 bit integer (Do not use
  both fields. We recommend using sex_preferred rather than orientation. For
  sex_preferred 0 is male, 1 is female. For orientation either use 0 and 1 for
  heterosexual and homosexual respectively, or 0, 1, 2 and 3 for MSM, MSW, WSM
  and WSW respectively. Higher values are user-defined.)
- age: a positive year age of an agent between 0 and 120.
- birthday: a signed 32 bit integer (Unless you understand the internal workings
  of |PROJECT| very well, we recommend you rather use age)
- infected: unsigned 8 bit integer (0 is uninfected. 1 and up can correspond to
  stages of infection.)
- treated: unsigned 8 bit integer (0 is untreated. 1 and up can correspond to
  treatment regimens.)
- resistant: unsigned 8 bit integer (0 is no resistance. You can either use a
  simple approach to resistance, whereby 1 means resistant to treatment regimen
  1, 2 to treatment regimen 2 etc, or you can use a more complex binary bitmask
  approach where 1 denotes resistance to regimen 1 only, 10, denotes resistance
  to regimen 2, 11 denotes resistance to regimen 1 and 2 etc.)
- coinfected (0 means not coinfected. 1 and up denotes different types of
  coinfection as chosen by the user. Once again, as with the resistant field,
  either a simple or bitmask approach can be used.)
- partners_0, partners_1, and partners_2: unsigned 32 bit integers denoting the
  id of a sexual partner of this agent (-1 implies agent is single. The agents
  are typically numbered from 0. Note: None of the default |PROJECT| events
  currently caters for concurrency. Only use partners_1 and partners_2 if you
  are implementing events that rely on partner concurrency. If you need more
  partners, change the value of FSTI_MAX_PARTNERS in fsti_userdefs.h.)
- relchange_0, relchange_1, and relchange_2: unsigned 32 bit integers
  corresponding to the iteration (i.e. time step) in the simulation when the
  agent's relationship status for partner_0, partner_1 and partner_2
  respectively should change, either to single for agents with partners or to be
  placed in the mating pool if the agent is single

Here is an example CSV file. The default delimiter is a semi-colon, not a comma. You
can change this by setting the csv_delimiter.

.. code-block:: none
   :linenos:

      id;age;infected;sex;sex_preferred;partners_0
      0;45.21;0;1;0;-1
      1;47.35;1;0;1;0
      2;36.62;0;1;0;-1
      3;35.40;0;1;0;-1
      4;24.25;0;0;1;-1
      5;24.12;0;0;1;4
      6;23.26;0;0;1;-1
      7;45.17;0;0;1;-1
      8;34.81;0;0;1;-1
      9;35.80;0;0;1;8
