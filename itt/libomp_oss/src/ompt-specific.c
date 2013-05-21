#include "kmp.h"
#include "ompt-internal.h"


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


/* safely extract team from a thread. 
 * - a thread may not be an openmp thread
 * - an openmp thread may have an uninitialized team
 */
kmp_team_t *ompt_team(int ancestor_level)
{
  int i;
  kmp_info_t *th = ompt_get_thread();
  kmp_team_t *team = th ? th->th.th_team : NULL;

  for (i = 0; team && (i < ancestor_level); i++) {
    team = team->t.t_parent;
  }
  return team;
} 


/* safely extract a task from a thread. 
 * - a thread need not be an openmp thread
 * - an openmp thread may have an uninitialized task
 */
kmp_taskdata_t *ompt_task(int ancestor_level)
{
  int i;
  kmp_info_t *th = ompt_get_thread();
  kmp_taskdata_t *task = th ? th->th.th_current_task : NULL;

  for (i = 0; task && (i < ancestor_level); i++) {
    task = task->td_parent;
  }
  return task;
} 


ompt_state_t __ompt_get_state_internal(ompt_wait_id_t *ompt_wait_id)
{
  kmp_info_t  *ti = ompt_get_thread();

  if (ti) {
    *ompt_wait_id = ti->th.ompt_thread_info.wait_id;
    return ti->th.ompt_thread_info.state;
  } else {
    return ompt_state_undefined;
  }
}


ompt_data_t *__ompt_get_thread_data_internal(void)
{
  kmp_info_t  *ti = ompt_get_thread();

  return ti ?  &ti->th.ompt_thread_info.data : NULL;
}


ompt_lw_taskteam_t *__ompt_get_lw_taskteam(int *ancestor_level)
{
  int level = *ancestor_level;
  kmp_info_t  *ti = ompt_get_thread();
  if (ti) {
    ompt_lw_taskteam_t *lwt = ti->th.ompt_thread_info.lw_taskteam;
    while (lwt) {
      if (level == 0) return lwt;
      lwt = lwt->parent;
      level--;
    }
    *ancestor_level = level;
  } 
  return NULL;
}


void *__ompt_get_parallel_function_internal(int ancestor_level) 
{
  int level = ancestor_level;
  ompt_lw_taskteam_t *lwt = __ompt_get_lw_taskteam(&level);
  if (lwt) {
    return lwt->microtask;
  } else {
    kmp_team_t *team = ompt_team(level); /* remaining levels */
    return (void *) (team ? team->t.t_pkfn : NULL);
  }
}


ompt_parallel_id_t __ompt_get_parallel_id_internal(int ancestor_level) 
{
  int level = ancestor_level;
  ompt_lw_taskteam_t *lwt = __ompt_get_lw_taskteam(&level);
  if (lwt) {
    return lwt->ompt_team_info.parallel_id;
  } else {
    kmp_team_t *team = ompt_team(level); /* remaining levels */
    return team ? team->t.ompt_team_info.parallel_id : 0; 
  }
}


ompt_data_t *__ompt_get_task_data_internal(int ancestor_level) 
{
  int level = ancestor_level;
  ompt_lw_taskteam_t *lwt = __ompt_get_lw_taskteam(&level);
  if (lwt) {
    return &lwt->ompt_task_info.data;
  } else {
    kmp_taskdata_t *task = ompt_task(level); /* remaining levels */
    return task ? &task->ompt_task_info.data : NULL;
  }
}


void *__ompt_get_task_function_internal(int ancestor_level) 
{
  kmp_taskdata_t *td = ompt_task(ancestor_level);
  kmp_task_t *task = td ? KMP_TASKDATA_TO_TASK(td) : NULL;

  return (void *) (task ? task->routine : NULL);
}


ompt_frame_t *__ompt_get_task_frame_internal(int ancestor_level) 
{
  kmp_taskdata_t *task = ompt_task(ancestor_level);

  return task ? &task->ompt_task_info.frame : NULL;
}


#define OMPT_THREAD_ID_BITS 20
#define NEXT_ID(ti,tid) (((ti)->th.ompt_thread_info.next_parallel_id++ << OMPT_THREAD_ID_BITS) | (tid))

void __ompt_team_assign_id(kmp_team_t *team)
{
  int gtid = __kmp_gtid_get_specific();
  kmp_info_t *ti = ompt_get_thread_gtid(gtid);

  team->t.ompt_team_info.parallel_id = ti ? NEXT_ID(ti, gtid) : 0;
}


void __ompt_thread_assign_wait_id(void *variable)
{
  int gtid = __kmp_gtid_get_specific();
  kmp_info_t *ti = ompt_get_thread_gtid(gtid);

  ti->th.ompt_thread_info.wait_id = (ompt_wait_id_t)(variable);
}


void __ompt_lw_taskteam_init(ompt_lw_taskteam_t *lwt, kmp_info_t *thr, int gtid, microtask_t microtask)
{
  lwt->microtask = (void *) microtask;
  lwt->ompt_team_info.parallel_id = NEXT_ID(thr, gtid);
  lwt->ompt_task_info.data.value = 0;
  lwt->ompt_task_info.frame.reenter_runtime_frame = 0;
  lwt->ompt_task_info.frame.exit_runtime_frame = 0;
  ompt_lw_taskteam_t *my_parent = thr->th.ompt_thread_info.lw_taskteam;
  lwt->parent = my_parent;
}


void __ompt_lw_taskteam_fini(ompt_lw_taskteam_t *lwt, kmp_info_t *thr)
{
  thr->th.ompt_thread_info.lw_taskteam = lwt->parent;
}


int __ompt_get_runtime_version_internal(char *buffer, int length)
{
  int slen = strlen(__kmp_version_lib_ver) + 1; /* include space for null */
  int extra = slen - length;                    
  int result = (extra > 0) ? extra : 0; /* how many characters won't fit */
  int copylen = slen - result; /* all of the characters that will fit */

  /* fill buffer with a null-terminated string that fits */
  strncpy(buffer, __kmp_version_lib_ver, copylen); 
  buffer[copylen-1] = NULL; /* no matter what, last character is null */

  return result;
}
