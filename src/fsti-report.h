#ifndef FSTI_REPORT_H
#define FSTI_REPORT_H

#include <stdint.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#include "fsti-simulation.h"
#include "fsti-defs.h"
#include "fsti-agent.h"

#define FSTI_MIN_VAL(x) _Generic((x),                                   \
                                 char: CHAR_MIN,                        \
                                 signed char: SCHAR_MIN,                \
                                 unsigned char: 0,                      \
                                 short: SHRT_MIN,                       \
                                 unsigned short: 0,                     \
                                 int: INT_MIN,                          \
                                 unsigned int: 0,                       \
                                 long: LONG_MIN,                        \
                                 unsigned long int: 0,                  \
                                 long long int: LLONG_MIN,              \
                                 unsigned long long int: 0,             \
                                 float: -FLT_MAX,                       \
                                 double: -DBL_MAX,                      \
                                 long double: -LDBL_MAX)

#define FSTI_MAX_VAL(x) _Generic((x),                                   \
                                 char: CHAR_MAX,                        \
                                 signed char: SCHAR_MAX,                \
                                 unsigned char: UCHAR_MAX,              \
                                 short: SHRT_MAX,                       \
                                 unsigned short: USHRT_MAX,             \
                                 int: INT_MAX,                          \
                                 unsigned int: UINT_MAX,                \
                                 long: LONG_MAX,                        \
                                 unsigned long int: ULONG_MAX,          \
                                 long long int: LLONG_MAX,              \
                                 unsigned long long int: ULLONG_MAX,    \
                                 float: FLT_MAX,                       \
                                 double: DBL_MAX,                      \
                                 long double: LDBL_MAX)

#define FSTI_MIN(agent_ind, elem, result) do {                          \
        struct fsti_agent *_agent;                                      \
        size_t *_it;                                                    \
        result = FSTI_MAX_VAL(result);                                  \
        if (agent_ind.len == 0) {                                       \
            result = NAN;                                               \
        } else {                                                        \
            for (_it = fsti_agent_ind_begin(&(agent_ind));              \
                 _it != fsti_agent_ind_end(&(agent_ind)); _it++) {      \
                _agent = fsti_agent_ind_arrp(&(agent_ind), _it);        \
                if (_agent->elem < result) result = _agent->elem;       \
            }                                                           \
        }                                                               \
    } while(0)

#define FSTI_MAX(agent_ind, elem, result) do {                          \
        struct fsti_agent *_agent;                                      \
        size_t *_it;                                                    \
        result = FSTI_MIN_VAL(result);                                  \
        if (agent_ind.len == 0) {                                       \
            result = NAN;                                               \
        } else {                                                        \
            for (_it = fsti_agent_ind_begin(&(agent_ind));              \
                 _it != fsti_agent_ind_end(&(agent_ind)); _it++) {      \
                _agent = fsti_agent_ind_arrp(&(agent_ind), _it);        \
                if (_agent->elem > result) result = _agent->elem;       \
            }                                                           \
        }                                                               \
    } while(0)

// This median function is generic but not efficient.

#define FSTI_MEDIAN(agent_ind, elem, result) do {                       \
        size_t *_it, mid;                                               \
        if (agent_ind.len > 0) {                                        \
            float *_vals = malloc(sizeof(*_vals) * agent_ind.len);      \
            FSTI_ASSERT(_vals, FSTI_ERR_NOMEM, NULL);                   \
            float *_d = _vals;                                          \
            for (_it = agent_ind.indices;                               \
                 _it < agent_ind.indices + agent_ind.len; _it++)  {     \
                *_d = agent_ind.agent_arr->agents[*_it].elem;           \
                _d++;                                                   \
            }                                                           \
            qsort(_vals, agent_ind.len, sizeof(float), fsti_cmpf);      \
            mid = agent_ind.len / 2;                                    \
            if (agent_ind.len % 2 == 0)                                 \
                result = (_vals[mid] + _vals[mid + 1]) / 2.0;           \
            else                                                        \
                result = _vals[mid];                                    \
            free(_vals);                                                \
        } else {                                                        \
            result = NAN;                                               \
        }                                                               \
    } while(0)

#define FSTI_MEAN(agent_ind, elem, result) do {                         \
        struct fsti_agent *_agent_;                                     \
        size_t *_it;                                                    \
        result = 0;                                                     \
        for (_it = fsti_agent_ind_begin(&(agent_ind));                  \
             _it != fsti_agent_ind_end(&(agent_ind)); _it++) {          \
            _agent_ = fsti_agent_ind_arrp(&(agent_ind), _it);           \
            result += _agent_->elem;                                    \
        }                                                               \
        result = (double) result / agent_ind.len;                       \
    } while(0)

#define FSTI_SUM(agent_ind, elem, result) do {                          \
        struct fsti_agent *_agent_;                                     \
        size_t *_it;                                                    \
        result = 0;                                                     \
        for (_it = fsti_agent_ind_begin(&(agent_ind));                  \
             _it != fsti_agent_ind_end(&(agent_ind)); _it++) {          \
            _agent_ = fsti_agent_ind_arrp(&(agent_ind), _it);           \
            result += _agent_->elem;                                    \
        }                                                               \
    } while(0)

#define FSTI_SIZE(agent_ind, elem, result) do {  \
        result = agent_ind.len;                 \
    } while(0)

#define FSTI_HALF(x) x /= 2


#define FSTI_FMT(x) _Generic((x),                                   \
                                 char: "%c",                            \
                                 _Bool: "%u",                           \
                                 signed char: "%hhd",                   \
                                 unsigned char: "%hhu",                 \
                                 signed short: "%hd",                   \
                                 unsigned short: "%hu",                 \
                                 signed int: "%d",                      \
                                 unsigned int: "%u",                    \
                                 long int: "%ld",                       \
                                 unsigned long int: "%lu",              \
                                 long long int: "%lld",                 \
                                 unsigned long long int: "%llu",        \
                                 float: "%f",                           \
                                 double: "%f",                          \
                                 long double: "%Lf",                    \
                                 char *: "%s",                          \
                                 void *: "%p")

#define FSTI_FMT__(x) FSTI_FMT(x)

#define FSTI_REPORT_OUTPUT_HEADER(delim)                                \
    fprintf(simulation->results_file, "%s%c%s%c%s%c%s%c%s%c%s\n",       \
            "name",delim,"sim",delim,"num",delim,                       \
            "date",delim,"description",delim,"value")

#define FSTI_REPORT_OUTPUT_POST_PREC(func, agent_ind, elem, desc, post, spec) \
    do {                                                                \
        double __result__;                                              \
        char _c = simulation->csv_delimiter;                            \
        func(simulation->agent_ind, elem, __result__);                  \
        post(__result__);                                               \
        fprintf(simulation->results_file,                               \
                "%s%c%d%c%d%c%s%c%s%c" spec "\n",                     \
                simulation->name, _c, simulation->sim_number, _c,       \
                simulation->config_sim_number, _c,                      \
                fsti_julian_date_sprint(                                \
                    fsti_to_julian_date(                                \
                        fsti_add_time_step(simulation->start_date,      \
                                           simulation->iteration,       \
                                           simulation->time_step))),    \
                _c,                                                     \
                desc, _c,  __result__);                                 \
    } while(0)

#define FSTI_REPORT_OUTPUT_PREC(func, agent_ind, elem, desc, spec)      \
        FSTI_REPORT_OUTPUT_POST_PREC(func, agent_ind, elem, desc, (void), spec)

#define FSTI_REPORT_OUTPUT(func, agent_ind, elem, desc)                 \
    FSTI_REPORT_OUTPUT_PREC(func, agent_ind, elem, desc, "%f")


int fsti_cmpf(const void *a, const void *b);

#endif
