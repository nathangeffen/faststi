FastSTI: Simulations of sexually transmitted infections
======================================================================

Introduction
------------

FastSTI is a simulation framework for modelling large simulations of sexually
transmitted infections. It is designed to handle up to tens of millions of
agents on standard consumer hardware, like your laptop or, or a high performance
computer.

It runs simulations in parallel. The output of the simulations is written to CSV
files, which can be further processed in scripting languages like Python
or R.

Installation
------------

Linux
~~~~~

If you're using an Ubuntu or Debian Linux or one of their derivatives, you can
install a recent stable version using Synaptic or the command line::

  sudo apt install faststi

Installing from Git
~~~~~~~~~~~~~~~~~~~

Windows, Mac and Linux users who do not use *apt* can install by cloning
FastSTI's Github repository:

- Create a folder for the installation and cd into it. E.g.

  - *mkdir mysim; cd mysim*

- git clone https://github.com/nathangeffen/faststi .

To compile and execute FastSTI, you'll need to install

- either the Clang or GCC C compiler (MinGW on Windows),
- the GNU scientific library (libgsl-dev on Linux systems supporting apt),
- the Glib 2 library (libglib2.0-dev on Linux systems supporting apt), and
- the Meson build system, though you can easily compile the source without
  Meson.

If you intend to modify the FastSTI source code, consider installing Valgrind.

Testing that it works
~~~~~~~~~~~~~~~~~~~~~

On a command line run::

  fsti -t

This should run in at most a few seconds. It should print out the results of the
test cases. If it's working it should report hundreds or even thousands of
successes and zero failures.


Quick start
-----------

FastSTI

How it works
------------

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

Example
~~~~~~~

.. _parameter-ref:

Parameters
----------

Datasets
--------

.. _event-ref:

Events
------

Extending FastSTI
-----------------

Help improve FastSTI
--------------------

.. toctree::
   :maxdepth: 2
   :caption: Contents:


Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
