#ifndef OMPT_SPECIFIC_H
#define OMPT_SPECIFIC_H

#include "kmp.h"

void __ompt_team_assign_id(kmp_team_t *team, ompt_parallel_id_t ompt_pid);
void __ompt_thread_assign_wait_id(void *variable);

void __ompt_lw_taskteam_init(ompt_lw_taskteam_t *lwt, kmp_info_t *thr, 
                             int gtid, microtask_t microtask, ompt_parallel_id_t ompt_pid);

void __ompt_lw_taskteam_link(ompt_lw_taskteam_t *lwt,  kmp_info_t *thr);

void __ompt_lw_taskteam_unlink(ompt_lw_taskteam_t *lwt, kmp_info_t *thr);

ompt_parallel_id_t __ompt_parallel_id_new(int gtid);
ompt_task_id_t __ompt_task_id_new(int gtid);

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


inline
kmp_info_t *ompt_get_thread()
{
  int gtid = __kmp_gtid_get_specific();
  return ompt_get_thread_gtid(gtid);
}

int __ompt_get_parallel_team_size_internal(int ancestor_level); 

ompt_task_id_t __ompt_get_task_id_internal(int ancestor_level); 

ompt_frame_t *__ompt_get_task_frame_internal(int ancestor_level); 

#endif
