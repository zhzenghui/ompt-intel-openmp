/* -------------- OMPT events -------------- */

  /*--- Mandatory Events ---*/
ompt_event(ompt_event_parallel_create, ompt_parallel_callback_t, 1) /* parallel create */
ompt_event(ompt_event_parallel_exit, ompt_new_parallel_callback_t, 2) /* parallel exit */

ompt_event(ompt_event_task_create, ompt_new_task_callback_t, 3) /* task create */
ompt_event(ompt_event_task_exit, ompt_task_callback_t, 4) /* task destroy */

ompt_event(ompt_event_thread_create, ompt_thread_callback_t, 5) /* thread create */
ompt_event(ompt_event_thread_exit, ompt_thread_callback_t, 6) /* thread exit */

ompt_event(ompt_event_control, ompt_control_callback_t, 7) /* support control calls */

ompt_event(ompt_event_runtime_shutdown, ompt_callback_t, 8) /* runtime shutdown */

  /*--- Optional Events (blame shifting) ---*/
ompt_event(ompt_event_idle_begin, ompt_thread_callback_t, 9) /* begin idle state */
ompt_event(ompt_event_idle_end, ompt_thread_callback_t, 10) /* end idle state */

ompt_event(ompt_event_wait_barrier_begin, ompt_parallel_callback_t, 11) /* begin wait at barrier */
ompt_event(ompt_event_wait_barrier_end, ompt_parallel_callback_t, 12) /* end wait at barrier */

ompt_event(ompt_event_wait_taskwait_begin, ompt_parallel_callback_t, 13) /* begin wait at taskwait */
ompt_event(ompt_event_wait_taskwait_end, ompt_parallel_callback_t, 14) /* end wait at taskwait */

ompt_event(ompt_event_wait_taskgroup_begin, ompt_parallel_callback_t, 15) /* begin wait at taskgroup */
ompt_event(ompt_event_wait_taskgroup_end, ompt_parallel_callback_t, 16) /* end wait at taskgroup */

ompt_event(ompt_event_release_lock, ompt_wait_callback_t, 17) /* lock release */
ompt_event(ompt_event_release_nest_lock_last, ompt_wait_callback_t, 18) /* last nest lock release */
ompt_event(ompt_event_release_critical, ompt_wait_callback_t, 19) /* critical release */

ompt_event(ompt_event_release_atomic, ompt_wait_callback_t, 20) /* atomic release */

ompt_event(ompt_event_release_ordered, ompt_wait_callback_t, 21) /* ordered release */

  /*--- Optional Events (synchronous events) --- */
ompt_event(ompt_event_implicit_task_create, ompt_parallel_callback_t, 22) /* implicit task create   */
ompt_event(ompt_event_implicit_task_exit, ompt_parallel_callback_t, 23) /* implicit task destroy  */

ompt_event(ompt_event_task_switch, ompt_task_switch_callback_t, 24) /* task switch */

ompt_event(ompt_event_loop_begin, ompt_parallel_callback_t, 25) /* task at loop begin */
ompt_event(ompt_event_loop_end, ompt_parallel_callback_t, 26) /* task at loop end */

ompt_event(ompt_event_section_begin, ompt_parallel_callback_t, 27) /* task at section begin  */
ompt_event(ompt_event_section_end, ompt_parallel_callback_t, 28) /* task at section end */

ompt_event(ompt_event_single_in_block_begin, ompt_parallel_callback_t, 29) /* task at single begin*/
ompt_event(ompt_event_single_in_block_end, ompt_parallel_callback_t, 30) /* task at single end */

ompt_event(ompt_event_single_others_begin, ompt_parallel_callback_t, 31) /* task at single begin */
ompt_event(ompt_event_single_others_end, ompt_parallel_callback_t, 32) /* task at single end */

ompt_event(ompt_event_master_begin, ompt_parallel_callback_t, 33) /* task at master begin */
ompt_event(ompt_event_master_end, ompt_parallel_callback_t, 34) /* task at master end */

ompt_event(ompt_event_barrier_begin, ompt_parallel_callback_t, 35) /* task at barrier begin  */
ompt_event(ompt_event_barrier_end, ompt_parallel_callback_t, 36) /* task at barrier end */

ompt_event(ompt_event_taskwait_begin, ompt_parallel_callback_t, 37) /* task at taskwait begin */
ompt_event(ompt_event_taskwait_end, ompt_parallel_callback_t, 38) /* task at task wait end */

ompt_event(ompt_event_taskgroup_begin, ompt_parallel_callback_t, 39) /* task at taskgroup begin */
ompt_event(ompt_event_taskgroup_end, ompt_parallel_callback_t, 40) /* task at taskgroup end */

ompt_event(ompt_event_release_nest_lock_prev, ompt_parallel_callback_t, 41) /* prev nest lock release */

ompt_event(ompt_event_wait_lock, ompt_wait_callback_t, 42) /* lock wait */
ompt_event(ompt_event_wait_nest_lock, ompt_wait_callback_t, 43) /* nest lock wait */
ompt_event(ompt_event_wait_critical, ompt_wait_callback_t, 44) /* critical wait */
ompt_event(ompt_event_wait_atomic, ompt_wait_callback_t, 45) /* atomic wait */
ompt_event(ompt_event_wait_ordered, ompt_wait_callback_t, 46) /* ordered wait */

ompt_event(ompt_event_acquired_lock, ompt_wait_callback_t, 47) /* lock acquired */
ompt_event(ompt_event_acquired_nest_lock_first, ompt_wait_callback_t, 48) /* 1st nest lock acquired */
ompt_event(ompt_event_acquired_nest_lock_next, ompt_parallel_callback_t, 49) /* next nest lock acquired*/
ompt_event(ompt_event_acquired_critical, ompt_wait_callback_t, 50) /* critical acquired */
ompt_event(ompt_event_acquired_atomic, ompt_wait_callback_t, 51) /* atomic acquired */
ompt_event(ompt_event_acquired_ordered, ompt_wait_callback_t, 52) /* ordered acquired */

ompt_event(ompt_event_init_lock, ompt_wait_callback_t, 53) /* lock init */
ompt_event(ompt_event_init_nest_lock, ompt_wait_callback_t, 54) /* nest lock init */
ompt_event(ompt_event_destroy_lock, ompt_wait_callback_t, 55) /* lock destruction */
ompt_event(ompt_event_destroy_nest_lock, ompt_wait_callback_t, 56) /* nest lock destruction */

ompt_event(ompt_event_flush, ompt_thread_callback_t, 57) /* after executing flush */
