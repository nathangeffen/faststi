############
Installation
############

FastSTI is easiest to run in a Posix environment (GNU Linux, FreeBSD, Unix etc),
because we develop it on under Linux. Nevertheless, it isn't difficult to get it
running on a Mac or Windows machine.

**********
Linux etc.
**********

Download the FastSTI `archived distribution
<https://www.simhub.online/media/dist/faststi/latest.tar.gz>`_ and decompress
it.

E.g. ::

  wget https://www.simhub.online/media/dist/faststi/latest.tar.gz
  tar xzvf latest.tar.gz

You also need to install some dependencies. These are the standard C development
tools, the popular git source code control system, the `GNU Scientific Library
<https://www.gnu.org/software/gsl/>`_ and `Gnome's Glib utility library
<https://developer.gnome.org/glib/>`_.


On Ubuntu, Debian, Mint etc. this works: ::

  sudo apt install git build-essential libgsl-dev libglib2.0-dev

Now you're ready to test FastSTI.

***
Mac
***

Download and decompress this file: https://www.simhub.online/media/dist/faststi/latest.tar.gz

Using `Homebrew <https://brew.sh/>`_ install the dependencies: ::

  brew install gcc pkg-config glib gsl

Now you're ready to test FastSTI.

*******
Windows
*******

Under Windows you either need to install the Cygwin or MinGW environment, or if
you are using Windows 10, you can install the Windows Subsystem for Linux. But
to date we have only tested installation with Cygwin.

Cygwin
******

Use the Cygwin setup tool to install these packages:

- gcc-core
- gcc-tools-epoch2-autoconf
- gcc-tools-epoch2-automake
- libgsl-dev
- libglib2.0-devel
- wget

After installation open the Cygwin terminal and run these steps: ::

  wget https://www.simhub.online/media/dist/faststi/latest.tar.gz
  tar xzvf latest.tar.gz

Now you're ready to test FastSTI.
