#ifndef FSTI_AGENT
#define FSTI_AGENT

#include <stdio.h>

#include "fsti-defs.h"
#include "fsti-userdefs.h"

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
float fsti_agent_default_distance(const struct fsti_agent *a,
                                  const struct fsti_agent *b);
void fsti_agent_arr_init(struct fsti_agent_arr *agent_arr);
void fsti_agent_arr_push(struct fsti_agent_arr *agent_arr, struct fsti_agent *agent);
struct fsti_agent * fsti_agent_arr_pop(struct fsti_agent_arr *agent_arr);
struct fsti_agent * fsti_agent_arr_remove(struct fsti_agent_arr *agent_arr,
                                          size_t index);
void fsti_agent_arr_copy(struct fsti_agent_arr *dest, struct fsti_agent_arr *src);
void fsti_agent_arr_deep_copy(struct fsti_agent_arr *dest, struct fsti_agent_arr *src);
struct fsti_agent *fsti_agent_arr_new_agent(struct fsti_agent_arr *agent_arr);
void fsti_agent_arr_clear(struct fsti_agent_arr *agent_arr);
void fsti_agent_arr_free(struct fsti_agent_arr *agent_arr);

#endif
