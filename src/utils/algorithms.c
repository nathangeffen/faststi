/* Generic C functions */

#include <error.h>
#include <errno.h>
#include "algorithms.h"

/**
   Makes a duplicate of a string. It is the caller's responsibility to
   free the copy.

   \param src Null terminated string.
   \return Copy of the parameter string
 */


char * strdup(const char * src)
{
	char *dest = malloc(strlen(src) + 1), *p = dest;
        if (dest == NULL)
            error(EXIT_FAILURE, errno,
                  "Failed to allocate space for string.");

	while (*src)
		*p++ = *src++;
	*p = 0;
	return dest;
}

/**
   Generates a random number in the semi-open range 0 .. to - 1.
   \param to Upper bound of semi-open range from which to draw random number
   \param rng NULL for now
 */

uint32_t rand_to(uint32_t to)
{
	uint32_t num_bins = to;
	uint32_t num_rand = ~( to & 0);
	uint32_t bin_size = num_rand / num_bins;
	uint32_t defect = num_rand % num_bins;

	uint32_t x;
	do {
#if _SVID_SOURCE || _BSD_SOURCE || _XOPEN_SOURCE >= 500 \
	|| _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED
		x = random();
#else
		x = rand();
#endif
	} while (num_rand - defect <= (unsigned long) x);

	return x / bin_size;
}


/**
   Swaps the values of *a* and *b*.

   \param size The number of bytes contained in *a* and *b*.
 */

void swap(void *a, void *b, size_t size)
{
	char t[size];
	memmove(t, a, size);
	memmove(a, b, size);
	memmove(b, t, size);
}

/**
   Shuffles an array.

   \param data Array to shuffle
   \param nmemb Number of elements in the array
   \param size Size of an element in chars (or bytes)
   \param gen Random number generator (can be NULL)
   \param rand_to Function that returns random int than its first parameter.
   Its second parameter is gen.
 */
void shuffle(void *data, size_t nmemb, size_t size)
{

	for (size_t i = nmemb; i-- > 1; ) {
		size_t __to__ = i + 1;
		size_t j =  RANDOM;
		swap( VOS(data, i, size), VOS(data, j, size), size );
	}
}

/**
   Finds closest element in an array.

   \param elem Variable to compare against
   \param nmemb Array to search for closest value to *elem*
   \param nmemb Number of elements in the array
   \paran elem_size Size of an element in the array
   \param distance Function that returns distance between two elements

   \return index of element in *arr* that is closest to *elem*.
 */
size_t least_dist(const void *elem, const void *arr,
		size_t nmemb, size_t elem_size,
		double (*distance)(const void *, const void *))
{
	double least_distance = DBL_MAX;
	size_t least_index = nmemb;
	for (size_t i = 0; i < nmemb; ++i) {
		double d = distance(elem, VOS(arr, i, elem_size));
		if ( d < least_distance) {
			least_distance = d;
			least_index = i;
		}
	}
	return least_index;
}

/**
   For each agent, finds the nearest unmatched of k agents to the right of it in
   an array of agents.

   \param agents Array of pointers to agents
   \param nmemb Number of entries in *agents*
   \param k Number of adjacent entries to the right of the agent under
   consideration to search
   \param has_partner Function that determines if agent has partner
   \param set_partners Function that places two agents into a partnership
   \param distance Measures distance between two agents
*/
void knn_match(void * agents, size_t nmemb, unsigned k,
	bool (*has_partner)(const void *), void (*set_partners)(void *, void *),
	double (*distance)(const void *, const void *))
{
	const size_t elem_size = sizeof(agents);
	for (size_t i = 0; i < nmemb - 1; ++i) {
		if (has_partner( VOS(agents, i, elem_size))) continue;
		size_t n, start = i + 1;
		n = (start + k) < nmemb ? k : nmemb - start;
		size_t best = least_dist(VOS(agents, i, elem_size),
					VOS(agents, start, elem_size), n,
					elem_size, distance);
		if (best < n)
			set_partners(VOS(agents, i, elem_size),
				VOS(agents, start + best, elem_size));
	}
}

/**
   Matches a set of agents into partnerships using the Cluster Shuffle
   Pair-Matching approximation algorithm.

   \param agents Array of pointers to agents in mating pool
   \param nmemb Number of agents in the partner market
   \param k Number of neighbors to search for appropriate partner
   \param clusters Number of clusters to divide the agents into
   \param cmp_cluster Function that compares the cluster values of two agents
   \param has_partner Function that determines if agent is in partnership
   \param set_partners Function to place two agents into partnership
   \param distance Function to measure distance between two agents
   \param gen Random number generator (can be NULL)
   \param rand_to Function that returns random int than its first parameter.
   Its second parameter is gen.
 */
void cspm(
	void * agents,
	size_t nmemb, size_t k, unsigned clusters,
	int (*cmp_cluster)(const void *, const void *),
	bool (*has_partner)(const void *), // Does agent have partner
	void (*set_partners)(void *, void *), // Set agents to be partners
	double (*distance)(const void *, const void *)) // Distance between two agents

{
	const size_t elem_size = sizeof(agents);
	size_t cluster_size = nmemb / clusters;

	qsort(agents, nmemb, elem_size, cmp_cluster);

	// Shuffle each of the clusters
	for (unsigned i = 0; i < clusters; ++i) {
		size_t first = i * cluster_size;
		size_t last = first + cluster_size;
		if (last > nmemb) last = nmemb;
		shuffle(VOS(agents, first, elem_size), last - first, elem_size);
	}

	knn_match(agents, nmemb, k, has_partner, set_partners, distance);
}
