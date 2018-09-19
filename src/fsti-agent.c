#include <string.h>
#include <math.h>

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
    if (agent->num_partners)
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
    assert(a->num_partners && b->num_partners);
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

void fsti_agent_arr_add_dependency(struct fsti_agent_arr *owner,
                                   struct fsti_agent_arr *dependent)
{
    assert(owner->owner == NULL);
    dependent->agents = owner->agents;
    dependent->dependency = owner->dependency;
    owner->dependency = dependent;
}

void fsti_agent_arr_init(struct fsti_agent_arr *agent_arr,
                         struct fsti_agent_arr *owner)
{
    agent_arr->agents = NULL;
    agent_arr->indices = NULL;
    agent_arr->len = 0;
    agent_arr->capacity = 0;
    agent_arr->dependency = NULL;
    agent_arr->owner = owner;
    if (owner) fsti_agent_arr_add_dependency(owner, agent_arr);
}

void fsti_agent_arr_init_n(struct fsti_agent_arr *agent_arr, size_t n,
                           struct fsti_agent_arr *owner)
{
    agent_arr->owner = owner;
    agent_arr->dependency = NULL;
    agent_arr->len = agent_arr->capacity = n;
    agent_arr->indices = calloc(n, sizeof(size_t));
    FSTI_ASSERT(agent_arr->indices, FSTI_ERR_NOMEM, NULL);
    if (owner == NULL) {
        agent_arr->agents = calloc(n, sizeof(struct fsti_agent));
        FSTI_ASSERT(agent_arr->agents, FSTI_ERR_NOMEM, NULL);
        for (size_t i = 0; i < n; i++)
            agent_arr->indices[i] = agent_arr->agents[i].id = i;
    } else {
        fsti_agent_arr_add_dependency(owner, agent_arr);
    }
}


size_t * fsti_agent_arr_begin(struct fsti_agent_arr *agent_arr)
{
    return agent_arr->indices;
}

size_t * fsti_agent_arr_end(struct fsti_agent_arr *agent_arr)
{
    return agent_arr->indices + agent_arr->len;
}


struct fsti_agent * fsti_agent_arr_at(struct fsti_agent_arr *agent_arr,
                                      size_t *it)
{
    return agent_arr->agents + *it;
}

struct fsti_agent * fsti_agent_arr_front(struct fsti_agent_arr *agent_arr)
{
    return fsti_agent_arr_at(agent_arr, fsti_agent_arr_begin(agent_arr));
}

struct fsti_agent * fsti_agent_arr_back(struct fsti_agent_arr *agent_arr)
{
    return fsti_agent_arr_at(agent_arr, fsti_agent_arr_end(agent_arr) - 1);
}

struct fsti_agent *fsti_agent_partner_at(struct fsti_agent_arr *agent_arr,
                                         struct fsti_agent *agent,
                                         size_t index)
{
    return fsti_agent_arr_at(agent_arr, agent->partners + index);
}

struct fsti_agent *fsti_agent_partner_at0(struct fsti_agent_arr *agent_arr,
                                          struct fsti_agent *agent)
{
    return fsti_agent_partner_at(agent_arr, agent, 0);
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

static void allocate(struct fsti_agent_arr *agent_arr)
{
    struct fsti_agent_arr *it;
    if (agent_arr->capacity == 0)
        agent_arr->capacity = 5;
    else
        agent_arr->capacity = (3 * agent_arr->capacity) / 2;
    if (agent_arr->owner == NULL) {
        agent_arr->agents = realloc(agent_arr->agents,
                                    sizeof(struct fsti_agent) *
                                    agent_arr->capacity);
        FSTI_ASSERT(agent_arr->agents, FSTI_ERR_NOMEM, NULL);
        for (it = agent_arr->dependency; it; it = it->dependency) {
            it->agents = agent_arr->agents;
        }
    }
    agent_arr->indices = realloc(agent_arr->indices,
                                 sizeof(size_t) *
                                 agent_arr->capacity);
    FSTI_ASSERT(agent_arr->indices, FSTI_ERR_NOMEM, NULL);
}

void fsti_agent_arr_push_index(struct fsti_agent_arr *agent_arr,
                               size_t index)
{
    if (agent_arr->len == agent_arr->capacity)
        allocate(agent_arr);

    agent_arr->indices[agent_arr->len] = index;
    ++agent_arr->len;
}

size_t fsti_agent_arr_pop_index(struct fsti_agent_arr *agent_arr)
{
    assert(agent_arr->len);
    --agent_arr->len;
    return agent_arr->indices[agent_arr->len];
}

void fsti_agent_remove(struct fsti_agent_arr *agent_arr,
                       size_t index)
{
    size_t t;
    --agent_arr->len;
    t = agent_arr->indices[index];
    agent_arr->indices[index] = agent_arr->indices[agent_arr->len];
    agent_arr->indices[agent_arr->len] = t;
}


void fsti_agent_arr_push_agent(struct fsti_agent_arr *agent_arr,
                               struct fsti_agent *agent)
{
    // Only agent_arrs that own their agents can call this
    assert(agent_arr->owner == NULL);

    if (agent_arr->len == agent_arr->capacity)
        allocate(agent_arr);

    if (agent)
        agent_arr->agents[agent_arr->len] = *agent;
    else
        memset(agent_arr->agents + agent_arr->len, 0, sizeof(struct fsti_agent));

    agent_arr->agents[agent_arr->len].id = agent_arr->len;
    agent_arr->indices[agent_arr->len] = agent_arr->len;
    agent_arr->len++;
}

struct fsti_agent * fsti_agent_arr_pop_agent(struct fsti_agent_arr *agent_arr)
{
    agent_arr->len--;
    return agent_arr->agents + agent_arr->len;
}

void fsti_agent_arr_copy(struct fsti_agent_arr *dest,
                         struct fsti_agent_arr *src)
{
    dest->agents = malloc(src->len * sizeof(struct fsti_agent));
    FSTI_ASSERT(dest->agents, FSTI_ERR_NOMEM, NULL);
    dest->indices = malloc(src->len * sizeof(size_t));
    FSTI_ASSERT(dest->indices, FSTI_ERR_NOMEM, NULL);
    dest->capacity = dest->len = src->len;
    dest->owner = dest->dependency = NULL;

    memcpy(dest->indices, src->indices, sizeof(size_t) * src->len);
    memcpy(dest->agents, src->agents, sizeof(struct fsti_agent) * src->len);

    FSTI_AGENT_ARR_COPY(dest, src);
}

inline size_t fsti_agent_arr_count(struct fsti_agent_arr *agent_arr)
{
    return agent_arr->len;
}

void fsti_agent_arr_clear(struct fsti_agent_arr *agent_arr)
{
    agent_arr->len = 0;
}

void fsti_agent_arr_free_indices(struct fsti_agent_arr *agent_arr)
{
    free(agent_arr->indices);
    agent_arr->len = agent_arr->capacity = 0;
}

void fsti_agent_arr_free(struct fsti_agent_arr *agent_arr)
{
    FSTI_AGENT_FREE(agent_arr);
    if (agent_arr->owner == NULL) free(agent_arr->agents);
    fsti_agent_arr_free_indices(agent_arr);
}

void fsti_agent_test(struct test_group *tg)
{
    struct fsti_agent_arr agent_arr, mating_pool, copy_to;
    struct fsti_agent *agent;
    size_t i;
    size_t *it, id;

    fsti_agent_arr_init_n(&agent_arr, 101, NULL);
    fsti_agent_arr_init(&mating_pool, &agent_arr);


    TESTEQ(agent_arr.indices[agent_arr.len - 1],
           (agent_arr.agents + agent_arr.len - 1)->id, *tg);

    bool correct = true;
    id = 0;
    for (agent = agent_arr.agents; agent != agent_arr.agents + agent_arr.len;
         agent++) {
        if (agent->id == id) {
            id++;
        } else {
            correct = false;
        }
    }
    TESTEQ(id, 101, *tg);
    TESTEQ(correct, true, *tg);

    correct = true;
    id = 0;
    for (it = fsti_agent_arr_begin(&agent_arr);
         it < fsti_agent_arr_end(&agent_arr); it++) {
        if (fsti_agent_arr_at(&agent_arr, it)->id == id) {
            id++;
        } else {
            correct = false;
        }
    }
    TESTEQ(id, 101, *tg);
    TESTEQ(correct, true, *tg);

    struct fsti_agent a;
    memset(&a, 0, sizeof(struct fsti_agent));
    a.infected = true;
    fsti_agent_arr_push_agent(&agent_arr, &a);
    correct = true;
    id = 0;
    for (it = fsti_agent_arr_begin(&agent_arr);
         it < fsti_agent_arr_end(&agent_arr); it++) {
        if (fsti_agent_arr_at(&agent_arr, it)->id == id) {
            id++;
        } else {
            correct = false;
        }
    }
    TESTEQ(id, 102, *tg);
    TESTEQ(correct, true, *tg);
    TESTEQ(fsti_agent_arr_back(&agent_arr)->infected, true, *tg);

    correct = true;
    id = 0;
    FSTI_LOOP_AGENTS(agent_arr, agent, {
            if (agent->id == id) {
                id++;
            } else {
                correct = false;
            }
        });
    TESTEQ(id, 102, *tg);
    TESTEQ(correct, true, *tg);

    correct = true;
    id = 0;

    FSTI_LOOP_AGENTS_INDEX(agent_arr, agent, i, {
            if (agent->id != i) correct = false;
        });
    TESTEQ(correct, true, *tg);

    FSTI_LOOP_AGENTS_INDEX(agent_arr, agent, i, {
            fsti_agent_arr_push_index(&mating_pool, agent_arr.indices[i]);
        });

    TESTEQ(mating_pool.len, agent_arr.len, *tg);

    correct = true;
    for (it = fsti_agent_arr_begin(&agent_arr);
         it < fsti_agent_arr_end(&agent_arr); ++it) {
        if (fsti_agent_arr_at(&agent_arr, it) !=
            fsti_agent_arr_at(&mating_pool, it))
            correct = false;
    }
    TESTEQ(correct, true, *tg);

    fsti_agent_make_partners(fsti_agent_arr_front(&mating_pool),
                             fsti_agent_arr_back(&mating_pool));
    TESTEQ(fsti_agent_arr_front(&mating_pool)->partners[0],
           fsti_agent_arr_back(&mating_pool)->id, *tg);
    TESTEQ(fsti_agent_arr_back(&mating_pool)->partners[0],
           fsti_agent_arr_front(&mating_pool)->id, *tg);
    TESTEQ(fsti_agent_partner_at(&mating_pool,
                                     fsti_agent_arr_front(&mating_pool), 0),
           fsti_agent_arr_back(&mating_pool), *tg);

    fsti_agent_arr_copy(&copy_to, &agent_arr);
    correct = true;
    for (it=fsti_agent_arr_begin(&copy_to); it<fsti_agent_arr_end(&copy_to);
         it++) {
        if(fsti_agent_arr_at(&copy_to, it)->id !=
           fsti_agent_arr_at(&copy_to, it)->id)
            correct = false;
    }

    fsti_agent_arr_free(&copy_to);
    fsti_agent_arr_free(&mating_pool);
    fsti_agent_arr_free(&agent_arr);
}
