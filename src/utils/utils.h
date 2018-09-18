/**
   @file

   @brief Convenient header file containing other headers used in *Useful*.

   @mainpage Useful: Some algorithms, macros and utilities for C programmers

   @section intro_sec Introduction

   This is a collection of useful C algorithms, macros and utilities.

   It includes:

   - <a href="test_8h.html">Testing framework</a> (a very simple one): test.h
   - <a href="array_8h.html">Array management macros and functions</a>: array.h
   - <a href="algorithms_8h.html">Algorithms that are not already in glib or
     gsl</a>: algorithms.h
   - <a href="csv_8h.html">CSV file input and output, and conversion to and from
     R-like dataframes</a>: csv.h

   @section install_sec Installation

   Install libuseful using a package manager. To use it *Useful* in your
   program, include <useful.h> in your source code. Alternately include just
   what you need, i.e. <useful/test.h>, <useful/array.h> and/or
   <useful/algorithms.h>.

   The easiest way to compile is to use pkg-config. E.g.

   @verbatim
   gcc myprog.c `pkg-config --cflags --libs useful` -o myprog
   @endverbatim

   # Examples

   ## Arrays

   @include arrays.c

   Output:

   @include arrays.out

   ## Read and process a CSV file

   @include readcsv.c

   Output:

   @include readcsv.out

   See @ref useful.h


   @copyright Copyright (C) Nathan Geffen, 2017.  (See LICENSE in the root
   directory of the distribution)

   @date 2018

 */

#ifndef UTILS_H
#define UTILS_H

/**
   Includes the header files for the rest of the *Useful* project.
 */

#include "test.h"
#include "array.h"
#include "algorithms.h"
#include "csv.h"

#endif
