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
    unsigned long id;
    size_t len;
    size_t capacity;
    char data[];
};

#ifndef FSTI_CUSTOM_AGENT

struct fsti_agent_default_data {
    double age;
    bool infected;
    int sex;
    int sex_preferred;
};

static const struct fsti_agent_csv_entry fsti_default_csv_entries[] = {
    FSTI_AGENT_FIELD(id, LONG),
    FSTI_AGENT_DATA_FIELD(struct fsti_agent_default_data, age, DBL),
    FSTI_AGENT_DATA_FIELD(struct fsti_agent_default_data, infected, UNSIGNED),
    FSTI_AGENT_DATA_FIELD(struct fsti_agent_default_data, sex, UNSIGNED),
    FSTI_AGENT_DATA_FIELD(struct fsti_agent_default_data,
                          sex_preferred, UNSIGNED)
};


#endif

struct fsti_agent_arr {
    struct fsti_agent **agents;
    size_t agent_size;
    size_t len;
    size_t capacity;
};

extern struct fsti_agent_arr fsti_saved_agent_arr;

void fsti_agent_arr_init(struct fsti_agent_arr *agent_arr, size_t data_size);
void fsti_agent_arr_push(struct fsti_agent_arr *agent_arr, struct fsti_agent *agent);
struct fsti_agent * fsti_agent_arr_pop(struct fsti_agent_arr *agent_arr);
struct fsti_agent * fsti_agent_arr_remove(struct fsti_agent_arr *agent_arr,
                                          size_t index);
void fsti_agent_arr_copy(struct fsti_agent_arr *dest, struct fsti_agent_arr *src);
void fsti_agent_arr_deep_copy(struct fsti_agent_arr *dest, struct fsti_agent_arr *src);
struct fsti_agent *fsti_agent_arr_new_agent(struct fsti_agent_arr *agent_arr);
void fsti_agent_arr_free(struct fsti_agent_arr *agent_arr);


#endif
