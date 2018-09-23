#ifndef FSTI_AGENT
#define FSTI_AGENT

#include <stdio.h>
#include <gsl/gsl_rng.h>

#include "utils/utils.h"
#include "fsti-defs.h"
#include "fsti-userdefs.h"

#define FSTI_FOR(agent_ind, agent, code) do {                           \
        size_t *_it;                                                    \
        for (_it = fsti_agent_ind_begin(&(agent_ind));                  \
             _it != fsti_agent_ind_end(&(agent_ind)); _it++) {          \
            agent = fsti_agent_ind_arrp(&(agent_ind), _it);             \
            { code }                                                    \
        }                                                               \
    } while(0)

#define FSTI_FOR_IT(agent_ind, it, code) do {                           \
        for (it = fsti_agent_ind_begin(&(agent_ind));                   \
             it != fsti_agent_ind_end(&(agent_ind)); it++) {            \
            { code }                                                    \
        }                                                               \
    } while(0)

struct fsti_agent {
    size_t id;
    unsigned char sex;
    unsigned char sex_preferred;
    float age;
    float infected; // Date of last infection (or 1.0 for simplicity), else 0
    float cured; // Date last cured of last infection
    float date_death; // 0 if still alive
    unsigned char cause_of_death;
    size_t partners[FSTI_MAX_PARTNERS];
    size_t num_partners;
    FSTI_AGENT_FIELDS
};

struct fsti_ind_list {
    struct fsti_agent_ind *ind;
    struct fsti_ind_list *next;
};

struct fsti_agent_arr {
    struct fsti_agent *agents;
    size_t len;
    size_t capacity;
    struct fsti_ind_list *ind_list;
};

struct fsti_agent_ind {
    size_t *indices;
    size_t len;
    size_t capacity;
    struct fsti_agent_arr *agent_arr;
};

extern struct fsti_agent_arr fsti_saved_agent_arr;

void fsti_agent_print_csv(FILE *f, unsigned id, struct fsti_agent *agent);
void fsti_agent_print_pretty(FILE *f, unsigned id, struct fsti_agent *agent);
_Bool fsti_agent_has_partner(const struct fsti_agent *agent);
void fsti_agent_make_half_partner(struct fsti_agent *a, struct fsti_agent *b);
void fsti_agent_make_partners(struct fsti_agent *a, struct fsti_agent *b);
void fsti_agent_break_half_partner(struct fsti_agent *a, struct fsti_agent *b);
void fsti_agent_break_partners(struct fsti_agent *a, struct fsti_agent *b);
float fsti_agent_default_distance(const struct fsti_agent *a, const struct fsti_agent *b);
void fsti_agent_arr_add_dependency(struct fsti_agent_arr *agent_arr, struct fsti_agent_ind *agent_ind);
void fsti_agent_arr_fill_n(struct fsti_agent_arr *agent_arr,  size_t n);
void fsti_agent_arr_init(struct fsti_agent_arr *agent_arr);
void fsti_agent_arr_init_n(struct fsti_agent_arr *agent_arr, size_t n);
struct fsti_agent *fsti_agent_arr_begin(struct fsti_agent_arr *agent_arr);
struct fsti_agent *fsti_agent_arr_end(struct fsti_agent_arr *agent_arr);
struct fsti_agent *fsti_agent_arr_at(struct fsti_agent_arr *agent_arr, size_t index);
struct fsti_agent *fsti_agent_arr_front(struct fsti_agent_arr *agent_arr);
struct fsti_agent *fsti_agent_arr_back(struct fsti_agent_arr *agent_arr);
struct fsti_agent *fsti_agent_partner_get(struct fsti_agent_arr *agent_arr, struct fsti_agent *agent, size_t index);
struct fsti_agent *fsti_agent_partner_get0(struct fsti_agent_arr *agent_arr, struct fsti_agent *agent);
void fsti_agent_arr_push(struct fsti_agent_arr *agent_arr, const struct fsti_agent *agent);
struct fsti_agent *fsti_agent_arr_pop(struct fsti_agent_arr *agent_arr);
void fsti_agent_ind_fill_n(struct fsti_agent_ind *agent_ind,  size_t n);
void fsti_agent_ind_init_n(struct fsti_agent_ind *agent_ind,
                           struct fsti_agent_arr *agent_arr,
                           size_t n);
void fsti_agent_ind_init(struct fsti_agent_ind *agent_ind,
                         struct fsti_agent_arr *agent_arr);
size_t *fsti_agent_ind_begin(struct fsti_agent_ind *agent_ind);
size_t *fsti_agent_ind_end(struct fsti_agent_ind *agent_ind);
size_t *fsti_agent_ind_at(struct fsti_agent_ind *agent_ind, size_t index);
void fsti_agent_ind_push(struct fsti_agent_ind *agent_ind, size_t index);
size_t fsti_agent_ind_pop(struct fsti_agent_ind *agent_ind);
void fsti_agent_ind_remove(struct fsti_agent_ind *agent_ind, size_t *it);
struct fsti_agent * fsti_agent_ind_arr(struct fsti_agent_ind *agent_ind,
                                       size_t index);
struct fsti_agent * fsti_agent_ind_arrp(struct fsti_agent_ind *agent_ind,
                                        size_t *it);
struct fsti_agent * fsti_agent_ind_arr_front(struct fsti_agent_ind *agent_ind);
struct fsti_agent * fsti_agent_ind_arr_back(struct fsti_agent_ind *agent_ind);
void fsti_agent_arr_copy(struct fsti_agent_arr *dest,
                         struct fsti_agent_arr *src,
                         bool reset_members);
void fsti_agent_ind_shuffle(struct fsti_agent_ind *agent_ind, gsl_rng *rng);
void fsti_agent_ind_copy(struct fsti_agent_ind *dest, struct fsti_agent_ind *src);
void fsti_agent_ind_clear(struct fsti_agent_ind *agent_ind);
void fsti_agent_ind_free(struct fsti_agent_ind *agent_ind);
void fsti_agent_arr_free(struct fsti_agent_arr *agent_arr);
void fsti_agent_test(struct test_group *tg);

#endif
