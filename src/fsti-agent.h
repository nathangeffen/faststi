#ifndef FSTI_AGENT
#define FSTI_AGENT

#include <stdio.h>

#include "utils/utils.h"
#include "fsti-defs.h"
#include "fsti-userdefs.h"

#define FSTI_LOOP_AGENTS_INDEX(agent_arr, agent, i, code) do {          \
        size_t *_it;                                                    \
        for (_it = fsti_agent_arr_begin(&(agent_arr)), i = 0;           \
             _it != fsti_agent_arr_end(&(agent_arr)); _it++, i++) {     \
            agent = fsti_agent_arr_at(&(agent_arr), _it);              \
            { code }                                                    \
        }                                                               \
    } while(0)


#define FSTI_LOOP_AGENTS(agent_arr, agent, code) do {                   \
        size_t *_it;                                                    \
        for (_it = fsti_agent_arr_begin(&(agent_arr));                  \
             _it != fsti_agent_arr_end(&(agent_arr)); _it++) {          \
            agent = fsti_agent_arr_at(&(agent_arr), _it);              \
            { code }                                                    \
        }                                                               \
    } while(0)




struct fsti_agent {
    size_t id;
    unsigned char sex;
    unsigned char sex_preferred;
    float age;
    float infected; // Date of last infection, else 0
    float cured; // Date last cured of last infection
    float date_death; // 0 if still alive
    unsigned char cause_of_death;
    size_t partners[FSTI_MAX_PARTNERS];
    size_t num_partners;
    FSTI_AGENT_FIELDS
};

struct fsti_agent_arr {
    struct fsti_agent *agents;
    size_t *indices;
    size_t len;
    size_t capacity;
    struct fsti_agent_arr *owner;
};

extern struct fsti_agent_arr fsti_saved_agent_arr;

void fsti_agent_print_csv(FILE *f, unsigned id, struct fsti_agent *agent);
void fsti_agent_print_pretty(FILE *f, unsigned id, struct fsti_agent *agent);
bool fsti_agent_has_partner(const struct fsti_agent *agent);
void fsti_agent_make_partners(struct fsti_agent *a, struct fsti_agent *b);
void fsti_agent_break_partners(struct fsti_agent *a, struct fsti_agent *b);
void fsti_agent_arr_init(struct fsti_agent_arr *agent_arr, struct fsti_agent_arr *owner);
void fsti_agent_arr_init_n(struct fsti_agent_arr *agent_arr, size_t n, struct fsti_agent_arr *owner);
size_t *fsti_agent_arr_begin(struct fsti_agent_arr *agent_arr);
size_t *fsti_agent_arr_end(struct fsti_agent_arr *agent_arr);
struct fsti_agent *fsti_agent_arr_at(struct fsti_agent_arr *agent_arr, size_t *it);
struct fsti_agent *fsti_agent_arr_front(struct fsti_agent_arr *agent_arr);
struct fsti_agent *fsti_agent_arr_back(struct fsti_agent_arr *agent_arr);
struct fsti_agent *fsti_agent_partner_at(struct fsti_agent_arr *agent_arr, struct fsti_agent *agent, size_t index);
struct fsti_agent *fsti_agent_partner_at0(struct fsti_agent_arr *agent_arr, struct fsti_agent *agent);
float fsti_agent_default_distance(const struct fsti_agent *a, const struct fsti_agent *b);
void fsti_agent_arr_push_index(struct fsti_agent_arr *agent_arr, size_t index);
size_t fsti_agent_arr_pop_index(struct fsti_agent_arr *agent_arr);
void fsti_agent_remove(struct fsti_agent_arr *agent_arr, size_t index);
void fsti_agent_arr_push_agent(struct fsti_agent_arr *agent_arr, struct fsti_agent *agent);
struct fsti_agent *fsti_agent_arr_pop_agent(struct fsti_agent_arr *agent_arr);
void fsti_agent_arr_copy(struct fsti_agent_arr *dest, struct fsti_agent_arr *src);
void fsti_agent_arr_clear(struct fsti_agent_arr *agent_arr);
void fsti_agent_arr_free_indices(struct fsti_agent_arr *agent_arr);
void fsti_agent_arr_free(struct fsti_agent_arr *agent_arr);
void fsti_agent_test(struct test_group *tg);

#endif
