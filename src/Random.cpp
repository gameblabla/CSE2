#include "Random.h"

// A replication of MSVC's rand algorithm
static unsigned long next = 1;

int msvc_rand(void)
{
	next = ((next) * 214013 + 2531011);
	return ((next) >> 16) & 0x7FFF;
}

void msvc_srand(unsigned int seed)
{
	next = seed;
}
