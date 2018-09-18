/**
    @file

    @brief Macros and functions for simple unit testing.

    Declare a test_group struct and initialize it with test_group_new().  Use
    the macros TEST, TESTEQ, TESTGT and TESTLT to run tests.  The function
    test_group_summary() prints the number of successes and failures in your
    test group.  To reduce the amount of output to stdout set the test_group's
    verbose value to false.
*/


#ifndef USEFUL_TEST_H
#define USEFUL_TEST_H

#include <stdio.h>
#include <stdbool.h>

/**
   Convenient wrapper for debug output.
*/

#define DBG(...)							\
	printf("DEBUG %s %d: ", __FILE__, __LINE__);			\
	printf(__VA_ARGS__);						\
	printf("\n")

/**
   Holds information about a test group, including number of successes,
   failures, and whether to use verbose output in tests.
 */

struct test_group {
	unsigned successes;
	unsigned failures;
	bool verbose;
};

#define EQ(X, Y) ( (X) == (Y) )

#define GT(X, Y) ( (X) > (Y) )

#define LT(X, Y) ( (X) < (Y) )

#define GE(X, Y) ( (X) >= (Y) )

#define LE(X, Y) ( (X) <= (Y) )

/**
   Macro that compares two operands for a test in a test group.

   \param X the first operand of the test
   \param Y the second operand fo the test
   \param comparator the operand use to compare X and Y, e.g. EQ, GT, LT
   \expected the expected result of the comparison, typically true or false
   \test_group test group to tally the success or failure in
 */
#define TEST(X, Y, comparator, expected, test_group)			\
	do {								\
		if ( comparator( (X), (Y) ) == (expected) )		\
			(test_group).successes++;			\
		else {							\
			(test_group).failures++;			\
			if ( (test_group).verbose)			\
				printf("Failed test in file %s "	\
					"at line %d: "			\
					"%s %s %s %s\n",		\
					__FILE__, __LINE__, #X, #Y,	\
					#comparator, #expected);	\
		}							\
	} while(0)


/**
   Macro that checks if two operands are equal for a test in a test group.

   \param X the first operand of the test
   \param Y the second operand fo the test
   \test_group test group to tally the success or failure in
 */
#define TESTEQ(X, Y, test_group) TEST( (X), (Y), EQ, true, test_group)

/**
   Macro that checks if the first operand is greater than the second
   operand for a test in a test group.

   \param X the first operand of the test
   \param Y the second operand fo the test
   \test_group test group to tally the success or failure in
 */
#define TESTGT(X, Y, test_group) TEST( (X), (Y), GT, true, test_group)

/**
   Macro that checks if the first operand is less than the second
   operand for a test in a test group.

   \param X the first operand of the test
   \param Y the second operand fo the test
   \test_group test group to tally the success or failure in
 */
#define TESTLT(X, Y, test_group) TEST( (X), (Y), LT, true, test_group)

/**
   Macro that checks if the first operand is greater than or equal to the second
   operand for a test in a test group.

   \param X the first operand of the test
   \param Y the second operand fo the test
   \test_group test group to tally the success or failure in
 */
#define TESTGE(X, Y, test_group) TEST( (X), (Y), GE, true, test_group)

/**
   Macro that checks if the first operand is less than or equal to the second
   operand for a test in a test group.

   \param X the first operand of the test
   \param Y the second operand fo the test
   \test_group test group to tally the success or failure in
 */
#define TESTLE(X, Y, test_group) TEST( (X), (Y), LE, true, test_group)


void test_group_new(struct test_group* test_group);
void test_group_summary(const struct test_group* test_group);

#endif
