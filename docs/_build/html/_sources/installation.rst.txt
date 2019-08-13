############
Installation
############

|PROJECT| is easiest to run in a Posix environment (GNU Linux, FreeBSD, Unix etc),
because we develop on Linux. Nevertheless, it isn't difficult to get it running
on a Mac or Windows machine.

**********
Linux etc.
**********

1. |PROJECT| requires a standard C development environment, the `GNU Scientific
Library <https://www.gnu.org/software/gsl/>`_ and `Gnome's Glib utility library
<https://developer.gnome.org/glib/>`_.


On Ubuntu, Debian, Mint etc. this should do the trick: ::

  sudo apt install build-essential libgsl-dev libglib2.0-dev


2. Download the |PROJECT| `archived distribution
<https://www.simhub.online/media/dist/faststi/latest.tar.gz>`_ and decompress
it.

E.g. ::

  wget https://www.simhub.online/media/dist/faststi/latest.tar.gz
  tar xzvf latest.tar.gz

This should create a directory called |DIRNAME| which contains the |PROJECT| files.

Now go to step 3.

***
Mac
***

1. Download and decompress this file: https://www.simhub.online/media/dist/faststi/latest.tar.gz

2. Using `Homebrew <https://brew.sh/>`_ install the dependencies: ::

     brew install gcc pkg-config glib gsl

This should create a directory called |DIRNAME| which contains the |PROJECT| files.

Now go to step 3.

*******
Windows
*******

Under Windows you either need to install the Cygwin or MinGW environment, or if
you are using Windows 10, you can install the Windows Subsystem for Linux. But
to date we have only tested installation with Cygwin.

Cygwin
******

1. Use the Cygwin setup tool to install these packages:

- gcc-core
- make
- automake
- libgsl-dev
- libglib2.0-devel
- wget

2. After installation open the Cygwin terminal and run these steps: ::

     wget https://www.simhub.online/media/dist/faststi/latest.tar.gz
     tar xzvf latest.tar.gz

This should create a directory called |DIRNAME| which contains the |PROJECT| files.

Now go to step 3.

***************
On all machines
***************

3. Change into the directory that you've decompressed the distribution
into: |DIRNAME|

4. Now run: ::

     ./configure
     make -j

5. Then depending on your system either one of these should install the files: ::

     sudo make install

Or::

     make install

6. Now test that it works: ::

     FSTI_DATA=./data faststi -t

This will run the test suite. If everything is properly installed then no
failures should be reported.

.. note:: You may be wondering why we prefix the executable with
          *FSTI_DATA=./data*. The *FSTI_DATA* environment variable simply tells
          |PROJECT| where the data files are stored. As you work on |PROJECT| you
          may use a different directory for your data files. It's simply that
          the default data files are in the *data* directory.

|PROJECT| uses `GNU autotools
<https://www.gnu.org/software/automake/manual/html_node/index.html>`_ to compile
which is the standard on POSIX (Linux, Unix etc) systems. The standard autotools
commands work with |PROJECT|.

Incidentally, compiling like this might make |PROJECT| a bit faster: ::

  make clean
  make "CFLAGS=-DNDEBUG -O3"
  sudo make install

But results may vary.

To uninstall |PROJECT| simply run: ::

  sudo make uninstall
