AC_INIT([faststi], [0.3.0], [nathangeffen@quackdown.info])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_CONFIG_MACRO_DIRS([m4])
AM_PROG_AR
AC_PROG_CC
LT_INIT

AC_CONFIG_HEADERS([config.h])
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.48])
PKG_CHECK_MODULES([GSL], [gsl >= 2.1])
PKG_INSTALLDIR
AC_CONFIG_FILES([
 Makefile
 src/Makefile
 faststi.pc
])
AC_OUTPUT
