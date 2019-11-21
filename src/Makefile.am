
SOURCES = fsti-agent.c fsti-defs.c fsti-simulation.c fsti-config.c fsti-error.c fsti-procini.c fsti-tests.c fsti-dataset.c fsti-eventdefs.c fsti-report.c fsti-userdefs.c fsti-defaults.c  fsti-events.c fsti-simset.c fsti-pythoncalls.c utils/algorithms.c  utils/array.c  utils/csv.c  utils/test.c

lib_LTLIBRARIES = libfaststi.la
libfaststi_la_SOURCES = $(SOURCES)
libfaststi_la_CFLAGS =  $(GLIB_CFLAGS) $(GSL_CFLAGS)
libfaststi_la_LIBADD = $(GLIB_LIBS) $(GSL_LIBS)

nobase_include_HEADERS = fsti-agent.h fsti-defs.h fsti.h fsti-simulation.h fsti-config.h fsti-error.h fsti-procini.h fsti-tests.h fsti-dataset.h fsti-eventdefs.h fsti-report.h fsti-userdefs.h fsti-defaults.h fsti-events.h fsti-simset.h fsti-variants.h fsti-pythoncalls.h utils/algorithms.h  utils/array.h  utils/csv.h utils/test.h utils/uthash.h utils/utils.h


bin_PROGRAMS = faststi
faststi_CFLAGS =  $(GLIB_CFLAGS) $(GSL_CFLAGS)
faststi_LDADD = $(GLIB_LIBS) $(GSL_LIBS) libfaststi.la
faststi_SOURCES =  fsti-main.c

TESTS = check_faststi.sh valgrind_faststi.sh
EXTRA_DIST = check_faststi.sh valgrind_faststi.sh
