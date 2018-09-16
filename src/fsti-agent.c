#include <string.h>

#include "useful.h"
#include "fsti-agent.h"
#include "fsti-error.h"


//struct fsti_agent fsti_global_agent;
struct fsti_agent_arr fsti_saved_agent_arr = {NULL, 0, 0};

static struct fsti_agent * new_agent()
{
    struct fsti_agent *agent =  calloc(1, sizeof(*agent));
    FSTI_ASSERT(agent, FSTI_ERR_NOMEM, NULL);
    return agent;
}

void fsti_agent_print_csv(FILE *f, unsigned id, struct fsti_agent *agent)
{
    FSTI_AGENT_PRINT_CSV(f, id, agent);
}

void fsti_agent_print_pretty(FILE *f, unsigned id, struct fsti_agent *agent)
{
    FSTI_AGENT_PRINT_PRETTY(f, id, agent);


}

void fsti_agent_arr_init(struct fsti_agent_arr *agent_arr)
{
    agent_arr->agents = NULL;
    agent_arr->len = 0;
    agent_arr->capacity = 0;
}

inline struct fsti_agent *
fsti_agent_arr_at(struct fsti_agent_arr *agent_arr, size_t index)
{
    return agent_arr->agents[index];
}

void fsti_agent_arr_push(struct fsti_agent_arr *agent_arr,
                         struct fsti_agent *agent)
{
    if (agent_arr->len == agent_arr->capacity) {
        if (agent_arr->capacity == 0)
            agent_arr->capacity = 5;
        else
            agent_arr->capacity = (3 * agent_arr->capacity) / 2;
        agent_arr->agents = realloc(agent_arr->agents,
                                    sizeof(struct fsti_agent *) *
                                    agent_arr->capacity);
        FSTI_ASSERT(agent_arr->agents, FSTI_ERR_NOMEM, NULL);
    }
    agent_arr->agents[agent_arr->len] = agent;
    agent_arr->len++;
}

struct fsti_agent * fsti_agent_arr_pop(struct fsti_agent_arr *agent_arr)
{
    agent_arr->len--;
    return agent_arr->agents[agent_arr->len];
}

struct fsti_agent * fsti_agent_arr_remove(struct fsti_agent_arr *agent_arr,
                                   size_t index)
{
    struct fsti_agent *t;
    t = agent_arr->agents[index];
    agent_arr->agents[index] = agent_arr->agents[agent_arr->len - 1];
    agent_arr->agents[agent_arr->len - 1] = t;
    return fsti_agent_arr_pop(agent_arr);
}

void fsti_agent_arr_copy(struct fsti_agent_arr *dest,
                         struct fsti_agent_arr *src)
{
    dest->agents = malloc(sizeof(struct fsti_agent *) * src->len);
    memcpy(dest->agents, src->agents, src->len);
    dest->capacity = dest->len = src->len;
}


/*
  Deep copying the agents is complicated because the partnerships
  are recorded as pointers which cannot be naively copied.
 */

void fsti_agent_arr_deep_copy(struct fsti_agent_arr *dest,
                              struct fsti_agent_arr *src)
{
    struct partnerships {
        struct fsti_agent *partners[FSTI_MAX_PARTNERS];
        size_t indices[FSTI_MAX_PARTNERS];
    };

    struct partnerships *p = calloc(src->len, sizeof(*p));

    dest->agents = malloc(src->len * sizeof(struct fsti_agent *));
    FSTI_ASSERT(dest->agents, FSTI_ERR_NOMEM, NULL);
    dest->capacity = dest->len = src->len;

    for (size_t i = 0; i < src->len; ++i) {
        dest->agents[i] = new_agent();
        memcpy(dest->agents[i], src->agents[i], sizeof(struct fsti_agent));
        memset(dest->agents[i]->partners, 0,
               FSTI_MAX_PARTNERS * sizeof(struct fsti_agent *));
        // Record this agent's partnerships
        for (size_t j = 0; j < FSTI_MAX_PARTNERS; j++) {
            if (src->agents[i]->partners[j]) {
                struct fsti_agent *a = src->agents[i]->partners[j];
                size_t index = a->id - 1;
                for (size_t k = 0; k < FSTI_MAX_PARTNERS; k++) {
                    if (a->partners[k] == src->agents[i]) {
                        p[index].partners[k] = dest->agents[i];
                        p[index].indices[k] = j;
                        break;
                    }
                }
            }
        }
    }

    // Set the partnerships
    for (size_t i = 0; i < dest->len; ++i) {
        size_t index = dest->agents[i]->id - 1;
        for (size_t j = 0; j < FSTI_MAX_PARTNERS; j++) {
            if (p[index].partners[j]) {
                size_t k = p[index].indices[j];
                dest->agents[i]->partners[k] = p[index].partners[j];
            }
        }
    }

    free(p);
}

struct fsti_agent *
fsti_agent_arr_new_agent(struct fsti_agent_arr *agent_arr)
{
    struct fsti_agent *agent = new_agent();
    fsti_agent_arr_push(agent_arr, agent);
    return agent;
}


inline size_t fsti_agent_arr_count(struct fsti_agent_arr *agent_arr)
{
    return agent_arr->len;
}

void fsti_agent_arr_free(struct fsti_agent_arr *agent_arr)
{
    for (size_t i = 0; i < agent_arr->len; ++i)
        free(agent_arr->agents[i]);
    free(agent_arr->agents);
}
