/* 
 * This header file implements a dummy tool which will execute all
 * of the implemented callbacks in the OMPT framework. When a supported
 * callback function is executed, it will print a message with some
 * relevant information.
 */

#include <stdlib.h>
#include <ompt.h>

/* Entering a parallel region */
void my_parallel_region_create (
  ompt_data_t  *parent_task_data,   /* tool data for parent task   */
  ompt_frame_t *parent_task_frame,  /* frame data of parent task   */
  ompt_parallel_id_t parallel_id)   /* id of parallel region       */
{
  printf("OpenMP Parallel Region Create: %llx\n", parallel_id); fflush(stdout);
}

/* Exiting a parallel region */
void my_parallel_region_exit (
  ompt_data_t  *parent_task_data,   /* tool data for parent task   */
  ompt_frame_t *parent_task_frame,  /* frame data of parent task   */
  ompt_parallel_id_t parallel_id)   /* id of parallel region       */
{
  printf("OpenMP Parallel Region Exit: %llx\n", parallel_id); fflush(stdout);
}

/* Task creation */
void my_task_create (
  ompt_data_t *task_data)            /* tool data for task          */
{
  printf("OpenMP Task Create: %llx\n", task_data); fflush(stdout);
}

/* Task exit */
void my_task_exit (
  ompt_data_t *task_data)            /* tool data for task          */
{
  printf("OpenMP Task Exit: %llx\n", task_data); fflush(stdout);
}

/* Thread creation */
void my_thread_create(
  ompt_data_t *thread_data)           /* tool data for thread       */
{
  printf("OpenMP Thread Create: %llx\n", thread_data); fflush(stdout);
}

/* Thread exit */
void my_thread_exit(
  ompt_data_t *thread_data)           /* tool data for thread       */
{
  printf("OpenMP Thread Create: %llx\n", thread_data); fflush(stdout);
}

/* Some control event happened */
void my_control(
  uint64_t command,                /* command of control call      */
  uint64_t modifier)                /* modifier of control call     */
{
  printf("OpenMP Control: %llx, %llx\n", command, modifier); fflush(stdout);
}

/* Shutting down the OpenMP runtime */
void my_shutdown()
{
  printf("OpenMP Shutdown.\n"); fflush(stdout);
}

/* Wait for atomic lock */
void my_atomic_wait (
  ompt_wait_id_t *waitid)            /* address of atomic variable          */
{
  printf("OpenMP Atomic Wait: %llx\n", waitid); fflush(stdout);
}

/* Acquired atomic lock */
void my_atomic_acquired (
  ompt_wait_id_t *waitid)            /* address of atomic variable          */
{
  printf("OpenMP Atomic Acquired: %llx\n", waitid); fflush(stdout);
}

/* Released atomic lock */
void my_atomic_released (
  ompt_wait_id_t *waitid)            /* address of atomic variable          */
{
  printf("OpenMP Atomic Released: %llx\n", waitid); fflush(stdout);
}

#define CHECK(RC) \
  if (RC != 0) { fprintf(stderr, "Error registering callback.\n"); return 0; }

int ompt_initialize() {
  int rc = 0;
  CHECK(ompt_set_callback(ompt_event_parallel_create, my_parallel_region_create));
  CHECK(ompt_set_callback(ompt_event_parallel_exit, my_parallel_region_exit));
  CHECK(ompt_set_callback(ompt_event_task_create, my_task_create));
  CHECK(ompt_set_callback(ompt_event_task_exit, my_task_exit));
  CHECK(ompt_set_callback(ompt_event_thread_create, my_thread_create));
  CHECK(ompt_set_callback(ompt_event_thread_exit, my_thread_exit));
  CHECK(ompt_set_callback(ompt_event_control, my_control));
  CHECK(ompt_set_callback(ompt_event_runtime_shutdown, my_shutdown));
  CHECK(ompt_set_callback(ompt_event_wait_atomic, my_atomic_wait));
  CHECK(ompt_set_callback(ompt_event_acquired_atomic, my_atomic_acquired));
  CHECK(ompt_set_callback(ompt_event_release_atomic, my_atomic_released));
  return 1;
}


