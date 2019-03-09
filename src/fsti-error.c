/*
  FastSTIi: large simulations of sexually transmitted infection epidemics.

  Copyright (C) 2019 Nathan Geffen

  FastSTI is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FastSTI is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>

#include "fsti-error.h"

#define ERROR_MSG_LEN 200

_Thread_local int fsti_error;
_Thread_local char fsti_error_string[FSTI_ERROR_STRING_LEN + 1] = "";

bool fsti_output_error_messages = true;

const char *fsti_error_message(int code)
{
    switch (code) {
    case FSTI_FAILURE:
	return "Operation failed";
    case FSTI_ERR_KEY_FILE_OPEN:
	return "Could not open key file";
    case FSTI_ERR_KEY_FILE_LOAD:
	return "Could not load key file";
    case FSTI_ERR_KEY_FILE_SYNTAX:
	return "Syntax error in key file";
    case FSTI_ERR_KEY_FILE_GROUP_EMPTY:
	return "Group in key file empty";
    case FSTI_ERR_NOMEM:
	return "Could not allocate memory";
    case FSTI_ERR_THREAD_FAIL:
	return "Failure with thread system";
    case FSTI_ERR_CONFIG_ADD:
	return "Could not add configuration entry";
    case FSTI_ERR_NO_VALUE_FOR_KEY:
	return "No value found for key";
    case FSTI_ERR_KEY_NOT_FOUND:
	return "Key not found";
    case FSTI_ERR_INVALID_CSV_FILE:
        return "Error in csv file";
    case FSTI_ERR_DATASET_FILE:
        return "Error in dataset file";
    case FSTI_ERR_MISSING_DATASET:
        return "Dataset expected but not found";
    case FSTI_ERR_OUT_OF_BOUNDS:
	return "Index for value is out of bounds";
    case FSTI_ERR_STR_EXPECTED:
	return "String value was expected but not received";
    case FSTI_ERR_LONG_EXPECTED:
	return "Long value was expected but not received";
    case FSTI_ERR_DBL_EXPECTED:
	return "Double value was expected but not received";
    case FSTI_ERR_WRONG_TYPE:
	return "Value was not the expected type";
    case FSTI_ERR_EVENT:
	return "Unspecified error occurred in an event";
    case FSTI_ERR_EVENT_NOT_FOUND:
	return "Event not found";
    case FSTI_ERR_AGENT_FILE:
	return "Error processing agent file";
    case FSTI_ERR_FILE:
	return "Error doing file processing";
    case FSTI_ERR_NO_STOP_EVENT:
        return "No event defined to check for end of simulation";
    case FSTI_ERR_INVALID_VALUE:
	return "Invalid value";
    case FSTI_ERR_INVALID_DATE:
	return "Invalid date";

    case FSTI_WARN_DATASET_USING_VALS_NOT_FILE:
        return "Warning: dataset using configuration values instead of file";

    default:
	return "Unknown error message number - this shouldn't happen";
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
    fputs("\n", stderr);
    return errnum;
}

char *fsti_sprintf(char *fmt, ...)
{
    va_list ap;
    int size;

    va_start(ap, fmt);
    size = vsnprintf(fsti_error_string, FSTI_ERROR_STRING_LEN, fmt, ap);
    va_end(ap);
    fsti_error_string[size] = '\0';

    return fsti_error_string;
}
