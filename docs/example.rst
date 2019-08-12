###################
Interesting example
###################

Here is an experiment that we hope demonstrates the versatility of |PROJECT|.

It shows some interesting and counter-intuitive results, as well as how to use
FastSTI in conjunction with a scripting language like R.

Here is the input configuration, which you can also find in simulations/examples/experiment.ini:

.. code-block:: ini
   :linenos:

      [Simulation Group 0]

      threads=0
      match_k = 1
      num_simulations=30
      num_agents=10000
      time_step=1 DAY
      simulation_period=10 YEARS
      report_frequency=100

      before_events=_write_results_csv_header;_generate_and_pair;_report
      during_events=_age;_breakup_and_pair;_infect;_stage;_birth;_death;_report
      after_events=_report

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

      results_file=results.csv

      [Simulation Group 1]
      match_k = 100

      [Simulation Group 2]
      match_k = 1
      num_agents=100000

      [Simulation Group 3]
      match_k = 100


It tells FastSTI to run four groups of simulations. Each group runs 30 simulations.

All groups run daily for ten years starting on 1 January 2018. For each
simulation statistics are printed every 100 days, and then at the end, 1 January
2028. (The dates don't matter, by the way.)

- The first group (group 0) is 10,000 agents with random matching.
- The second group is 10,000 agents with k-nearest neighbour matching (using
  |PROJECT|'s _rkpm event) xwith k=100.
- The third group is 100,000 agents with random matching.
- The fourth group is 100,000 agents with k-nearest neighbour matching with
  k=100.

The results are written to a file called results.csv. The file is large so we've
`compressed <_static/results.tgz>`_ it.  On a mid-range laptop with an i5 with
four cores and 12GB of RAM it took just over 16 minutes to run the 120
simulations.

We then ran results.csv through this R script, which you can find in
scripts/processResultsFile.R.

.. code-block:: R
   :linenos:

      # Takes a results CSV file produced by FastSTI and runs a function
      # (default mean) over a statistic (description) grouping the result
      # either by the maximum date or all the dates.

      processFastSTIResults <- function(filename="results.csv",
                                        sep=";",
                                        description="INFECT_RATE_ALIVE",
                                        fun=mean,
                                        header=TRUE,
                                        filter_max_date=TRUE) {
          inp = read.csv(filename, sep=sep, header=header)
          values = inp[inp$description==description,]
          if (filter_max_date) {
              analysisDate = max(as.Date(values$date))
              values = values[as.Date(values$date)==analysisDate,]
          }
          mean_values = aggregate(values$value,
                                  by=list(values$name,
                                          values$date),
                                  FUN=fun)
      }

      print(processFastSTIResults())

This is the output:

.. code-block:: none
   :linenos:

      Simulation Group 0	2028-01-01	0.4280027667
      Simulation Group 1	2028-01-01	0.3647885667
      Simulation Group 2	2028-01-01	0.4283530667
      Simulation Group 3	2028-01-01	0.2342883667

What do the results tell us?

1. Random matching results in much higher infection rates than "intelligent"
   matching algorithms that try to pair agents using realistic criteria. This is
   expected. With random matching, one would expect an infection to spread quite
   rapidly through a population. With more realistic matching, we expect more
   sexual networks to form and the infection only occasionally moves from one
   network to another.

2. Random matching gives the same results irrespective of the number of agents
   (42.8%). This too is expected.

3. With "intelligent" matching prevalence decreases as the population increases
   (a mean of 36.5% for the 10,000 agent simulations after ten years vs 23.4%
   for the 100,000 agent simulations). This is not intuitive, at least not to
   us, though we have hypotheses for why it happens. We leave it to you to
   speculate further.
