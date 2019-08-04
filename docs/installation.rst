############
Installation
############

First download the FastSTI `archived distribution
<https://www.simhub.online/media/dist/faststi/latest.tar.gz>`_ and decompress
it.

E.g. ::

  wget https://www.simhub.online/media/dist/faststi/latest.tar.gz
  tar xzvf latest.tar.gz

You also need to install some dependencies. These are the standard C development
tools, the popular git source code control system, the `GNU Scientific Library
<https://www.gnu.org/software/gsl/>`_ and `Gnome's Glib utility library
<https://developer.gnome.org/glib/>`_.


On an **Ubuntu GNU/Linux** distribution you can do this: ::

  sudo apt install git build-essential libgsl-dev libglib2.0-dev

On a **Mac** you can do this using `Homebrew <https://brew.sh/>`_: ::

  brew install gcc pkg-config glib gsl

We haven't developed or tested FastSTI on Windows, but it should work,
especially if you use the MinGW suite. Feedback (via the `Github repository
<https://github.com/faststi>`_) on running it under Windows is welcome.

Now you're ready to test FastSTI.
