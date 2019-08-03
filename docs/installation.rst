############
Installation
############

*********
GNU/Linux
*********

Carry out these steps to install FastSTI on a GNU/Linux PC that uses apt
(e.g. Ubuntu).

Install the dependencies: ::

  sudo apt install git build-essential libgsl-dev libglib2.0-dev

These are the standard Linux development tools, the popular git source code
control system, the `GNU Scientific Library <https://www.gnu.org/software/gsl/>`_
and `Gnome's Glib utility library <https://developer.gnome.org/glib/>`_.

Installing the framework is quick and easy.

Download the latest `distribution
<https://www.simhub.online/media/latest.tar.gz>`_ and decompress it. For
example: ::

  wget https://www.simhub.online/media/latest.tar.gz
  tar xzvf latest.tar.gz
  cd faststi-0.2.0

Installations are cheap and easy. Install as often as you like and, if an
installation is no longer needed, simply remove its folder with "rm -f".

***********
Apple macOS
***********

Compiling on macOS is easy. You can use either Apples clang/LLVM compiler or the
GNU C compiler.

Install and configure the dependencies with `Homebrew <https://brew.sh/>`_: ::

  brew install gcc pkg-config glib gsl


If you don't have Homebrew installed: ::

  /usr/bin/ruby -e "$(curl -fsSL  https://raw.githubusercontent.com/Homebrew/install/master/install)"

Download  https://www.simhub.online/media/latest.tar.gz and decompress it.

Installations are cheap and easy. Install as often as you like and, if an
installation is no longer needed, simply remove its folder with "rm -f".


*******
Windows
*******

We haven't developed or tested FastSTI on Windows, but it should work,
especially if you use the MinGW suite. Feedback (via the `Github repository <https://github.com/faststi>`_) on
running it under Windows is welcome.
