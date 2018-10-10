# FastSTI: Agent-based simulator of sexually transmitted infections

FastSTI is a microsimulation framework for modelling sexually transmitted
infections (STIs).

FastSTI is written in C and designed to be fast and efficient. It is capable of
running simulations of tens of millions of agents on modern consumer hardware.

## Installation on GNU/Linux

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

## Installation on macOS

Compiling faststi on macOS is easy, but involves a few additional steps

Install and configure the dependencies with [Homebrew](https://brew.sh):

* (If you don't have Homebrew installed): `/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`
* `brew install gcc pkg-config glib gsl meson ninja`
* `pkg-config --cflags --libs gsl`
* `pkg-config --cflags --libs glib-2.0`

Clone the repository: `git clone https://github.com/nathangeffen/faststi` and
switch to the macOS port branch (from within the local directory): `git checkout macos`

Build using meson and ninja using the gcc toolchain:

* `CC=/usr/local/bin/cc meson debug`
* `cd debug`
* `ninja`

## Installation on Windows

**Instructions to follow**

## Using the framework

You can do a lot with FastSTI without any programming.

XX Instructions here
