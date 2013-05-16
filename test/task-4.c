#include <stdio.h>
#include <ompt.h>

void dobreak()
{
}

void dump_frames(int rank, int level)
{
	int i = 0;
	while(1) {
	if (rank == 22) dobreak();
	ompt_frame_t *frame = ompt_get_task_frame(i);
	void *task = ompt_get_task_function(i);
	printf("%d(%d, %d): task = %p frame = %p exit = %p reenter = %p\n", 
		rank, level, i, task, frame, 
		frame ? frame->exit_runtime_frame : 0, 
		frame ? frame->reenter_runtime_frame : 0); 
	i++;
        if (frame == 0) break;
	}
}

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
