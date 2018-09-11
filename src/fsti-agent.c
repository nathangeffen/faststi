#include <string.h>

#include "fsti-agent.h"
#include "fsti-error.h"

struct fsti_agent_arr fsti_saved_agent_arr = {NULL, 0, 0};

static struct fsti_agent * new_agent(size_t bytes)
{
    struct fsti_agent *agent =  malloc(bytes);
    FSTI_ASSERT(agent, FSTI_ERR_NOMEM, NULL);
    return agent;
}

inline
size_t fsti_agent_arr_size(struct fsti_agent_arr *agent_arr)
{
    return agent_arr->agent_size;
}


void fsti_agent_arr_init(struct fsti_agent_arr *agent_arr, size_t data_size)
{
    agent_arr->agents = NULL;
    agent_arr->len = 0;
    agent_arr->capacity = 0;
    agent_arr->agent_size = sizeof(struct fsti_agent) + data_size;
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


void fsti_agent_arr_deep_copy(struct fsti_agent_arr *dest,
                              struct fsti_agent_arr *src)
{
    dest->agents = malloc(sizeof(struct fsti_agent *) * src->len);
    FSTI_ASSERT(dest->agents, FSTI_ERR_NOMEM, NULL);
    for (size_t i = 0; i < src->len; ++i) {
        dest->agents[i] = new_agent(src->agent_size);
        memcpy(dest->agents[i], src->agents[i], src->agent_size);
    }
    dest->capacity = dest->len = src->len;
}

struct fsti_agent *
fsti_agent_arr_new_agent(struct fsti_agent_arr *agent_arr)
{
    struct fsti_agent *agent = new_agent(agent_arr->agent_size);
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
