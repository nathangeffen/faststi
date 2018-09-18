/**
    @file

    @brief Provides some useful generic algorithms using void pointers.

*/

#ifndef USEFUL_ALGORITHMS_H
#define USEFUL_ALGORITHMS_H

#ifndef RANDOM
#if _SVID_SOURCE || _BSD_SOURCE || _XOPEN_SOURCE >= 500 \
	|| _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED
#define RANDOM random() % to __to__
#else
#define RANDOM rand() % __to__
#endif
#endif


#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/// Calculate offset for void pointer array
#define VOS(base, i, elem_size) (char *) (base) + ( (i) * (elem_size))

char * strdup(const char * src);

uint32_t rand_to(uint32_t to);

void swap(void *a, void *b, size_t size);

void shuffle(void *data, size_t nmemb, size_t size);

size_t least_dist(const void *elem, const void *arr,
		size_t nmemb, size_t elem_size,
		double (*distance)(const void *, const void *));

void knn_match(void * agents, size_t nmemb, unsigned k,
	bool (*has_partner)(const void *), void (*set_partners)(void *, void *),
	double (*distance)(const void *, const void *));

void cspm(void* agents, size_t nmemb, size_t neighbors, unsigned clusters,
	int (*cmp_cluster)(const void *, const void *),
	bool (*has_partner)(const void *),
	void (*set_partners)(void *, void *),
	double (*distance)(const void *, const void *));

#endif
