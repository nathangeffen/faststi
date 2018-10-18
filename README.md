# FastSTI: Agent-based simulator of sexually transmitted infections

FastSTI is a microsimulation framework for modelling sexually transmitted
infections (STIs).

FastSTI is written in C and designed to be fast and efficient. It is capable of
running simulations of tens of millions of agents on modern consumer hardware.

The code is available under the GPL-3 license.

## Installation on GNU/Linux

Carry out these steps to install FastSTI on a GNU/Linux PC that uses apt (e.g. Ubuntu).

Install the dependencies:

*sudo apt install git build-essential meson valgrind libgsl-dev libglib2.0-dev*

Installing the framework is quick and easy.

- Create a folder for the installation and cd into it. E.g.
-- *mkdir mysim; cd mysim*
- git clone https://github.com/nathangeffen/faststi .

Installations are cheap and easy. Install as often as you like and, if an
installation is no longer needed, simply remove its folder with "rm -f".

Test that it works:

- *meson debug*
- *cd debug; ninja*
- *./src/faststi -t*

This will run the test suite. If everything is properly installed no failures
should be reported.

You've created an unoptimised version of the faststi executable, useful for
debugging and development. To create an optimised version for running big,
CPU-intensive simulations, do this from the root directory of your simulation
(*mysim* in the above example):

*meson --buildtype release release*

To test that it's working:

*cd release;ninja test*

## Installation on macOS

Compiling faststi on macOS is easy. You can use either Apples clang/LLVM compiler
or the GNU C compiler.

### Building using GCC

Install and configure the dependencies with [Homebrew](https://brew.sh):

* (If you don't have Homebrew installed): `/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`
* `brew install gcc pkg-config glib gsl meson ninja`
* `pkg-config --cflags --libs gsl`
* `pkg-config --cflags --libs glib-2.0`

Clone the repository: `git clone https://github.com/nathangeffen/faststi` and
switch to the macOS port branch (from within the local directory): `git checkout macos`

Change into the `faststi` directory (or wherever you cloned it to) and execute
the following commands:

* `CC=/usr/local/bin/cc meson fsti_gcc`
* `cd fsti_gcc; ninja`
* `ninja test`

### Building using Apple's clang

Make sure you have the XCode command line tools installed. The easiest way to obtain
them is to install XCode from the App Store and run it. It will install the tools
upon first execution.

You also need glib, meson and ninja:

* (If you don't have Homebrew installed): `/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`
* `brew install pkg-config glib meson ninja`
* `pkg-config --cflags --libs glib-2.0`

Clone the repository: `git clone https://github.com/nathangeffen/faststi`

Change into the `faststi` directory (or wherever you cloned it to) and execute
the following commands:

* `CC=/usr/bin/cc meson fsti_clang`
* `cd fsti_clang; ninja`
* `ninja test`

## Installation on Windows

XX Instructions here

## Using the framework

You can do a lot with FastSTI without any programming.

XX Instructions here
