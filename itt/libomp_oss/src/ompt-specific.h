#ifndef OMPT_SPECIFIC_H
#define OMPT_SPECIFIC_H

#include "kmp.h"

void *__ompt_get_team(int depth, int *tid_p);

void __ompt_team_assign_id(kmp_team_t *team, ompt_parallel_id_t ompt_pid);
void __ompt_thread_assign_wait_id(void *variable);

void __ompt_lw_taskteam_init(ompt_lw_taskteam_t *lwt, kmp_info_t *thr, 
                             int gtid, void *microtask, ompt_parallel_id_t ompt_pid);

void __ompt_lw_taskteam_link(ompt_lw_taskteam_t *lwt,  kmp_info_t *thr);

ompt_lw_taskteam_t * __ompt_lw_taskteam_unlink(kmp_info_t *thr);

ompt_parallel_id_t __ompt_parallel_id_new(int gtid);
ompt_task_id_t __ompt_task_id_new(int gtid);

ompt_team_info_t *__ompt_get_teaminfo(int depth, int *size);

ompt_task_info_t *__ompt_get_taskinfo(int depth);

/* the wait_id assignment is needed in many, many macros - so define
 * a wrapper macro which can be a noop when it is not wanted.
 */
#if OMPT_SUPPORT
#define OMPT_THREAD_ASSIGN_WAIT_ID(DATA) __ompt_thread_assign_wait_id(DATA);
#else
#define OMPT_THREAD_ASSIGN_WAIT_ID(DATA)
#endif


inline
kmp_info_t *ompt_get_thread_gtid(int gtid)
{
  return (gtid >= 0) ? __kmp_thread_from_gtid(gtid) : NULL;
}


void __ompt_thread_begin(ompt_thread_type_t thread_type, int gtid);


void __ompt_thread_end(ompt_thread_type_t thread_type, int gtid);


inline
kmp_info_t *ompt_get_thread()
{
  int gtid = __kmp_gtid_get_specific();
  return ompt_get_thread_gtid(gtid);
}

int __ompt_get_parallel_team_size_internal(int ancestor_level); 

ompt_task_id_t __ompt_get_task_id_internal(int depth); 

ompt_frame_t *__ompt_get_task_frame_internal(int depth); 

ompt_target_id_t __ompt_target_id_new();

void __ompt_initialize_openmp_runtime();

ompt_target_info_t* __ompt_get_target_info();

#if 0
ompt_target_id_t __ompt_get_target_id_internal();
#endif

ompt_data_map_id_t __ompt_data_map_id_new();

#endif
