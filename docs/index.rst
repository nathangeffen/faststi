=======================================================
FastSTI: Simulations of sexually transmitted infections
=======================================================

############
Introduction
############

FastSTI is a framework for agent-based models of large simulations of sexually
transmitted infection epidemics. It is designed to handle up to tens of millions of
agents on standard consumer hardware, like your laptop or, or a high performance
computer.

It runs simulations in parallel. The output of the simulations is written to CSV
files, which can be further processed in scripting languages like Python
or R.

You can do sophisticated simulations without making any modifications to the
code. But if you do wish to extend the framework, it is written in C. The
framework has been designed with extensibility in mind. The code adheres
strictly to the C11 standard and should compile on any good modern C compiler.

############
Installation
############

*********
GNU/Linux
*********

Carry out these steps to install FastSTI on a GNU/Linux PC that uses apt
(e.g. Ubuntu).

Install the dependencies: ::

  sudo apt install git build-essential meson valgrind libgsl-dev libglib2.0-dev

Installing the framework is quick and easy.

Create a folder for the installation and cd into it. E.g. ::

  mkdir mysim; cd mysim
  git clone https://github.com/nathangeffen/faststi .

Installations are cheap and easy. Install as often as you like and, if an
installation is no longer needed, simply remove its folder with "rm -f".

***********
Apple macOS
***********

Compiling on macOS is easy. You can use either Apples clang/LLVM compiler or the
GNU C compiler.

Install and configure the dependencies with `Homebrew <https://brew.sh/>`_: ::

* (If you don't have Homebrew installed): ::

    /usr/bin/ruby -e "$(curl -fsSL  https://raw.githubusercontent.com/Homebrew/install/master/install)"
    brew install gcc pkg-config glib gsl meson ninja

- Create a folder for the installation and cd into it. E.g. ::

    mkdir mysim; cd mysim
    git clone https://github.com/nathangeffen/faststi .

Installations are cheap and easy. Install as often as you like and, if an
installation is no longer needed, simply remove its folder with "rm -f".



*******
Windows
*******

We haven't developed or tested FastSTI on Windows, but it should work,
especially if you use the MinGW suite. Feedback (via the `Github repository <https://github.com/faststi>`_) on
running it under Windows is welcome.


#####################
Testing that it works
#####################

If you use gcc as your C compiler, then simply do this: ::

  meson debug
  cd debug; ninja
  FSTI_DATA=../data ./src/faststi -t

This will run the test suite. If everything is properly installed no failures
should be reported.

You've created an unoptimised version of the faststi executable, useful for
debugging and development. To create an optimised version for running big,
CPU-intensive simulations, do this from the root directory of your simulation
(*mysim* in the above example): ::

  meson --buildtype release release

To test that it's working: ::

  cd release
  FSTI_DATA=../data ./src/faststi -t

Alternately you can use the *fsti* script in your installation directory. Assuming
you called it *mysim*, change into the *mysim* directory, and run: ::

  ./fsti -t

The above will create the debug directory, compile FastSTI, and run the test
suite. To instead run the release version: ::

  ./fsti release -t

Again, no errors should be reported.

If you have `valgrind <https://valgrind.org>`_ on your system (an excellent tool
for finding memory and other problems in C programs), you can do this: ::

  cd debug
  ninja test

##################
How to use FastSTI
##################

A simulation continuously iterates over sets of agents, executing events on the
agents on each iteration (which we call a time step). The structure of a FastSTI
simulation is: ::

  Execute pre-simulation events
  for each time-step
    for each event E
        for each agent A
            if E should be applied to A
                apply E to A
  Execute post-simulation events

The number of agents and the specific events to execute are specified in a
configuration file. FastSTI's configuration file uses the *.ini* format, which
are the standard simple configuration format used on MS Windows and the GTK
framework popular on Linux systems.

You can configure the number of agents, the events and the order of events that
execute upon them, the size of the time step (default 1 day), the number of time
steps (default 10 years) and much else (see :ref:`parameter-ref`).

FastSTI has a number of useful built-in events useful for modelling STI
epidemics (see :ref:`event-ref`). These include agent ageing, death, matching
agents in sexual relationships, infection with the STI, disease advance,
co-infection, and breakups.

There are also useful supporting events that read in agent files or generate the
agents, write the agents to a CSV file, and write basic statistics to a CSV file.

If you need more events, the framework has been designed with extensibility in
mind. You can define new events in C, identify them to FastSTI, quickly
recompile the code and use them.

To do.

*******
Example
*******

There are more examples in the simulation directory.

.. _parameter-ref:

##########
Parameters
##########

To do

########
Datasets
########

To do

.. _event-ref:

######
Events
######

To do

#################
Extending FastSTI
#################

To do

####################
Help improve FastSTI
####################

We appreciate help with FastSTI, both with coding and documentation. Here are
some things that need to be done:

- Built-in calibration: At present if you want to calibrate a FastSTI model,
  you'd best use R or Python to process agent output files from FastSTI, and generate .ini
  configuration files as input back into FastSTI with new parameters. We think
  it would probably be better if FastSTI's configuration could be enhanced to
  calibrate the model.

- Concurrent relationships: Although the fsti_agent data structure supports
  concurrent relationships, none of the events are currently designed to handle
  meaningfully concurrent relationships.

- More events: Code variations on the current events and new events.

- Build Python and R interfaces to FastSTI.

- Improve this documentation.

Feel free to clone the Github repository and submit patches. But before doing
so, it may be best to email nathangeffen@gmail.com to inform us what you
would like to do, so we can discuss the best way to proceed.

#####################
Bugs and other issues
#####################

Please lodge bugs, requests for enhancements, etc at the FastSTI `Github
repository  issues page <https://github.com/nathangeffen/faststi/issues>`_.

#######
Credits
#######

- Nathan Geffen designed and implemented FastSTI.
- Stefan Scholz coded the syphilis model with Nathan.
- Eduard Grebe wrote the macOS installation instructions.

.. toctree::
   :maxdepth: 2
   :caption: Contents:


.. Indices and tables
.. ==================

.. * :ref:`genindex`
.. * :ref:`search`
