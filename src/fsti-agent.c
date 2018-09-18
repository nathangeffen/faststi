#include <string.h>
#include <math.h>

#include "utils/utils.h"
#include "fsti-agent.h"
#include "fsti-error.h"


//struct fsti_agent fsti_global_agent;
struct fsti_agent_arr fsti_saved_agent_arr = {NULL, 0, 0};

void fsti_agent_print_csv(FILE *f, unsigned id, struct fsti_agent *agent)
{
    FSTI_AGENT_PRINT_CSV(f, id, agent);
}

void fsti_agent_print_pretty(FILE *f, unsigned id, struct fsti_agent *agent)
{
    FSTI_AGENT_PRINT_PRETTY(f, id, agent);


}

bool fsti_agent_has_partner(const struct fsti_agent *agent)
{
    if (agent->partners[0])
        return true;
    else
        return false;
}

void fsti_agent_make_partners(struct fsti_agent *a, struct fsti_agent *b)
{
    a->partners[a->num_partners++] = b->id;
    b->partners[b->num_partners++] = a->id;
}

void fsti_agent_break_partners(struct fsti_agent *a, struct fsti_agent *b)
{
    size_t n;
    n = a->num_partners;
    for (size_t i = 0; i < n; i++)
        if (a->partners[i] == b->id) {
            a->partners[i] = a->partners[a->num_partners - 1];
            a->num_partners--;
            break;
        }
    n = b->num_partners;
    for (size_t i = 0; i < n; i++)
        if (b->partners[i] == a->id) {
            b->partners[i] = b->partners[b->num_partners - 1];
            b->num_partners--;
            break;
        }
}

struct fsti_agent *fsti_get_partner(struct fsti_agent_arr *agent_arr,
                                    struct fsti_agent *agent,
                                    size_t index)
{
    assert(agent->partners[index]);
    return (agent_arr->first + agent->partners[index] - 1);
}

struct fsti_agent *fsti_get_partner_at0(struct fsti_agent_arr *agent_arr,
                                        struct fsti_agent *agent)
{
    return fsti_get_partner(agent_arr, agent, 0);
}


float fsti_agent_default_distance(const struct fsti_agent *a,
                                  const struct fsti_agent *b)
{
    float result = 0.0;
    if (a->sex_preferred != b->sex)
        result += 25.0;
    if (b->sex_preferred != a->sex)
        result += 25.0;
    result += fabs(a->age - b->age);
    return result;
}

void fsti_agent_arr_init(struct fsti_agent_arr *agent_arr)
{
    agent_arr->agents = NULL;
    agent_arr->first = NULL;
    agent_arr->len = 0;
    agent_arr->capacity = 0;
}

void fsti_agent_arr_init_n(struct fsti_agent_arr *agent_arr, size_t n)
{
    agent_arr->len = agent_arr->capacity = n;
    agent_arr->agents = calloc(n, sizeof(struct fsti_agent *));
    FSTI_ASSERT(agent_arr->agents, FSTI_ERR_NOMEM, NULL);
    agent_arr->first = calloc(n, sizeof(struct fsti_agent));
    FSTI_ASSERT(agent_arr->first, FSTI_ERR_NOMEM, NULL);
    for (size_t i = 0; i < n; i++)
        agent_arr->agents[i] = (agent_arr->first + i);
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
    dest->first = src->first;
    dest->capacity = dest->len = src->len;
}

void fsti_agent_arr_deep_copy(struct fsti_agent_arr *dest,
                              struct fsti_agent_arr *src)
{
    dest->first = calloc(src->len, sizeof(struct fsti_agent));
    FSTI_ASSERT(dest->first, FSTI_ERR_NOMEM, NULL);
    dest->agents = malloc(src->len * sizeof(struct fsti_agent *));
    FSTI_ASSERT(dest->agents, FSTI_ERR_NOMEM, NULL);
    dest->capacity = dest->len = src->len;

    for (size_t i = 0; i < src->len; ++i) {
        dest->agents[i] = dest->first + i;
        memcpy(&dest->first[i], src->agents[i], sizeof(struct fsti_agent));
    }
    FSTI_DEEP_COPY(dest, src);
}

struct fsti_agent *
fsti_agent_arr_new_agent(struct fsti_agent_arr *agent_arr)
{
    agent_arr->first = realloc(agent_arr->first, agent_arr->len + 1);
    fsti_agent_arr_push(agent_arr, agent_arr->first + agent_arr->len);
    return agent_arr->first + agent_arr->len - 1;
}


inline size_t fsti_agent_arr_count(struct fsti_agent_arr *agent_arr)
{
    return agent_arr->len;
}

void fsti_agent_arr_clear(struct fsti_agent_arr *agent_arr)
{
    agent_arr->len = 0;
    agent_arr->capacity = 0;
}

void fsti_agent_arr_free_pointers(struct fsti_agent_arr *agent_arr)
{
    free(agent_arr->agents);
    agent_arr->len = agent_arr->capacity = 0;
}

void fsti_agent_arr_free(struct fsti_agent_arr *agent_arr)
{
    FSTI_AGENT_FREE(agent_arr);
    free(agent_arr->first);
    fsti_agent_arr_free_pointers(agent_arr);
}
