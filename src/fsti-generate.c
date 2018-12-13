#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "fsti-generate.h"
#include "fsti-simulation.h"

const struct fsti_generator_pair fsti_generator_map[] = {
    FSTI_GENERATOR_MAP
};

const size_t fsti_generator_map_n =
    sizeof(fsti_generator_map) / sizeof(struct fsti_generator_pair);


void fsti_gen_const(struct fsti_simulation *simulation,
                    struct fsti_agent *agent,
                    const struct fsti_generator_parms *parms)
{
    fsti_cnv_vals((char *) agent+parms->offset, parms->parameters,
                  parms->type, DBL);
}

void fsti_gen_age(struct fsti_simulation *simulation,
                  struct fsti_agent *agent,
                  const struct fsti_generator_parms *parms)
{
    double val, a, b;
    int32_t min, max;

    a = parms->parameters[0];
    b = parms->parameters[1];
    min = parms->parameters[2] * FSTI_YEAR;
    max = parms->parameters[3] * FSTI_YEAR;
    val = gsl_ran_beta(simulation->rng, a, b) * (max - min) + min;
    fsti_cnv_vals((char *) agent+parms->offset, &val, parms->type, DBL);
}


void fsti_gen_beta(struct fsti_simulation *simulation,
                   struct fsti_agent *agent,
                   const struct fsti_generator_parms *parms)
{
    double val, a, b, min, max;

    a = parms->parameters[0];
    b = parms->parameters[1];
    min = parms->parameters[2];
    max = parms->parameters[3];
    val = gsl_ran_beta(simulation->rng, a, b) * (max - min) + min;
    fsti_cnv_vals((char *) agent+parms->offset, &val, parms->type, DBL);
}

void fsti_gen_uniform(struct fsti_simulation *simulation,
                      struct fsti_agent *agent,
                      const struct fsti_generator_parms *parms)
{
    long min = parms->parameters[0];
    long max = parms->parameters[1];
    long val = gsl_rng_uniform_int(simulation->rng, max - min + 1) + min;

    fsti_cnv_vals((char *)agent+parms->offset, &val, parms->type, LONG);
}

void fsti_gen_sex_sexor(struct fsti_simulation *simulation,
                        struct fsti_agent *agent,
                        const struct fsti_generator_parms *parms)
{
   if (gsl_rng_uniform(simulation->rng) < parms->parameters[0])
        agent->sex = FSTI_MALE;
    else
        agent->sex = FSTI_FEMALE;

    if (gsl_rng_uniform(simulation->rng) < parms->parameters[1])
        agent->sex_preferred = agent->sex;
    else
        agent->sex_preferred = !agent->sex;
}

void fsti_gen_sex_sexor_infection(struct fsti_simulation *simulation,
                                  struct fsti_agent *agent,
                                  const struct fsti_generator_parms *parms)
{
    fsti_gen_sex_sexor(simulation, agent, parms);

    if (agent->sex == FSTI_MALE && agent->sex_preferred == FSTI_FEMALE) {
        if (gsl_rng_uniform(simulation->rng) < parms->parameters[2])
            agent->infected = true;
        else
            agent->infected = false;
    }

    if (agent->sex == FSTI_FEMALE && agent->sex_preferred == FSTI_MALE) {
        if (gsl_rng_uniform(simulation->rng) < parms->parameters[3])
            agent->infected = true;
        else
            agent->infected = false;
    }

    if (agent->sex == FSTI_MALE && agent->sex_preferred == FSTI_MALE) {
        if (gsl_rng_uniform(simulation->rng) < parms->parameters[4])
            agent->infected = true;
        else
            agent->infected = false;
    }

    if (agent->sex == FSTI_FEMALE && agent->sex_preferred == FSTI_FEMALE) {
        if (gsl_rng_uniform(simulation->rng) < parms->parameters[5])
            agent->infected = true;
        else
            agent->infected = false;
    }
}
