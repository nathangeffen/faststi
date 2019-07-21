AC_INIT([faststi], [0.1], [nathangeffen@quackdown.info])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PROG_CC
# CFLAGS+=" -std=c11 -Wall -Werror -pedantic"
AC_CONFIG_HEADERS([config.h])
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.48])
PKG_CHECK_MODULES([GSL], [gsl >= 2.1])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT

