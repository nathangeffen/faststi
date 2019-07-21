AM_CFLAGS = -std=c11  

bin_PROGRAMS = faststi
faststi_SOURCES = fsti-agent.c fsti-defs.c fsti-main.c fsti-simulation.c fsti-config.c fsti-error.c fsti-procini.c fsti-tests.c fsti-dataset.c fsti-eventdefs.c fsti-report.c fsti-userdefs.c fsti-defaults.c  fsti-events.c fsti-simset.c utils/algorithms.c  utils/array.c  utils/csv.c  utils/test.c
faststi_CFLAGS =  $(GLIB_CFLAGS) $(GSL_CFLAGS)
faststi_LDADD = $(GLIB_LIBS) $(GSL_LIBS)

