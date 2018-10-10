#!/bin/bash

# DEPENDENCIES
# if you don't have the dependencies set up, uncomment some or all of the next lines
# install homebrew
#/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
# install gcc, pkg-config glib and gsl
#brew install gcc pkg-config glib gsl
# configure pkg-config
#pkg-config --cflags --libs gsl
#pkg-config --cflags --libs glib-2.0

# clean previous builds
rm -rf debug

# if you want to use Apple's clang comment next line and uncomment the one after
CC=/usr/local/bin/cc meson debug
#meson debug
cd debug
ninja
