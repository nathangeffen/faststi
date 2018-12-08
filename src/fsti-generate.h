#ifndef FSTI_GENERATE_H
#define FSTI_GENERATE_H

#include "fsti-defs.h"
#include "fsti-agent.h"
#include "fsti-defaults.h"

struct fsti_generator_parms {
    size_t offset;
    enum fsti_type type;
    double parameters[FSTI_GEN_PARMS];
};

typedef void (*fsti_generate_func)(struct fsti_simulation *simulation,
                                   struct fsti_agent *agent,
                                   const struct fsti_generator_parms *parms);


struct fsti_generator {
    struct fsti_generator_parms parms;
    fsti_generate_func gen_func;
};


struct fsti_generator_pair {
    char name[FSTI_KEY_LEN];
    fsti_generate_func func;
};

extern const struct fsti_generator_pair fsti_generator_map[];
extern const size_t fsti_generator_map_n;

void fsti_gen_const(struct fsti_simulation *simulation,
                    struct fsti_agent *agent,
                    const struct fsti_generator_parms *parms);


void fsti_gen_uniform(struct fsti_simulation *simulation,
                      struct fsti_agent *agent,
                      const struct fsti_generator_parms *parms);

void fsti_gen_age(struct fsti_simulation *simulation,
                  struct fsti_agent *agent,
                  const struct fsti_generator_parms *parms);

void fsti_gen_beta(struct fsti_simulation *simulation,
                   struct fsti_agent *agent,
                   const struct fsti_generator_parms *parms);

void fsti_gen_sex_sexor(struct fsti_simulation *simulation,
                        struct fsti_agent *agent,
                        const struct fsti_generator_parms *parms);

void fsti_gen_sex_sexor_infection(struct fsti_simulation *simulation,
                                  struct fsti_agent *agent,
                                  const struct fsti_generator_parms *parms);

#endif
