############
Installation
############

*********
GNU/Linux
*********

Carry out these steps to install FastSTI on a GNU/Linux PC that uses apt
(e.g. Ubuntu).

Install the dependencies: ::

  sudo apt install git build-essential meson valgrind libgsl-dev libglib2.0-dev

Installing the framework is quick and easy.

Create a folder for the installation and cd into it. E.g. ::

  mkdir mysim; cd mysim
  git clone https://github.com/nathangeffen/faststi .

Installations are cheap and easy. Install as often as you like and, if an
installation is no longer needed, simply remove its folder with "rm -f".

***********
Apple macOS
***********

Compiling on macOS is easy. You can use either Apples clang/LLVM compiler or the
GNU C compiler.

Install and configure the dependencies with `Homebrew <https://brew.sh/>`_: ::

  brew install gcc pkg-config glib gsl meson ninja


If you don't have Homebrew installed: ::

  /usr/bin/ruby -e "$(curl -fsSL  https://raw.githubusercontent.com/Homebrew/install/master/install)"

- Create a folder for the installation and cd into it. E.g. ::

    mkdir mysim; cd mysim
    git clone https://github.com/nathangeffen/faststi .

Installations are cheap and easy. Install as often as you like and, if an
installation is no longer needed, simply remove its folder with "rm -f".



*******
Windows
*******

We haven't developed or tested FastSTI on Windows, but it should work,
especially if you use the MinGW suite. Feedback (via the `Github repository <https://github.com/faststi>`_) on
running it under Windows is welcome.
