#ifndef  __OMPT_EVENT_SPECIFIC_H__
#define  __OMPT_EVENT_SPECIFIC_H__

/******************************************************************************
 * File: ompt-event-specific.h
 *
 * Description:
 *
 *   specify which of the OMPT events are implemented by this runtime system
 *
 *****************************************************************************/

/*----------------------------------------------------------------------------
 | specify whether an event is implemented or not. 
 |
 | Note: the value for ompt_event_IMPLEMENTED must be non-zero. it is used
 |       in conditionals as follows
 |
 |          if (ompt_event_IMPLEMENTED) {
 |             ...
 |          }
 +--------------------------------------------------------------------------*/
#define ompt_event_IMPLEMENTED   1
#define ompt_event_unimplemented 0


/*----------------------------------------------------------------------------
 | Mandatory Events 
 +--------------------------------------------------------------------------*/

#define ompt_event_parallel_create_implemented          ompt_event_IMPLEMENTED
#define ompt_event_parallel_exit_implemented            ompt_event_IMPLEMENTED

#define ompt_event_task_create_implemented              ompt_event_IMPLEMENTED
#define ompt_event_task_exit_implemented                ompt_event_IMPLEMENTED

#define ompt_event_thread_create_implemented            ompt_event_IMPLEMENTED
#define ompt_event_thread_exit_implemented              ompt_event_IMPLEMENTED

#define ompt_event_control_implemented                  ompt_event_IMPLEMENTED

#define ompt_event_runtime_shutdown_implemented         ompt_event_IMPLEMENTED


/*----------------------------------------------------------------------------
 | Optional Events (blame shifting) 
 +--------------------------------------------------------------------------*/

#define ompt_event_idle_begin_implemented               ompt_event_IMPLEMENTED
#define ompt_event_idle_end_implemented                 ompt_event_IMPLEMENTED

#define ompt_event_wait_barrier_begin_implemented       ompt_event_unimplemented
#define ompt_event_wait_barrier_end_implemented         ompt_event_unimplemented

#define ompt_event_wait_taskwait_begin_implemented      ompt_event_unimplemented
#define ompt_event_wait_taskwait_end_implemented        ompt_event_unimplemented

#define ompt_event_wait_taskgroup_begin_implemented     ompt_event_unimplemented
#define ompt_event_wait_taskgroup_end_implemented       ompt_event_unimplemented

#define ompt_event_release_lock_implemented             ompt_event_IMPLEMENTED
#define ompt_event_release_nest_lock_implemented        ompt_event_unimplemented
#define ompt_event_release_nest_lock_last_implemented   ompt_event_unimplemented
#define ompt_event_release_critical_implemented         ompt_event_IMPLEMENTED
#define ompt_event_release_atomic_implemented           ompt_event_IMPLEMENTED
#define ompt_event_release_ordered_implemented          ompt_event_IMPLEMENTED


/*----------------------------------------------------------------------------
 | Optional Events (synchronous events) 
 +--------------------------------------------------------------------------*/

#define ompt_event_implicit_task_create_implemented     ompt_event_unimplemented
#define ompt_event_implicit_task_exit_implemented       ompt_event_unimplemented

#define ompt_event_task_switch_implemented              ompt_event_unimplemented

#define ompt_event_loop_begin_implemented               ompt_event_IMPLEMENTED
#define ompt_event_loop_end_implemented                 ompt_event_IMPLEMENTED

#define ompt_event_section_begin_implemented            ompt_event_unimplemented
#define ompt_event_section_end_implemented              ompt_event_unimplemented

#define ompt_event_single_in_block_begin_implemented    ompt_event_IMPLEMENTED
#define ompt_event_single_in_block_end_implemented      ompt_event_IMPLEMENTED
#define ompt_event_single_others_begin_implemented      ompt_event_IMPLEMENTED
#define ompt_event_single_others_end_implemented        ompt_event_unimplemented

#define ompt_event_master_begin_implemented             ompt_event_IMPLEMENTED
#define ompt_event_master_end_implemented               ompt_event_IMPLEMENTED

#define ompt_event_barrier_begin_implemented            ompt_event_IMPLEMENTED
#define ompt_event_barrier_end_implemented              ompt_event_IMPLEMENTED

#define ompt_event_taskwait_begin_implemented           ompt_event_unimplemented
#define ompt_event_taskwait_end_implemented             ompt_event_unimplemented

#define ompt_event_taskgroup_begin_implemented          ompt_event_unimplemented
#define ompt_event_taskgroup_end_implemented            ompt_event_unimplemented

#define ompt_event_release_nest_lock_prev_implemented   ompt_event_unimplemented
#define ompt_event_wait_lock_implemented                ompt_event_unimplemented
#define ompt_event_wait_nest_lock_implemented           ompt_event_unimplemented
#define ompt_event_wait_critical_implemented            ompt_event_unimplemented
#define ompt_event_wait_atomic_implemented              ompt_event_IMPLEMENTED
#define ompt_event_wait_ordered_implemented             ompt_event_unimplemented

#define ompt_event_acquired_lock_implemented            ompt_event_unimplemented
#define ompt_event_acquired_nest_lock_first_implemented ompt_event_unimplemented
#define ompt_event_acquired_nest_lock_next_implemented  ompt_event_unimplemented
#define ompt_event_acquired_critical_implemented        ompt_event_unimplemented
#define ompt_event_acquired_atomic_implemented          ompt_event_IMPLEMENTED
#define ompt_event_acquired_ordered_implemented         ompt_event_unimplemented

#define ompt_event_init_lock_implemented                ompt_event_unimplemented
#define ompt_event_init_nest_lock_implemented           ompt_event_unimplemented

#define ompt_event_destroy_lock_implemented             ompt_event_unimplemented
#define ompt_event_destroy_nest_lock_implemented        ompt_event_unimplemented

#define ompt_event_flush_implemented                    ompt_event_unimplemented

#endif
