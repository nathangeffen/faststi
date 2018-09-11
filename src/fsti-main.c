#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <glib.h>
#include <useful.h>

#include "fsti.h"

static bool test = false, print_config = false;
static char *config_filename = NULL;
static char **config_strings = NULL;	// Configuration entries set on command line

static GOptionEntry command_line_entries[] = {
    {"file", 'f', 0, G_OPTION_ARG_FILENAME, &config_filename,
     "Name of configuration file", "<file name>"},
    {"config", 'c', 0, G_OPTION_ARG_STRING_ARRAY, &config_strings,
     "Configuration entry name and values",
     "<key>=<value>[;<value>]*"},
    {"print-default-config", 'p', 0, G_OPTION_ARG_NONE,
     &print_config, "Print the default configuration", NULL},
    {"test", 't', 0, G_OPTION_ARG_NONE, &test,
     "Run unit tests", NULL},
    {NULL}
};


static void get_command_line(int argc, char *argv[])
{
    GError *error = NULL;
    GOptionContext *context;

    context = g_option_context_new("Fast simulator of sexually "
				   "transmitted infection epidemics");
    g_option_context_add_main_entries(context, command_line_entries, NULL);


    if (!g_option_context_parse(context, &argc, &argv, &error)) {
	fprintf(stderr, "Command line error: %s\n", error->message);
	exit(1);
    }

    g_option_context_free(context);
}

void free_statics()
{
    free(config_filename);
    g_strfreev(config_strings);
}


int main(int argc, char *argv[])
{
    struct fsti_simset simset;

    setlocale(LC_ALL, "");
    get_command_line(argc, argv);


    if (test)
        return fsti_tests();

    fsti_simset_init(&simset);

    if (config_filename)
        fsti_simset_load_config_file(&simset, config_filename);
    if (config_strings)
        fsti_simset_load_config_strings(&simset, config_strings);
    if (print_config)
	fsti_config_print_all(&simset.config);

    fsti_simset_set_csv(&simset, fsti_default_csv_entries,
                        sizeof(fsti_default_csv_entries) /
                        sizeof(struct fsti_agent_csv_entry),
                        sizeof(struct fsti_agent_default_data));
    fsti_simset_exec(&simset);

    fsti_simset_free(&simset);
    free_statics();

    return 0;
}
