/**
   @file
   @brief Functions for a simple unit testing utility.
*/

#include <stdio.h>

#include "test.h"

/**
   Initiates a test group, setting the succeses and failures to 0, and
   verbose to true.
 */

void test_group_new(struct test_group* test_group)
{
	test_group->successes = test_group->failures = 0;
	test_group->verbose = true;
}

/**
   Summarises the results of a test group.
 */

void test_group_summary(const struct test_group* test_group)
{
	unsigned tests = test_group->successes + test_group->failures;
	printf("Tests: %u, Successes: %u. Failures %u\n",
		tests, test_group->successes, test_group->failures);
}
