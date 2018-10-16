#ifndef FSTI_EVENTS_H
#define FSTI_EVENTS_H

#include <stdint.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#include "fsti-simulation.h"
#include "fsti-defs.h"
#include "fsti-userdefs.h"
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

#define FSTI_REPORT_OUTPUT_POST_PREC(func, agent_ind, elem, desc, post, spec) \
    do {                                                                \
        double __result__;                                              \
        func(simulation->agent_ind, elem, __result__);                  \
        post(__result__);                                               \
        fprintf(simulation->results_file, "%s,%d,%d,%s," spec "\n",     \
                simulation->name, simulation->sim_number,               \
                simulation->config_sim_number, desc, __result__);       \
    } while(0)

#define FSTI_REPORT_OUTPUT_PREC(func, agent_ind, elem, desc, spec) do { \
        double __result__;                                              \
        func(simulation->agent_ind, elem, __result__);                  \
        fprintf(simulation->results_file, "%s,%d,%d,%s," spec "\n",     \
                simulation->name, simulation->sim_number,               \
                simulation->config_sim_number, desc, __result__);       \
    } while(0)

#define FSTI_REPORT_OUTPUT(func, agent_ind, elem, desc) do {            \
        double __result__;                                              \
        func(simulation->agent_ind, elem, __result__);                  \
        fprintf(simulation->results_file, "%s,%d,%d,%s,%f\n",           \
                simulation->name, simulation->sim_number,               \
                simulation->config_sim_number, desc, __result__);       \
    } while(0)


void fsti_to_float(void *to, const struct fsti_variant *from,
                   struct fsti_agent *agent);
void fsti_to_double(void *to, const struct fsti_variant *from,
                   struct fsti_agent *agent);
void fsti_to_int(void *to, const struct fsti_variant *from,
                   struct fsti_agent *agent);
void fsti_to_uint8_t(void *to, const struct fsti_variant *from,
                      struct fsti_agent *agent);
void fsti_to_uint16_t(void *to, const struct fsti_variant *from,
                      struct fsti_agent *agent);
void fsti_to_uint32_t(void *to, const struct fsti_variant *from,
                      struct fsti_agent *agent);
void fsti_to_uint64_t(void *to, const struct fsti_variant *from,
                      struct fsti_agent *agent);
void fsti_to_bool(void *to, const struct fsti_variant *from,
                  struct fsti_agent *agent);
void fsti_to_unsigned(void *to, const struct fsti_variant *from,
                   struct fsti_agent *agent);
void fsti_to_uchar(void *to, const struct fsti_variant *from,
                   struct fsti_agent *agent);
void fsti_to_size_t(void *to, const struct fsti_variant *from,
                     struct fsti_agent *agent);
void fsti_to_partner(void *to, const struct fsti_variant *from,
                     struct fsti_agent *agent);

void fsti_event_read_agents(struct fsti_simulation *simulation);
void fsti_event_create_agents(struct fsti_simulation *simulation);
void fsti_event_age(struct fsti_simulation *simulation);
void fsti_event_report(struct fsti_simulation *simulation);
void fsti_event_flex_report(struct fsti_simulation *simulation);
void fsti_event_shuffle_living(struct fsti_simulation *simulation);
void fsti_event_shuffle_mating(struct fsti_simulation *simulation);
void fsti_event_write_agents_csv(struct fsti_simulation *simulation);
void fsti_event_write_living_agents_csv(struct fsti_simulation *simulation);
void fsti_event_write_dead_agents_csv(struct fsti_simulation *simulation);
void fsti_event_write_agents_pretty(struct fsti_simulation *simulation);
void fsti_event_no_op(struct fsti_simulation *simulation);
void fsti_event_register_events();

extern struct fsti_agent fsti_global_agent;
extern const struct fsti_csv_entry fsti_csv_entries[];

extern const struct fsti_csv_agent fsti_global_csv;

#endif
