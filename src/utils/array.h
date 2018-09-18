/**
   @file

   @brief Macros and functions for dynamic arrays (think very simple C++
   STL vector).

   To use it declare a struct which contains a pointer array, and two integer
   fields (usually size_t), size (the number of elements in the array) and
   capacity (the number of elements the array can currently store). E.g.

   @verbatim

   struct MyArray {
      int *my_int_array;
      size_t len;
      size_t capacity;
      // You can add additional elements to the struct here if necessary.
   };

   @endverbatim

   Use ARRAY_NEW to initialize MyArray variables, ARRAY_APPEND to add elements
   to it and ARRAY_FREE to return the array to the heap. ARRAY_APPEND increases
   the capacity of the array if it is full by calling realloc.

   Note: The programmer is responsible for allocating and free elements of the
   array if these contain dynamically allocated memory. However,

   Here is an example:

   @include arrays.c

 */

#ifndef USEFUL_ARRAY_H
#define USEFUL_ARRAY_H

#include <stdlib.h>
#include <stddef.h>

/**
   Initial capacity of the array.
 */
#define INIT_CAPACITY 10

/**
   Factor to grow the array by when it it is full..
 */
#define GROWTH 3 / 2

size_t array_grow(void **array, size_t current_capacity, size_t object_size);


/**
   Creates a new array.

   @param array Name of the struct variable containing the array
   @param element Name of the struct pointer element that represents the array
*/
#define ARRAY_NEW(array, element) do {					\
		(array).len = 0;					\
		(array).element = NULL;					\
		(array).capacity = array_grow( (void *) &(array).element, \
					0, sizeof(*(array).element));	\
	} while(0)

/**
   Appends an element to the back of an array.

   @param array Struct variable containing the array
   @param element Struct pointer element that represents the array
   @param object Variable to append to the array
*/
#define ARRAY_PUSH(array, element, object)				\
	do {								\
		if ((array).len == (array).capacity)			\
			(array).capacity = array_grow( (void *)		\
						&(array).element,	\
						(array).capacity,	\
						sizeof(*(array).element)); \
		(array).element[(array).len] = object;			\
		++(array).len;						\
	} while(0)


/**
   Removes an element from the back of an array.

   @param array Struct variable containing the array
   @param element Struct pointer element that represents the array
   @param object Variable to place the last element of the array into
*/
#define ARRAY_POP(array, element, object)				\
	do {								\
		assert((array).len);					\
		--(array).len;						\
		object = (array).element[(array).len];			\
	} while(0)

/**
   Finds an element in an array.

   @param array Struct variable containing the array
   @param element Struct pointer element that represents the array
   @param key value to find
   @return index in array of the element or array.len if not found
*/
#define ARRAY_FIND(array, element, key, index)				\
	do {								\
		index = (array).len;					\
		for (size_t i = 0; i < (array).len; ++i)		\
			if ((array).element[i] == (key))		\
				index = i;				\
	} while(0)


/**
   Frees an array. Note it doesn't free the elements of the array.
   Either do that manually or use ARRAY_FREE_ELEM

   @param array Struct variable containing the array
   @param element Struct pointer element that represents the array
 */
#define ARRAY_FREE(array, element) do {		\
		free((array).element);		\
	} while(0)


/**
   Converts two-parameter freeing function to single parameter one that
   doesn't require data parameter.
 */
#define ONE_PARM_CUSTOM_FREE_FUNC(func, x, _) func( (x) )

/**
   Converts two parameter function to standard free.
 */
#define ONE_PARM_FREE_FUNC(x, y) ONE_PARM_CUSTOM_FREE_FUNC(free, x, y)


/**
   Frees an array and its elements, using a custom user-provided free function.

   @param array Struct variable containing the array
   @param element Struct pointer element that represents the array
   @param free_func User provided freeing function
   @param data User provided data passed as second parameter to freeing function
 */
#define ARRAY_FREE_ELEMS_CUSTOM(array, element, free_func, data) do {	\
		for (size_t i = 0; i < (array).len; ++i)		\
			free_func( (array).element[i], data);		\
		ARRAY_FREE( (array), element );				\
	} while(0)


/**
   Frees an array and its elements.

   @param array Struct variable containing the array
   @param element Struct pointer element that represents the array
 */
#define ARRAY_FREE_ELEMS(array, element) do {				\
		ARRAY_FREE_ELEMS_CUSTOM((array), element,		\
				ONE_PARM_FREE_FUNC, NULL);		\
	} while(0)



#endif
