# FastSTI: Agent-based simulator of sexually transmitted infections

FastSTI is a microsimulation framework for modelling sexually transmitted
infections (STIs).

FastSTI is written in C and designed to be fast and efficient. It is capable of
running simulations of tens of millions of agents on modern consumer hardware.

## Installation

Carry out these steps to install FastSTI on a GNU/Linux PC.

Install the depenendencies:

- *sudo apt install build-tools*
- *sudo apt install git*
- *sudo apt install meson-build*
- *sudo apt install valgrind* (optional but extremely useful if you write code)


Installing the framework is quick and easy.

- Create a folder for the installation and cd into it. E.g.
-- *mkdir mysim; cd mysim*
- git clone https://github.com/nathangeffen/faststi .

Installations are cheap and easy. Install as often as you like and, if an
installation is no longer needed, simply remove its folder with "rm -f".

Test that it works by running:

- *meson debug*
- *cd debug; ninja*
- *./src/faststi -t*

This will run the test suite. If everything is properly installed no failures
should be reported.

You've created an unoptimised version of the faststi executable, useful for
debugging and development. To create an optimised version for running big,
CPU-intensive simulations, do this:

XX Fill in steps

## Using the framework

You can do a lot with FastSTI without any programming.

XX Instructions here
