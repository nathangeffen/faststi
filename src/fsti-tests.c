#include <stdlib.h>
#include <stdio.h>

#include "utils/test.h"

#include "fsti-tests.h"
#include "fsti-config.h"

int fsti_tests()
{
    struct test_group tg;

    test_group_new(&tg);

    fsti_config_test(&tg);

    test_group_summary(&tg);

    return tg.failures;
}
