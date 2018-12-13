#include <string.h>
#include <math.h>

#include "fsti-agent.h"
#include "fsti-error.h"

struct fsti_agent_arr fsti_saved_agent_arr = {NULL, 0, 0, NULL};

_Thread_local struct fsti_agent fsti_thread_local_agent;

static struct fsti_agent_elem agent_elem[] = FSTI_AGENT_ELEM;

const size_t fsti_agent_elem_n =
    sizeof(agent_elem) / sizeof(struct fsti_agent_elem);

// Compare two strings (for qsort and bsearch)
static int cmps(const void *a, const void *b)
{
    struct fsti_agent_elem *x = (struct fsti_agent_elem *) a;
    struct fsti_agent_elem *y = (struct fsti_agent_elem *) b;
    return strcmp(x->name, y->name);
}

void fsti_agent_elem_init()
{
    qsort(agent_elem, fsti_agent_elem_n,  sizeof(struct fsti_agent_elem), cmps);
}

inline struct fsti_agent_elem *fsti_agent_elem_get()
{
    return agent_elem;
}

bool fsti_agent_has_partner(const struct fsti_agent *agent)
{
    if (agent->num_partners)
        return true;
    else
        return false;
}


void fsti_agent_make_half_partner(struct fsti_agent *a, struct fsti_agent *b)
{
    a->partners[a->num_partners++] = b->id;
}

void fsti_agent_make_partners(struct fsti_agent *a, struct fsti_agent *b)
{
    fsti_agent_make_half_partner(a, b);
    fsti_agent_make_half_partner(b, a);
    FSTI_HOOK_AFTER_MAKE_PARTNERS(a, b);
}

void fsti_agent_break_half_partner(struct fsti_agent *a, struct fsti_agent *b)
{
    size_t n;
    assert(a->num_partners);
    n = a->num_partners;
    for (size_t i = 0; i < n; i++)
        if (a->partners[i] == b->id) {
            a->partners[i] = a->partners[a->num_partners - 1];
            a->num_partners--;
            break;
        }
}

void fsti_agent_break_partners(struct fsti_agent *a, struct fsti_agent *b)
{
    assert(a->num_partners);
    assert(b->num_partners);
    fsti_agent_break_half_partner(a, b);
    fsti_agent_break_half_partner(b, a);
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

struct fsti_agent_elem *fsti_agent_elem_by_strname(const char *name)
{
    struct fsti_agent_elem *elem;

    elem = bsearch(name, agent_elem, fsti_agent_elem_n,
                   sizeof(struct fsti_agent_elem), cmps);
    FSTI_ASSERT(elem, FSTI_ERR_KEY_NOT_FOUND, name);

    return elem;
}

long fsti_agent_elem_val_l(struct fsti_agent_elem *elem,
                           struct fsti_agent *agent)
{
    unsigned char *addr;

    addr = (unsigned char *) agent;
    switch (elem->type) {
    case BOOL: {
        _Bool v;
        memcpy(&v, addr + elem->offset, sizeof(v));
        return (long) v;
    }
    case CHAR: {
        char v;
        memcpy(&v, addr + elem->offset, sizeof(v));
        return (long) v;
    }
    case SCHAR: {
        signed char v;
        memcpy(&v, addr + elem->offset, sizeof(v));
        return (long) v;
    }
    case UCHAR: {
        unsigned char v;
        memcpy(&v, addr + elem->offset, sizeof(v));
        return (long) v;
    }
    case SHRT: {
        short v;
        memcpy(&v, addr + elem->offset, sizeof(v));
        return (long) v;
    }
    case USHRT: {
        unsigned short v;
        memcpy(&v, addr + elem->offset, sizeof(v));
        return (long) v;
    }
    case INT: {
        int v;
        memcpy(&v, addr + elem->offset, sizeof(v));
        return (long) v;
    }
    case UINT: {
        unsigned int v;
        memcpy(&v, addr + elem->offset, sizeof(v));
        return (long) v;
    }
    case LONG: {
        long v;
        memcpy(&v, addr + elem->offset, sizeof(v));
        return (long) v;
    }
    case ULONG: {
        unsigned long v;
        memcpy(&v, addr + elem->offset, sizeof(v));
        return (long) v;
    }
    case LLONG: {
        long long v;
        memcpy(&v, addr + elem->offset, sizeof(v));
        return (long) v;
    }
    case ULLONG: {
        unsigned long long v;
        memcpy(&v, addr + elem->offset, sizeof(v));
        return (long) v;
    }
    case FLT: {
        float v;
        memcpy(&v, addr + elem->offset, sizeof(v));
        return (long) round(v);
    }
    case DBL: {
        double v;
        memcpy(&v, addr + elem->offset, sizeof(v));
        return (long) round(v);
    }
    case LDBL: {
        long double v;
        memcpy(&v, addr + elem->offset, sizeof(v));
        return (long) v;
    }
    default: FSTI_ASSERT(false, FSTI_ERR_WRONG_TYPE, NULL);
    };

    return 0;
}

long fsti_agent_elem_val_by_strname_l(const char *name, struct fsti_agent *agent)
{
    struct fsti_agent_elem *elem;

    elem = fsti_agent_elem_by_strname(name);

    return fsti_agent_elem_val_l(elem, agent);
}

void fsti_agent_arr_add_dependency(struct fsti_agent_arr *agent_arr,
                                   struct fsti_agent_ind *agent_ind)
{
    struct fsti_ind_list *node;
    node = malloc(sizeof(*node));
    FSTI_ASSERT(node, FSTI_ERR_NOMEM, NULL);
    agent_ind->agent_arr = agent_arr;
    node->next = agent_arr->ind_list;
    node->ind = agent_ind;
    agent_arr->ind_list = node;
}

void fsti_agent_arr_fill_n(struct fsti_agent_arr *agent_arr,  size_t n)
{
    agent_arr->agents = calloc(n, sizeof(struct fsti_agent));
    FSTI_ASSERT(agent_arr->agents, FSTI_ERR_NOMEM, NULL);
    for (size_t i = 0; i < n; i++)
        agent_arr->agents[i].id = i;
    agent_arr->len = agent_arr->capacity = n;
}

void fsti_agent_arr_init_n(struct fsti_agent_arr *agent_arr, size_t n)
{
    if (n) {
        fsti_agent_arr_fill_n(agent_arr, n);
    } else {
        agent_arr->agents = NULL;
        agent_arr->len = agent_arr->capacity = 0;
    }

    agent_arr->ind_list = NULL;
}

void fsti_agent_arr_init(struct fsti_agent_arr *agent_arr)
{
    fsti_agent_arr_init_n(agent_arr, 0);
}

struct fsti_agent * fsti_agent_arr_begin(struct fsti_agent_arr *agent_arr)
{
    return agent_arr->agents;
}

struct fsti_agent * fsti_agent_arr_end(struct fsti_agent_arr *agent_arr)
{
    return agent_arr->agents + agent_arr->len;
}


struct fsti_agent * fsti_agent_arr_at(struct fsti_agent_arr *agent_arr,
                                      size_t index)
{
    return agent_arr->agents + index;
}

struct fsti_agent * fsti_agent_arr_front(struct fsti_agent_arr *agent_arr)
{
    return fsti_agent_arr_begin(agent_arr);
}

struct fsti_agent * fsti_agent_arr_back(struct fsti_agent_arr *agent_arr)
{
    return agent_arr->agents + agent_arr->len - 1;
}

struct fsti_agent *fsti_agent_partner_get(struct fsti_agent_arr *agent_arr,
                                          struct fsti_agent *agent,
                                          size_t index)
{
    if (index < agent->num_partners)
        return agent_arr->agents + agent->partners[index];
    else
        return NULL;
}

struct fsti_agent *fsti_agent_partner_get0(struct fsti_agent_arr *agent_arr,
                                          struct fsti_agent *agent)
{
    return fsti_agent_partner_get(agent_arr, agent, 0);
}

static void arr_allocate(struct fsti_agent_arr *agent_arr)
{
    struct fsti_agent * agents;
    if (agent_arr->capacity == 0)
        agent_arr->capacity = 5;
    else
        agent_arr->capacity = (3 * agent_arr->capacity) / 2;

    agents = realloc(agent_arr->agents,
                     sizeof(struct fsti_agent) * agent_arr->capacity);
    FSTI_ASSERT(agents, FSTI_ERR_NOMEM, NULL);
    agent_arr->agents = agents;
}

static void ind_allocate(struct fsti_agent_ind *agent_ind)
{
    size_t * indices;
    if (agent_ind->capacity == 0)
        agent_ind->capacity = 5;
    else
        agent_ind->capacity = (3 * agent_ind->capacity) / 2;

    indices = realloc(agent_ind->indices, sizeof(size_t) *  agent_ind->capacity);
    FSTI_ASSERT(indices, FSTI_ERR_NOMEM, NULL);
    agent_ind->indices = indices;
}

void fsti_agent_arr_push(struct fsti_agent_arr *agent_arr,
                         const struct fsti_agent *agent)
{
    if (agent_arr->len == agent_arr->capacity)
        arr_allocate(agent_arr);

    if (agent)
        agent_arr->agents[agent_arr->len] = *agent;
    else
        memset(agent_arr->agents + agent_arr->len, 0, sizeof(struct fsti_agent));
    agent_arr->agents[agent_arr->len].id = agent_arr->len;

    agent_arr->len++;
}

struct fsti_agent *fsti_agent_arr_pop(struct fsti_agent_arr *agent_arr)
{
    assert(agent_arr->len);
    agent_arr->len--;
    return agent_arr->agents + agent_arr->len;
}

void fsti_agent_ind_fill_n(struct fsti_agent_ind *agent_ind,  size_t n)
{
    agent_ind->indices = calloc(n, sizeof(size_t));
    FSTI_ASSERT(agent_ind->indices, FSTI_ERR_NOMEM, NULL);
    for (size_t i = 0; i < n; i++)
        agent_ind->indices[i] = i;
    agent_ind->len = agent_ind->capacity = n;
}

void fsti_agent_ind_init_n(struct fsti_agent_ind *agent_ind,
                           struct fsti_agent_arr *agent_arr,
                           size_t n)
{
    assert(n <= agent_arr->len);
    agent_ind->agent_arr = agent_arr;
    if (n) {
        fsti_agent_ind_fill_n(agent_ind, n);
    } else {
        agent_ind->indices = NULL;
        agent_ind->len = agent_ind->capacity = 0;
    }

    fsti_agent_arr_add_dependency(agent_arr, agent_ind);
}

void fsti_agent_ind_init(struct fsti_agent_ind *agent_ind,
                         struct fsti_agent_arr *agent_arr)
{
    fsti_agent_ind_init_n(agent_ind, agent_arr, agent_arr->len);
}

size_t *fsti_agent_ind_begin(struct fsti_agent_ind *agent_ind)
{
    return agent_ind->indices;
}

size_t *fsti_agent_ind_end(struct fsti_agent_ind *agent_ind)
{
    return agent_ind->indices + agent_ind->len;
}

size_t *fsti_agent_ind_at(struct fsti_agent_ind *agent_ind, size_t index)
{
    return agent_ind->indices + index;
}

struct fsti_agent *
fsti_agent_ind_arrp(struct fsti_agent_ind *agent_ind, size_t *it)
{
    return fsti_agent_arr_at(agent_ind->agent_arr, *it);
}

struct fsti_agent *
fsti_agent_ind_arr(struct fsti_agent_ind *agent_ind, size_t index)
{
    return fsti_agent_arr_at(agent_ind->agent_arr, agent_ind->indices[index]);
}


void fsti_agent_ind_push(struct fsti_agent_ind *agent_ind, size_t index)
{
    assert(index < agent_ind->agent_arr->len);
    if (agent_ind->len == agent_ind->capacity)
        ind_allocate(agent_ind);

    agent_ind->indices[agent_ind->len] = index;
    agent_ind->len++;
}

size_t fsti_agent_ind_pop(struct fsti_agent_ind *agent_ind)
{
    assert(agent_ind->len);
    agent_ind->len--;
    return agent_ind->indices[agent_ind->len];
}

void fsti_agent_ind_remove(struct fsti_agent_ind *agent_ind,
                           size_t *it)
{
    agent_ind->len--;
    *it = agent_ind->indices[agent_ind->len];
}

void fsti_agent_ind_shuffle(struct fsti_agent_ind *agent_ind, gsl_rng *rng)
{
    if (agent_ind->len > 1) {
        for (size_t i = agent_ind->len - 1; i > 0; i--) {
            size_t j = (size_t) gsl_rng_uniform_int(rng, i + 1);
            size_t t = agent_ind->indices[j];
            agent_ind->indices[j] = agent_ind->indices[i];
            agent_ind->indices[i] = t;
        }
    }
}

void fsti_agent_arr_copy(struct fsti_agent_arr *dest,
                         struct fsti_agent_arr *src,
                         bool init_members)
{
    dest->agents = malloc(src->len * sizeof(struct fsti_agent));
    FSTI_ASSERT(dest->agents, FSTI_ERR_NOMEM, NULL);
    dest->capacity = dest->len = src->len;

    if (init_members)
        dest->ind_list = NULL;

    memcpy(dest->agents, src->agents, sizeof(struct fsti_agent) * src->len);

    FSTI_HOOK_AGENT_ARR_COPY(dest, src);
}

void fsti_agent_ind_copy(struct fsti_agent_ind *dest,
                         struct fsti_agent_ind *src)
{
    dest->indices = malloc(src->len * sizeof(size_t));
    FSTI_ASSERT(dest->indices, FSTI_ERR_NOMEM, NULL);
    dest->capacity = dest->len = src->len;
    dest->agent_arr = NULL;

    memcpy(dest->indices, src->indices, sizeof(size_t) * src->len);
}


inline size_t fsti_agent_arr_count(struct fsti_agent_arr *agent_arr)
{
    return agent_arr->len;
}

inline size_t fsti_agent_ind_count(struct fsti_agent_arr *agent_ind)
{
    return agent_ind->len;
}


void fsti_agent_ind_clear(struct fsti_agent_ind *agent_ind)
{
    agent_ind->len = 0;
}

void fsti_agent_ind_free(struct fsti_agent_ind *agent_ind)
{
    free(agent_ind->indices);
}

void fsti_agent_arr_free(struct fsti_agent_arr *agent_arr)
{
    struct fsti_ind_list *node, *prev;
    FSTI_HOOK_AGENT_ARR_FREE(agent_arr);
    free(agent_arr->agents);

    node = agent_arr->ind_list;
    while (node) {
        fsti_agent_ind_free(node->ind);
        prev = node;
        node = node->next;
        free(prev);
    }
}

void fsti_agent_test(struct test_group *tg)
{
    struct fsti_agent_arr agent_arr, copy_arr;
    struct fsti_agent_ind living, mating_pool, copy_ind, dead, shuffler;
    struct fsti_agent *agent;
    size_t *it, id;
    struct fsti_agent a, *x, *y;
    bool correct, ordered;
    gsl_rng *rng;
    long l;

    fsti_agent_arr_init_n(&agent_arr, 101);
    fsti_agent_ind_init(&living, &agent_arr);
    fsti_agent_ind_init_n(&mating_pool, &agent_arr, 0);

    TESTEQ(agent_arr.len, 101, *tg);
    TESTEQ(living.len, agent_arr.len, *tg);
    TESTEQ(mating_pool.len, 0, *tg);

    correct = true;
    id = 0;
    for (agent = agent_arr.agents; agent < agent_arr.agents + agent_arr.len;
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
    for (agent = fsti_agent_arr_begin(&agent_arr);
         agent < fsti_agent_arr_end(&agent_arr); agent++) {
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
    for (it = fsti_agent_ind_begin(&living);
         it < fsti_agent_ind_end(&living); it++) {
        if (fsti_agent_ind_arrp(&living, it)->id == id) {
            id++;
        } else {
            correct = false;
        }
    }
    TESTEQ(id, 101, *tg);
    TESTEQ(correct, true, *tg);

    memset(&a, 0, sizeof(struct fsti_agent));
    a.infected = true;
    fsti_agent_arr_push(&agent_arr, &a);

    fsti_agent_ind_push(&living, fsti_agent_arr_back(&agent_arr)->id);
    correct = true;
    id = 0;
    for (it = fsti_agent_ind_begin(&living);
         it < fsti_agent_ind_end(&living); it++) {
        if (fsti_agent_ind_arrp(&living, it)->id == id) {
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
    FSTI_FOR(living, agent, {
            if (agent->id == id) {
                id++;
            } else {
                correct = false;
            }
        });
    TESTEQ(id, 102, *tg);
    TESTEQ(correct, true, *tg);

    FSTI_FOR(living, agent, {
            if (agent->id % 2 == 0) fsti_agent_ind_push(&mating_pool, agent->id);
        });

    TESTEQ(mating_pool.len, agent_arr.len / 2, *tg);

    fsti_agent_arr_push(&agent_arr, NULL);
    correct = true;
    for (it = fsti_agent_ind_begin(&mating_pool);
         it < fsti_agent_ind_end(&mating_pool); ++it) {
        if (fsti_agent_ind_arrp(&mating_pool, it)->id % 2)
            correct = false;
    }
    TESTEQ(correct, true, *tg);

    x = fsti_agent_ind_arr(&mating_pool, 0);
    y = fsti_agent_ind_arr(&mating_pool, mating_pool.len - 1);
    fsti_agent_make_partners(x, y);
    TESTEQ(x->partners[0], y->id, *tg);
    TESTEQ(y->partners[0], x->id, *tg);

    fsti_agent_arr_copy(&copy_arr, &agent_arr, true);
    fsti_agent_ind_init(&copy_ind, &copy_arr);
    TESTEQ(copy_ind.len, copy_arr.len, *tg);
    TESTEQ(copy_ind.len, agent_arr.len, *tg);
    correct = true;
    for (it=fsti_agent_ind_begin(&copy_ind);it<fsti_agent_ind_end(&copy_ind);
         it++) {
        if(fsti_agent_ind_arrp(&copy_ind, it)->id !=
           fsti_agent_arr_at(&agent_arr, *it)->id)
            correct = false;
    }
    TESTEQ(correct, true, *tg);


    fsti_agent_ind_init_n(&dead, &agent_arr, 10);
    TESTEQ(dead.len, 10, *tg);

    rng = gsl_rng_alloc(gsl_rng_mt19937);
    fsti_agent_ind_init_n(&shuffler, &agent_arr, 0);
    TESTEQ(shuffler.len, 0, *tg);
    fsti_agent_ind_shuffle(&shuffler, rng);
    TESTEQ(shuffler.len, 0, *tg);
    fsti_agent_ind_push(&shuffler, 0);
    fsti_agent_ind_shuffle(&shuffler, rng);
    TESTEQ(shuffler.len, 1, *tg);
    fsti_agent_ind_push(&shuffler, 1);
    correct = true;
    unsigned count = 0;
    for (size_t i = 0; i < 100; ++i) {
        fsti_agent_ind_shuffle(&shuffler, rng);
        if (shuffler.indices[0] == 1 && shuffler.indices[1] == 0) count++;
    }
    TESTEQ(shuffler.len, 2, *tg);
    TESTEQ(count > 20, true, *tg);
    TESTEQ(count < 80, true, *tg);
    fsti_agent_ind_clear(&shuffler);
    size_t arr[agent_arr.len];
    for (size_t i=0; i<agent_arr.len; ++i) {
        fsti_agent_ind_push(&shuffler, i);
        arr[i] = 0;
    }
    ordered = true;
    for (size_t i=1; i<agent_arr.len; ++i) {
        if (shuffler.indices[i] != shuffler.indices[i - 1] + 1)
            ordered = false;
    }
    TESTEQ(ordered, true, *tg);
    fsti_agent_ind_shuffle(&shuffler, rng);
    for (size_t i=0; i<agent_arr.len; ++i) arr[shuffler.indices[i]]++;
    for (size_t i=1; i<agent_arr.len; ++i) {
        if (shuffler.indices[i] != shuffler.indices[i - 1] + 1) {
            ordered = false;
            break;
        }
    }
    TESTEQ(ordered, false, *tg);
    correct = true;
    for (size_t i=0; i<agent_arr.len; ++i) {
        if (arr[i] != 1)
            correct = false;
    }
    TESTEQ(correct, true, *tg);

    gsl_rng_free(rng);
    fsti_agent_arr_free(&copy_arr);
    fsti_agent_arr_free(&agent_arr);

    a.id = 10;
    a.age = 23;
    a.cured.date = 2018.9;
    a.date_death.date = 2023.1;

    l = fsti_agent_elem_val_by_strname_l("id", &a);
    TESTEQ(l, 10, *tg);
    l = fsti_agent_elem_val_by_strname_l("age", &a);
    TESTEQ(l, 23, *tg);
    l = fsti_agent_elem_val_by_strname_l("cured", &a);
    TESTEQ(l, 2018, *tg);
    l = fsti_agent_elem_val_by_strname_l("date_death", &a);
    TESTEQ(l, 2023, *tg);
}
