#include <stdio.h>
#include "omp.h"
#include <ompt.h>

#include "dump.h"
#include "callback.h"

main()
{
#pragma omp parallel
{
	int rank = omp_get_thread_num();
	printf("hello world from %d (level 1)\n", rank);
	dump_frames(rank, 1);
}
}
