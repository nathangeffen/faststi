#include <stdlib.h>
#include <stdio.h>

#include "utils/test.h"

#include "fsti-tests.h"
#include "fsti-dataset.h"
#include "fsti-config.h"
#include "fsti-agent.h"
#include "fsti-simset.h"

int fsti_tests()
{
    struct test_group tg;

    test_group_new(&tg);

    fsti_test_defs(&tg);
    fsti_dataset_test(&tg);
    fsti_config_test(&tg);
    fsti_agent_test(&tg);
    fsti_simulation_test(&tg);
    fsti_simset_test(&tg);

    test_group_summary(&tg);

    return tg.failures;
}
