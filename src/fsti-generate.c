#include "fsti-generate.h"

const struct fsti_generator_pair fsti_generator_map[] = {
    FSTI_GENERATOR_MAP
};

const size_t fsti_generator_map_n =
    sizeof(fsti_generator_map) / sizeof(struct fsti_generator_pair);


void fsti_gen_const(struct fsti_simulation *simulation,
                    struct fsti_agent *agent,
                    const struct fsti_generator_parms *parms)
{
    fsti_cnv_vals(agent+parms->offset, parms->parameters, parms->type, DBL);
}

void fsti_gen_normal(struct fsti_simulation *simulation,
                     struct fsti_agent *agent,
                     const struct fsti_generator_parms *parms)
{

}

void fsti_gen_sex(struct fsti_simulation *simulation,
                  struct fsti_agent *agent,
                  const struct fsti_generator_parms *parms)
{

}

void fsti_gen_uniform(struct fsti_simulation *simulation,
                      struct fsti_agent *agent,
                      const struct fsti_generator_parms *parms)
{

}
