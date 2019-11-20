#############################
Running |PROJECT| from Python
#############################

.. warning:: The Python interface to |PROJECT| is highly experimental. We hope
             to make Python the main way to use |PROJECT| because Python is so
             easy to use and popular among science researchers. But at this
             point we've only got the bare basics working. The entire Python
             interface to |PROJECT| is likely to change. Also currently if
             FastSTI exits because of an error, the Python interpreter also
             exits instead of throwing an exception, which is annoying. We will
             fix this in due course.

A basic Python 3 interface is supported (we don't support Python 2 and don't
intend to). You'll need to install `pandas <https://pandas.pydata.org/>`_.

In the scripts directory of the |PROJECT| folder is faststi.py.

To see the command line options it supports run (for example): ::

  python faststi -h

To execute a set of simulations: ::

  python faststi -c my_simulations.ini


Example of how to use faststi in a Python environment: ::

   import faststi
   import pandas

   simset1 = faststi.SimulationSet(config_filename="my_simulations.ini")

   # Runs all the simulations and save the results into a dataframe
   dataframe = simset1.run()

   # Slightly more sophisticated example
   simset2 = faststi.SimulationSet(config_filename="my_simulations.ini")
   print("There are", len(simset2.simulations), "simulations.")
   # Change the match_k parameter in the third simulation (0-based indexing)
   simset2.simulations[2].set_parm("match_k", 300)
   # Run the first four simulations: 0,1,2,3
   dataframe2 = simset2.run(0, 4)
   # Run the remaining simulations
   dataframe3 = simset2.run(4)


In time, we hope to make this much more comprehensive, robust and useful.
