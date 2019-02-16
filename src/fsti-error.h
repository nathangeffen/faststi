#ifndef FSTI_ERROR_H
#define FSTI_ERROR_H

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdatomic.h>

#ifdef __linux__
#include <execinfo.h>
#endif

#define FSTI_NAME "FastSTI"
#define FSTI_ERROR_STRING_LEN 200


#define FSTI_MSG(msg1, msg2)                                            \
    fsti_sprintf("%s %s", msg1, msg2)

#define FSTI_ERROR_SET(errnum, msg)					\
	do {					 			\
		fsti_error = errnum;					\
		fsti_error_msg(errnum, __FILE__, __LINE__, msg);	\
	} while (0)

#define FSTI_ERROR(cond, errnum, msg)					\
	(cond) ?							\
	       ( (fsti_error = errnum) &&				\
		       fsti_error_msg(errnum, __FILE__, __LINE__, msg))	\
	       :							\
	       0

#ifdef __linux__

#define FSTI_BACKTRACE                                                  \
    do {                                                                \
        int j, nptrs;                                                   \
        void *buffer[100];                                              \
        char **strings;                                                 \
        nptrs = backtrace(buffer, 100);                                 \
        fprintf(stderr, "backtrace() returned %d addresses\n", nptrs);  \
        strings = backtrace_symbols(buffer, nptrs);                     \
        if (strings)                                                    \
            for (j = 0; j < nptrs; j++)                                 \
                fprintf(stderr, "%s\n", strings[j]);                    \
        free(strings);                                                  \
    } while(0)

#else

#define FSTI_BACKTRACE

#endif

#define FSTI_WARNING(errnum, msg)                       \
    fsti_error_msg(errnum, __FILE__, __LINE__, msg)


#ifndef FSTI_ASSERT
#ifndef FSTI_NO_SAFETY_BELT
#define FSTI_ASSERT(cond, errnum, msg)                                  \
    do {                                                                \
        if( ((cond) == 0)) {                                            \
            fsti_error_msg(errnum, __FILE__, __LINE__, msg);            \
            FSTI_BACKTRACE;                                             \
            exit(1);                                                    \
        }                                                               \
    } while(0)
#else
#define FSTI_ASSERT(cond, errnum, msg)
#endif
#endif

enum {
    FSTI_SUCCESS = 0,
    FSTI_FAILURE = -1,
    FSTI_ERR_KEY_FILE_OPEN = 1,
    FSTI_ERR_KEY_FILE_LOAD,
    FSTI_ERR_KEY_FILE_SYNTAX,
    FSTI_ERR_KEY_FILE_GROUP_EMPTY,
    FSTI_ERR_NOMEM,
    FSTI_ERR_THREAD_FAIL,
    FSTI_ERR_CONFIG_ADD,
    FSTI_ERR_NO_VALUE_FOR_KEY,
    FSTI_ERR_KEY_NOT_FOUND,
    FSTI_ERR_INVALID_CSV_FILE,
    FSTI_ERR_DATASET_FILE,
    FSTI_ERR_MISSING_DATASET,
    FSTI_ERR_OUT_OF_BOUNDS,
    FSTI_ERR_STR_EXPECTED,
    FSTI_ERR_LONG_EXPECTED,
    FSTI_ERR_DBL_EXPECTED,
    FSTI_ERR_WRONG_TYPE,
    FSTI_ERR_EVENT,
    FSTI_ERR_EVENT_NOT_FOUND,
    FSTI_ERR_AGENT_FILE,
    FSTI_ERR_NO_STOP_EVENT,
    FSTI_ERR_FILE,
    FSTI_ERR_INVALID_VALUE,
    FSTI_ERR_INVALID_DATE,

    FSTI_WARN_DATASET_USING_VALS_NOT_FILE
};

extern bool fsti_output_error_messages;
extern _Thread_local int fsti_error;
extern _Thread_local char fsti_error_string[FSTI_ERROR_STRING_LEN + 1];

int fsti_error_msg(int errnum, const char *file, int line,
		   const char *extra_message);

char *fsti_sprintf(char *fmt, ...);

#endif
