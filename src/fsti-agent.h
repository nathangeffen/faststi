#ifndef FSTI_AGENT
#define FSTI_AGENT

#include <stdio.h>

#include "fsti-defs.h"
#include "fsti-userdefs.h"

#define FSTI_LOOP_AGENTS(agent_arr, agent_var_decl, code) do {          \
        for (size_t __i__ = 0; i < (agent_arr).len; __i__++)   {        \
            agent_var_decl = (agent_arr).agents[__i__];                 \
            code;                                                       \
        }                                                               \
    } while(0)

struct fsti_agent {
    unsigned id;
    unsigned char sex;
    unsigned char sex_preferred;
    float age;
    float infected; // Date of last infection, else 0
    float cured; // Date last cured of last infection
    float date_death; // 0 if still alive
    unsigned char cause_of_death;
    struct fsti_agent *partners[FSTI_MAX_PARTNERS];
    FSTI_AGENT_FIELDS
};

struct fsti_agent_arr {
    struct fsti_agent **agents;
    size_t len;
    size_t capacity;
};

extern struct fsti_agent_arr fsti_saved_agent_arr;

void fsti_agent_print_csv(FILE *f, unsigned id, struct fsti_agent *agent);
void fsti_agent_print_pretty(FILE *f, unsigned id, struct fsti_agent *agent);
bool fsti_agent_has_partner(const struct fsti_agent *agent);
void fsti_agent_arr_init(struct fsti_agent_arr *agent_arr);
void fsti_agent_arr_push(struct fsti_agent_arr *agent_arr, struct fsti_agent *agent);
struct fsti_agent * fsti_agent_arr_pop(struct fsti_agent_arr *agent_arr);
struct fsti_agent * fsti_agent_arr_remove(struct fsti_agent_arr *agent_arr,
                                          size_t index);
void fsti_agent_arr_copy(struct fsti_agent_arr *dest, struct fsti_agent_arr *src);
void fsti_agent_arr_deep_copy(struct fsti_agent_arr *dest, struct fsti_agent_arr *src);
struct fsti_agent *fsti_agent_arr_new_agent(struct fsti_agent_arr *agent_arr);
void fsti_agent_arr_free(struct fsti_agent_arr *agent_arr);

#endif
