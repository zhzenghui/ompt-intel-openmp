/* 
 * This header file implements a dummy tool which will execute all
 * of the implemented callbacks in the OMPT framework. When a supported
 * callback function is executed, it will print a message with some
 * relevant information.
 */

#include <stdlib.h>
#include <ompt.h>

/*
 * Macros to help generate test functions for each event
 */

#define TEST_THREAD_CALLBACK(EVENT) \
void my_##EVENT ( \
ompt_data_t *thread_data)  /* tool data for thread       */ \
{ \
  printf("%d: %s: %llx\n", omp_get_thread_num(), #EVENT, thread_data); \
  fflush(stdout);\
}

#define TEST_PARALLEL_CALLBACK(EVENT) \
void my_##EVENT ( \
ompt_data_t  *parent_task_data,   /* tool data for parent task   */ \
ompt_parallel_id_t parallel_id)   /* id of parallel region       */ \
{ \
  printf("%d: %s: %llx\n", omp_get_thread_num(), #EVENT, parallel_id); \
  fflush(stdout); \
}

#define TEST_NEW_PARALLEL_CALLBACK(EVENT) \
void my_##EVENT ( \
  ompt_data_t  *parent_task_data,   /* tool data for parent task   */ \
  ompt_frame_t *parent_task_frame,  /* frame data of parent task   */ \
  ompt_parallel_id_t parallel_id)   /* id of parallel region       */ \
{ \
  printf("%d: %s: %llx\n", omp_get_thread_num(), #EVENT, parallel_id); \
  fflush(stdout); \
}

#define TEST_TASK_CALLBACK(EVENT) \
void my_##EVENT ( \
ompt_data_t *task_data)            /* tool data for task          */ \
{ \
  printf("%d: %s: %llx\n", omp_get_thread_num(), #EVENT, task_data); \
  fflush(stdout); \
} \

#define TEST_CONTROL_CALLBACK(EVENT) \
void my_##EVENT( \
uint64_t command,                /* command of control call      */ \
uint64_t modifier)                /* modifier of control call     */ \
{ \
  printf("%d: %s: %llx, %llx\n", omp_get_thread_num(), #EVENT, command, modifier); \
  fflush(stdout); \
}

#define TEST_CALLBACK(EVENT) \
void my_##EVENT() \
{ \
  printf("%d: %s.\n", omp_get_thread_num(), #EVENT); \
  fflush(stdout); \
}

#define TEST_WAIT_CALLBACK(EVENT) \
void my_##EVENT ( \
  ompt_wait_id_t *waitid)            /* address of atomic variable          */ \
{ \
  printf("%d: %s: %llx\n", omp_get_thread_num(), #EVENT, waitid); \
  fflush(stdout); \
}

#define TEST_TASK_SWITCH_CALLBACK(EVENT) \
void my_##EVENT ( \
  ompt_data_t *suspended_task_data, /* tool data for suspended task */ \
  ompt_data_t *resumed_task_data)   /* tool data for resumed task   */ \
{ \
  printf("%d: %s: \n", omp_get_thread_num(), #EVENT); \
  fflush(stdout); \
}

/*******************************************************************
 * required events 
 *******************************************************************/

TEST_NEW_PARALLEL_CALLBACK(ompt_event_parallel_create)
TEST_NEW_PARALLEL_CALLBACK(ompt_event_parallel_exit)
TEST_TASK_CALLBACK(ompt_event_task_create)
TEST_TASK_CALLBACK(ompt_event_task_exit)
TEST_THREAD_CALLBACK(ompt_event_thread_create)
TEST_THREAD_CALLBACK(ompt_event_thread_exit)
TEST_CONTROL_CALLBACK(ompt_event_control)
TEST_CALLBACK(ompt_event_runtime_shutdown)

/*******************************************************************
 * optional events
 *******************************************************************/

/* Blameshifting events */
TEST_PARALLEL_CALLBACK(ompt_event_idle_begin)
TEST_PARALLEL_CALLBACK(ompt_event_idle_end)
TEST_PARALLEL_CALLBACK(ompt_event_wait_barrier_begin);
TEST_PARALLEL_CALLBACK(ompt_event_wait_barrier_end);
TEST_PARALLEL_CALLBACK(ompt_event_wait_taskwait_begin);
TEST_PARALLEL_CALLBACK(ompt_event_wait_taskwait_end);
TEST_PARALLEL_CALLBACK(ompt_event_wait_taskgroup_begin);
TEST_PARALLEL_CALLBACK(ompt_event_wait_taskgroup_end);
TEST_WAIT_CALLBACK(ompt_event_release_lock);
TEST_WAIT_CALLBACK(ompt_event_release_nest_lock_last);
TEST_WAIT_CALLBACK(ompt_event_release_critical);
TEST_WAIT_CALLBACK(ompt_event_release_ordered)
TEST_WAIT_CALLBACK(ompt_event_release_atomic)

/* synchronous events */
TEST_PARALLEL_CALLBACK(ompt_event_implicit_task_create);
TEST_PARALLEL_CALLBACK(ompt_event_implicit_task_exit);
TEST_PARALLEL_CALLBACK(ompt_event_barrier_begin)
TEST_PARALLEL_CALLBACK(ompt_event_barrier_end)
TEST_PARALLEL_CALLBACK(ompt_event_master_begin)
TEST_PARALLEL_CALLBACK(ompt_event_master_end)
TEST_TASK_SWITCH_CALLBACK(ompt_event_task_switch);
TEST_PARALLEL_CALLBACK(ompt_event_loop_begin);
TEST_PARALLEL_CALLBACK(ompt_event_loop_end);
TEST_PARALLEL_CALLBACK(ompt_event_section_begin);
TEST_PARALLEL_CALLBACK(ompt_event_section_end);
TEST_PARALLEL_CALLBACK(ompt_event_single_in_block_begin);
TEST_PARALLEL_CALLBACK(ompt_event_single_in_block_end);
TEST_PARALLEL_CALLBACK(ompt_event_single_others_begin);
TEST_PARALLEL_CALLBACK(ompt_event_single_others_end);
TEST_PARALLEL_CALLBACK(ompt_event_taskwait_begin);
TEST_PARALLEL_CALLBACK(ompt_event_taskwait_end);
TEST_PARALLEL_CALLBACK(ompt_event_taskgroup_begin);
TEST_PARALLEL_CALLBACK(ompt_event_taskgroup_end);
TEST_PARALLEL_CALLBACK(ompt_event_release_nest_lock_prev);
TEST_WAIT_CALLBACK(ompt_event_wait_lock);
TEST_WAIT_CALLBACK(ompt_event_wait_nest_lock);
TEST_WAIT_CALLBACK(ompt_event_wait_critical);
TEST_WAIT_CALLBACK(ompt_event_wait_atomic)
TEST_WAIT_CALLBACK(ompt_event_wait_ordered)
TEST_WAIT_CALLBACK(ompt_event_acquired_lock);
TEST_WAIT_CALLBACK(ompt_event_acquired_nest_lock_first);
TEST_PARALLEL_CALLBACK(ompt_event_acquired_nest_lock_next);
TEST_WAIT_CALLBACK(ompt_event_acquired_critical);
TEST_WAIT_CALLBACK(ompt_event_acquired_atomic);
TEST_WAIT_CALLBACK(ompt_event_acquired_ordered)
TEST_WAIT_CALLBACK(ompt_event_init_lock);
TEST_WAIT_CALLBACK(ompt_event_init_nest_lock);
TEST_WAIT_CALLBACK(ompt_event_destroy_lock);
TEST_WAIT_CALLBACK(ompt_event_destroy_nest_lock);
TEST_THREAD_CALLBACK(ompt_event_flush);

/*******************************************************************
 * Register the events
 *******************************************************************/

#define CHECK(EVENT) \
if (ompt_set_callback(EVENT, my_##EVENT) != 0) { \
  fprintf(stderr,"Failed to register OMPT callback %s!\n",#EVENT); \
}

int ompt_initialize() {

  /* required events */

  CHECK(ompt_event_parallel_create);
  CHECK(ompt_event_parallel_exit);
  CHECK(ompt_event_task_create);
  CHECK(ompt_event_task_exit);
  CHECK(ompt_event_thread_create);
  CHECK(ompt_event_thread_exit);
  CHECK(ompt_event_control);
  CHECK(ompt_event_runtime_shutdown);

  /* optional events, "blameshifting" */

  CHECK(ompt_event_idle_begin);
  CHECK(ompt_event_idle_end);
  //CHECK(ompt_event_wait_barrier_begin);
  //CHECK(ompt_event_wait_barrier_end);
  //CHECK(ompt_event_wait_taskwait_begin);
  //CHECK(ompt_event_wait_taskwait_end);
  //CHECK(ompt_event_wait_taskgroup_begin);
  //CHECK(ompt_event_wait_taskgroup_end);
  //CHECK(ompt_event_release_lock);
  //CHECK(ompt_event_release_nest_lock_last);
  //CHECK(ompt_event_release_critical);
  CHECK(ompt_event_release_atomic);
  CHECK(ompt_event_release_ordered);

  /* optional events, synchronous */

  //CHECK(ompt_event_implicit_task_create);
  //CHECK(ompt_event_implicit_task_exit);
  CHECK(ompt_event_barrier_begin);
  CHECK(ompt_event_barrier_end);
  CHECK(ompt_event_master_begin);
  CHECK(ompt_event_master_end);
  //CHECK(ompt_event_task_switch);
  //CHECK(ompt_event_loop_begin);
  //CHECK(ompt_event_loop_end);
  //CHECK(ompt_event_section_begin);
  //CHECK(ompt_event_section_end);
  //CHECK(ompt_event_single_in_block_begin);
  //CHECK(ompt_event_single_in_block_end);
  //CHECK(ompt_event_single_others_begin);
  //CHECK(ompt_event_single_others_end);
  //CHECK(ompt_event_taskwait_begin);
  //CHECK(ompt_event_taskwait_end);
  //CHECK(ompt_event_taskgroup_begin);
  //CHECK(ompt_event_taskgroup_end);
  //CHECK(ompt_event_release_nest_lock_prev);
  //CHECK(ompt_event_wait_lock);
  //CHECK(ompt_event_wait_nest_lock);
  //CHECK(ompt_event_wait_critical);
  CHECK(ompt_event_wait_atomic);
  //CHECK(ompt_event_wait_ordered);
  //CHECK(ompt_event_acquired_lock);
  //CHECK(ompt_event_acquired_nest_lock_first);
  //CHECK(ompt_event_acquired_nest_lock_next);
  //CHECK(ompt_event_acquired_critical);
  CHECK(ompt_event_acquired_atomic);
  //CHECK(ompt_event_acquired_ordered);
  //CHECK(ompt_event_init_lock);
  //CHECK(ompt_event_init_nest_lock);
  //CHECK(ompt_event_destroy_lock);
  //CHECK(ompt_event_destroy_nest_lock);
  //CHECK(ompt_event_flush);
  return 1;
}


