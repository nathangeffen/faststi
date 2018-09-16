#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>

#include "fsti-error.h"

atomic_int fsti_error;

char fsti_error_string[FSTI_ERROR_STRING_LEN + 1] = "";

bool fsti_output_error_messages = true;

const char *fsti_error_message(int code)
{
    switch (code) {
    case FSTI_FAILURE:
	return "operation failed";
    case FSTI_ERR_KEY_FILE_OPEN:
	return "could not open key file";
    case FSTI_ERR_KEY_FILE_LOAD:
	return "could not load key file";
    case FSTI_ERR_KEY_FILE_SYNTAX:
	return "syntax error in key file";
    case FSTI_ERR_KEY_FILE_GROUP_EMPTY:
	return "group in key file empty";
    case FSTI_ERR_NOMEM:
	return "could not allocate memory";
    case FSTI_ERR_THREAD_FAIL:
	return "failure with thread system";
    case FSTI_ERR_CONFIG_ADD:
	return "could not add configuration entry";
    case FSTI_ERR_NO_VALUE_FOR_KEY:
	return "no value found for key";
    case FSTI_ERR_KEY_NOT_FOUND:
	return "key not found";
    case FSTI_ERR_OUT_OF_BOUNDS:
	return "index for value in configuration is out of bounds";
    case FSTI_ERR_STR_EXPECTED:
	return "string value was expected but not received";
    case FSTI_ERR_LONG_EXPECTED:
	return "long value was expected but not received";
    case FSTI_ERR_DBL_EXPECTED:
	return "double value was expected but not received";
    case FSTI_ERR_WRONG_TYPE:
	return "value was not the expected type";
    case FSTI_ERR_EVENT:
	return "unspecified error occurred in an event";
    case FSTI_ERR_EVENT_NOT_FOUND:
	return "event not found";
    case FSTI_ERR_AGENT_FILE:
	return "error processing agent file";
    case FSTI_ERR_FILE:
	return "error doing file processing";
    case FSTI_ERR_INVALID_VALUE:
	return "invalid value";
    default:
	return "unknown error message number - this shouldn't happen";
    }
}

int fsti_error_msg(int errnum,
		   const char *file, int line, const char *extra_message)
{
    if (fsti_output_error_messages) {
	fprintf(stderr, "%s %d: %s.", file, line,
		fsti_error_message(errnum));
	if (extra_message) {
            fputc(' ', stderr);
            fputs(extra_message, stderr);
        }
    }
    fputs(".\n", stderr);
    return errnum;
}
