
SOURCES = fsti-agent.c fsti-defs.c fsti-simulation.c fsti-config.c fsti-error.c fsti-procini.c fsti-tests.c fsti-dataset.c fsti-eventdefs.c fsti-report.c fsti-userdefs.c fsti-defaults.c  fsti-events.c fsti-simset.c fsti-pythoncalls.c utils/algorithms.c  utils/array.c  utils/csv.c  utils/test.c


lib_LTLIBRARIES = libfaststi.la
libfaststi_la_SOURCES = $(SOURCES)
libfaststi_la_CFLAGS =  $(GLIB_CFLAGS) $(GSL_CFLAGS)
libfaststi_la_LIBADD = $(GLIB_LIBS) $(GSL_LIBS)

bin_PROGRAMS = faststi
faststi_CFLAGS =  $(GLIB_CFLAGS) $(GSL_CFLAGS)
faststi_LDADD = $(GLIB_LIBS) $(GSL_LIBS) libfaststi.la
faststi_SOURCES =  fsti-main.c

TESTS = check_faststi.sh valgrind_faststi.sh
EXTRA_DIST = check_faststi.sh valgrind_faststi.sh
