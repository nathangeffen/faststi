/**
   \file

   \brief Definitions of array management functions

*/
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "../fsti-error.h"
#include "array.h"

/**
   Grows an array if its capacity is full.
 */
size_t array_grow(void **array, size_t current_capacity, size_t object_size)
{
	size_t new_capacity, new_size;

	new_capacity = current_capacity == 0
		? INIT_CAPACITY : current_capacity * GROWTH;
	new_size = new_capacity * object_size;
	*array = realloc(*array, new_size);
        FSTI_ASSERT(array, FSTI_ERR_NOMEM, NULL);
	return new_capacity;
}
