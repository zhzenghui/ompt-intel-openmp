#include "kmp.h"
#include "ompt-internal.h"
#include "ompt-specific.h"

ompt_state_t __ompt_get_state_internal(ompt_wait_id_t *ompt_wait_id)
{
  kmp_info_t  *ti = ompt_get_thread();

  if (ti) {
    if(ompt_wait_id)
      *ompt_wait_id = ti->th.ompt_thread_info.wait_id;
    return ti->th.ompt_thread_info.state;
  } else {
    return ompt_state_undefined;
  }
}



void *__ompt_get_idle_frame_internal(void)
{
  kmp_info_t  *ti = ompt_get_thread();

  return ti ? ti->th.ompt_thread_info.idle_frame : NULL;
}

// Returns either a kmp_team or a ompt_lw_taskteam_t
// Sets the tid_p to the tid in the kmp_team or -1 for the lwt (tid is always 0 in that case)
// This is to avoid having multiple similar functions as getting the team is not easy (nested cases)
void *__ompt_get_team(int depth, int *tid_p){
  kmp_info_t *thr = ompt_get_thread();
  if(!thr)
      return NULL;

  kmp_team *team = thr->th.th_team;
  int tid = thr->th.th_info.ds.ds_tid;
  ompt_lw_taskteam_t *lwt = team->t.ompt_serialized_team_info;
#ifdef KMP_DEBUG
  int serializedCt = team->t.t_serialized;
#endif
  
  while(depth > 0){
    if(lwt){
      lwt = lwt->parent;
#ifdef KMP_DEBUG
      serializedCt--;
#endif
      if(!lwt) {
	if (team && team->t.t_parent){
          tid=team->t.t_master_tid;
          team=team->t.t_parent;
          lwt=team->t.ompt_serialized_team_info;
#ifdef KMP_DEBUG
          serializedCt = team->t.t_serialized;
#endif
	} else {
          return ompt_task_id_none;   
	}
      }
    }else{
      if(team && team->t.t_parent) {
#ifdef KMP_DEBUG
      	KMP_DEBUG_ASSERT2(!serializedCt, "OMPT lwt entries inconsistent!");
#endif
      	tid=team->t.t_master_tid;
      	team=team->t.t_parent;
      	lwt=team->t.ompt_serialized_team_info;
#ifdef KMP_DEBUG
      	serializedCt = team->t.t_serialized;
#endif
      } else {
        return ompt_task_id_none;   
      }
    }
    depth--;
  }
  if(lwt){
    *tid_p = -1;
    return lwt;
  }else if(team){
    *tid_p = tid;
    return team;
  }else
    return NULL;
}


void *__ompt_get_parallel_function_internal(int ancestor_level) 
{
  int tid;
  void* team=__ompt_get_team(ancestor_level, &tid);
  if(!team)
    return NULL;
  if(tid < 0) {
    ompt_lw_taskteam_t *lwt = (ompt_lw_taskteam_t*) team;
    return lwt->ompt_team_info.microtask;
  } else {
    kmp_team_t *kteam = (kmp_team*) team;
    return (void *) kteam->t.t_pkfn;
  }
}


ompt_parallel_id_t __ompt_get_parallel_id_internal(int ancestor_level) 
{
  int tid;
  void* team=__ompt_get_team(ancestor_level, &tid);
  if(!team)
    return NULL;
  if(tid < 0) {
    ompt_lw_taskteam_t *lwt = (ompt_lw_taskteam_t*) team;
    return lwt->ompt_team_info.parallel_id;
  } else {
    kmp_team_t *kteam = (kmp_team*) team;
    return kteam->t.ompt_team_info.parallel_id;
  }
}


int __ompt_get_parallel_team_size_internal(int ancestor_level)
{
  int tid;
  void* team=__ompt_get_team(ancestor_level, &tid);
  if(!team)
    return NULL;
  if(tid < 0) {
    return 1; // LWT -> Serial team -> 1 Thread
  } else {
    kmp_team_t *kteam = (kmp_team*) team;
    return kteam->t.t_nproc;
  }
}


ompt_thread_id_t __ompt_get_thread_id_internal() 
{
  //assert(0);

  // FIXME
  // until we have a better way of assigning ids, use __kmp_get_gtid
  // since the return value might be negative, we need to test that before
  // assigning it to an ompt_thread_id_t, which is unsigned.
  int id = __kmp_get_gtid();
  assert(id >= 0); 

  return (ompt_thread_id_t) (id >=0) ? id + 1: 0;
}


ompt_task_id_t __ompt_get_task_id_internal(int depth) 
{
  int tid;
  void* team=__ompt_get_team(depth, &tid);
  if(!team)
    return NULL;
  if(tid < 0) {
    ompt_lw_taskteam_t *lwt = (ompt_lw_taskteam_t*) team;
    return lwt->ompt_task_info.task_id;
  } else {
    kmp_team_t *kteam = (kmp_team*) team;
    return kteam->t.t_implicit_task_taskdata[tid].ompt_task_info.task_id;
  }
}


void *__ompt_get_task_function_internal(int depth) 
{
  int tid;
  void* team=__ompt_get_team(depth, &tid);
  if(!team)
    return NULL;
  if(tid < 0) {
    ompt_lw_taskteam_t *lwt = (ompt_lw_taskteam_t*) team;
    return lwt->ompt_task_info.function;
  } else {
    kmp_team_t *kteam = (kmp_team*) team;
    return kteam->t.t_implicit_task_taskdata[tid].ompt_task_info.function;
  }
}


ompt_frame_t *__ompt_get_task_frame_internal(int depth) 
{
  int tid;
  void* team=__ompt_get_team(depth, &tid);
  if(!team)
    return NULL;
  if(tid < 0) {
    ompt_lw_taskteam_t *lwt = (ompt_lw_taskteam_t*) team;
    return &lwt->ompt_task_info.frame;
  } else {
    kmp_team_t *kteam = (kmp_team*) team;
    return &kteam->t.t_implicit_task_taskdata[tid].ompt_task_info.frame;
  }
}



#define OMPT_THREAD_ID_BITS 16

// 2013 08 24 - John Mellor-Crummey
//   ideally, a thread should assign its own ids based on thread private data. however, the way the intel 
//   runtime reinitializes thread data structures when it creates teams makes it difficult to maintain 
//   persistent thread data. using a shared variable instead is simple. I leave it to intel to sort 
//   out how to implement a higher performance version in their runtime.  

// when using fetch_and_add to generate the IDs, there isn't any reason to waste bits for thread id.
#if 0
#define NEXT_ID(id_ptr,tid) ((__sync_fetch_and_add(id_ptr, 1) << OMPT_THREAD_ID_BITS) | (tid))
#else
#define NEXT_ID(id_ptr,tid) (__sync_fetch_and_add(id_ptr, 1)) 
#endif


ompt_parallel_id_t __ompt_thread_id_new()
{
  static uint64_t ompt_thread_id = 1;
  return NEXT_ID(&ompt_thread_id, 0);
}

ompt_parallel_id_t __ompt_parallel_id_new(int gtid)
{
  static uint64_t ompt_parallel_id = 1;
  return gtid >= 0 ? NEXT_ID(&ompt_parallel_id, gtid) : 0;
}


ompt_task_id_t __ompt_task_id_new(int gtid)
{
  static uint64_t ompt_task_id = 1;
  return NEXT_ID(&ompt_task_id, gtid);
}


void __ompt_team_assign_id(kmp_team_t *team, ompt_parallel_id_t ompt_pid)
{
  team->t.ompt_team_info.parallel_id = ompt_pid;
}


void __ompt_thread_assign_wait_id(void *variable)
{
  int gtid = __kmp_gtid_get_specific();
  kmp_info_t *ti = ompt_get_thread_gtid(gtid);

  ti->th.ompt_thread_info.wait_id = (ompt_wait_id_t)(variable);
}


void __ompt_lw_taskteam_init(ompt_lw_taskteam_t *lwt, kmp_info_t *thr, 
                             int gtid, microtask_t microtask, ompt_parallel_id_t ompt_pid)
{
  lwt->ompt_team_info.parallel_id = ompt_pid;
  lwt->ompt_team_info.microtask = (void *) microtask;
  lwt->ompt_task_info.task_id = 0;
  lwt->ompt_task_info.frame.reenter_runtime_frame = 0;
  lwt->ompt_task_info.frame.exit_runtime_frame = 0;
  lwt->ompt_task_info.function = NULL;
  lwt->parent = 0;
}


void __ompt_lw_taskteam_link(ompt_lw_taskteam_t *lwt,  kmp_info_t *thr)
{
  ompt_lw_taskteam_t *my_parent = thr->th.th_team->t.ompt_serialized_team_info;
  lwt->parent = my_parent;
  thr->th.th_team->t.ompt_serialized_team_info = lwt;
}


void __ompt_lw_taskteam_unlink(ompt_lw_taskteam_t *lwt, kmp_info_t *thr)
{
  thr->th.th_team->t.ompt_serialized_team_info = lwt->parent;
}


const char *__ompt_get_runtime_version_internal()
{
  return &__kmp_version_lib_ver[KMP_VERSION_MAGIC_LEN];
}


ompt_target_id_t __ompt_target_id_new()
{
  static uint64_t ompt_target_id = 1;
  return NEXT_ID(&ompt_target_id, 0);
}

ompt_target_data_id_t __ompt_target_data_id_new()
{
  static uint64_t ompt_target_data_id = 1;
  return NEXT_ID(&ompt_target_data_id, 0);
}


ompt_target_id_t __ompt_get_target_id_internal()
{
  int tid;
  void* team=__ompt_get_team(0, &tid);
  if(!team)
    return NULL;
  if(tid < 0) {
    ompt_lw_taskteam_t *lwt = (ompt_lw_taskteam_t*) team;
    return lwt->ompt_task_info.target_id;
  } else {
    kmp_team_t *kteam = (kmp_team*) team;
    return kteam->t.t_implicit_task_taskdata[tid].ompt_task_info.target_id;
  }
}
