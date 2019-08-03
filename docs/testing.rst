#####################
Testing that it works
#####################

Make sure you're in the directory that you created in the installation step. If
you use gcc as your C compiler, then simply do this: ::

  ./configure
  make
  FSTI_DATA=./data ./src/faststi -t

This will run the test suite. If everything is properly installed no failures
should be reported.

.. note:: You may be wondering why we prefix the executable with
          *FSTI_DATA=./data*. The *FSTI_DATA* environment variable simply tells
          FastSTI where the data files are stored. As you work on FastSTI you
          may use a different directory for your data files. It's simply that
          the default data files are in the *data* directory.


FastSTI uses `GNU autotools
<https://www.gnu.org/software/automake/manual/html_node/index.html>`_ to compile
which is the standard on POSIX (Linux, Unix etc) systems. The standard autotools
commands work with FastSTI, but we don't recommend running *make install*
because it's better to have each model that you build with FastSTI running in
its own standalone directory.


Incidentally, running make like this might make FastSTI a bit faster: ::

  make clean
  make CFLAGS=-O3

But results may vary.

.. tip:: You may have noticed that there are files for the the `Meson Build
          system <https://mesonbuild.com/>`_ in the FastSTI folder. (If you
          haven't heard of the Meson Build system, don't worry; skip this tip.)
          FastSTI can be built with meson and ninja. We intend to keep FastSTI
          compatible with both build systems, but because autotools is the more
          widely used standard, it's the one we document here.

          But feel free and encouraged to use meson and ninja to compile
          FastSTI. For one thing it's faster because it does parallel
          compilations. Note though, that the builds are placed in different
          directories, and so to keep things simple this documentation is
          written for people using autotools.
