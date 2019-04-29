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
