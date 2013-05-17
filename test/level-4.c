#include <stdio.h>
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
#pragma omp parallel 
{
	printf("hello world from %d (level 2)\n", rank);
	dump_frames(rank, 2);
#pragma omp parallel 
{
	printf("hello world from %d (level 3)\n", rank);
	dump_frames(rank, 3);
#pragma omp parallel 
{
	printf("hello world from %d (level 4)\n", rank);
	dump_frames(rank, 4);
}
}
}
}
}
