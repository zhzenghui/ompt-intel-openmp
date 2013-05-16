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
  for (i = 0; i < ancestor_level; i++) {
      team = team ? team->t.t_parent : NULL;
  }
  return team;
} 


/* safely extract a task from a thread. 
 * - a thread may not be an openmp thread
 * - an openmp thread may have an uninitialized task
 */
kmp_taskdata_t *ompt_task(int ancestor_level)
{
  int i;
  kmp_info_t *th = ompt_get_thread();
  kmp_taskdata_t *task = th ? th->th.th_current_task : NULL;
  for (i = 0; i < ancestor_level; i++) {
      task = task ? task->td_parent : NULL;
  }
  return task;
} 


ompt_state_t __ompt_get_state_internal(ompt_wait_id_t *ompt_wait_id)
{
  kmp_info_t  *ti = ompt_get_thread();
  ompt_state_t state = ompt_state_undefined;
  if (ti) {
    state = ti->th.ompt_thread_info.state;
    *ompt_wait_id = ti->th.ompt_thread_info.wait_id;
  } 
  return state;
}


ompt_data_t *__ompt_get_thread_data_internal(void)
{
  kmp_info_t  *ti = ompt_get_thread();
  ompt_data_t *data = ti ?  &ti->th.ompt_thread_info.data : NULL;
  return data;
}


void *__ompt_get_parallel_function_internal(int ancestor_level) 
{
  kmp_team_t *team = ompt_team(ancestor_level);
  microtask_t task =  team ? team->t.t_pkfn : NULL;
  return (void *) task;
}


ompt_parallel_id_t __ompt_get_parallel_id_internal(int ancestor_level) 
{
  kmp_team_t *team = ompt_team(ancestor_level);
  ompt_parallel_id_t id =  team ? team->t.ompt_team_info.parallel_id : 0;
  return id;
}


#define OMPT_THREAD_ID_BITS 20
void __ompt_team_assign_id(kmp_team_t *team)
{
  int gtid = __kmp_gtid_get_specific();
  kmp_info_t *ti = ompt_get_thread_gtid(gtid);
  team->t.ompt_team_info.parallel_id = 
     ti ? ((ti->th.ompt_thread_info.next_parallel_id++ << OMPT_THREAD_ID_BITS) | gtid) : 0;
}


ompt_data_t *__ompt_get_task_data_internal(int ancestor_level) 
{
  kmp_taskdata_t *task = ompt_task(ancestor_level);
  ompt_data_t *data =  task ? &task->ompt_task_info.data : NULL;
  return data;
}


void *__ompt_get_task_function_internal(int ancestor_level) 
{
  kmp_taskdata_t *td = ompt_task(ancestor_level);
  kmp_task_t *task = td ? KMP_TASKDATA_TO_TASK(td) : NULL;
  void *fcn =  (void *) (task ? task->routine : NULL);
  return fcn;
}

ompt_frame_t *__ompt_get_task_frame_internal(int ancestor_level) 
{
  kmp_taskdata_t *task = ompt_task(ancestor_level);
  ompt_frame_t *frame =  task ? &task->ompt_task_info.frame : NULL;
  return frame;
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
