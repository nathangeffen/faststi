#ifndef FSTI_AGENT
#define FSTI_AGENT

#include "fsti-defs.h"

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
    void *data; // Users can use this as they please. Initialized to NULL.
#ifdef FSTI_USER_AGENT_FIELDS
    FSTI_AGENT_FIELDS
#endif
};

extern struct fsti_agent fsti_global_agent;

static const struct fsti_agent_csv_entry fsti_default_csv_entries[] = {
    FSTI_AGENT_FIELD(id, LONG),
    FSTI_AGENT_FIELD(age, FLOAT),
    FSTI_AGENT_FIELD(infected, FLOAT),
    FSTI_AGENT_FIELD(sex, UCHAR),
    FSTI_AGENT_FIELD(sex_preferred, UCHAR)
};


struct fsti_agent_arr {
    struct fsti_agent **agents;
    size_t len;
    size_t capacity;
};

extern struct fsti_agent_arr fsti_saved_agent_arr;

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
