#include "kmp.h"
#include "ompt-internal.h"
#include "ompt-specific.h"

#define GTID_TO_OMPT_THREAD_ID(id) ((ompt_thread_id_t) (id >=0) ? id + 1: 0)

void
__ompt_init_internal()
{
  if (ompt_status & ompt_status_track) {
    // initialize initial thread for OMPT
    kmp_info_t  *root_thread = ompt_get_thread();
    __kmp_task_init_ompt(root_thread->th.th_team->t.t_implicit_task_taskdata, 0);
    __kmp_task_init_ompt(root_thread->th.th_serial_team->t.t_implicit_task_taskdata, 0);

    // make mandatory callback for creation of initial thread
    // this needs to occur here rather than in __kmp_register_root because
    // __kmp_register_root is called before ompt_initialize
    int gtid = __kmp_get_gtid();
    if (KMP_UBER_GTID(gtid)) {
      // initialize the initial thread's idle frame and state
      root_thread->th.ompt_thread_info.idle_frame = 0;
      root_thread->th.ompt_thread_info.state = ompt_state_overhead;
      if ((ompt_status == ompt_status_track_callback) &&
           ompt_callbacks.ompt_callback(ompt_event_thread_begin)) {
        __ompt_thread_begin(ompt_thread_initial, gtid);
      }
    }
  }
}


void
__ompt_thread_begin(ompt_thread_type_t thread_type, int gtid)
{
  ompt_callbacks.ompt_callback(ompt_event_thread_begin)(thread_type, 
                               GTID_TO_OMPT_THREAD_ID(gtid));
}


void
__ompt_thread_end(ompt_thread_type_t thread_type, int gtid)
{
  ompt_callbacks.ompt_callback(ompt_event_thread_end)(thread_type, 
                               GTID_TO_OMPT_THREAD_ID(gtid));
}


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


ompt_team_info_t *
__ompt_get_teaminfo(int depth, int *size)
{
  kmp_info_t *thr = ompt_get_thread();
  if(!thr) return NULL;

  kmp_team *team = thr->th.th_team;
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
          team=team->t.t_parent;
#if 0
          lwt=team->t.ompt_serialized_team_info;
#ifdef KMP_DEBUG
          serializedCt = team->t.t_serialized;
#endif
#endif
	} else {
          return NULL;
	}
      }
    }else{
      if(team && team->t.t_parent) {
#if 0
#ifdef KMP_DEBUG
      	KMP_DEBUG_ASSERT2(!serializedCt, "OMPT lwt entries inconsistent!");
#endif
#endif
      	team=team->t.t_parent;
#if 0
      	lwt=team->t.ompt_serialized_team_info;
#ifdef KMP_DEBUG
      	serializedCt = team->t.t_serialized;
#endif
#endif
      } else {
        return NULL;
      }
    }
    depth--;
  }

  if (lwt)  {
    if (size) *size = 1;
    return &lwt->ompt_team_info;
  }

  if (team) {
    if (size) *size = team->t.t_nproc;
    return &team->t.ompt_team_info;
  }

  return NULL;
}


void *__ompt_get_parallel_function_internal(int depth) 
{
  ompt_team_info_t *info = __ompt_get_teaminfo(depth, NULL);
  void *function = info ? info->microtask : NULL;
  return function;
}


ompt_parallel_id_t __ompt_get_parallel_id_internal(int depth) 
{
  ompt_team_info_t *info = __ompt_get_teaminfo(depth, NULL);
  ompt_parallel_id_t id = info ? info->parallel_id : 0;
  return id;
}


int __ompt_get_parallel_team_size_internal(int depth)
{
  // initialize the return value with the error value. 
  // if there is a team at the specified depth, the default 
  // value will be overwritten the size of that team. 
  int size = -1; 
  (void) __ompt_get_teaminfo(depth, &size);
  return size;
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

  return GTID_TO_OMPT_THREAD_ID(id);
}

ompt_task_info_t *
__ompt_get_taskinfo(int depth) 
{
  ompt_task_info_t *info = NULL;

  int gtid = __kmp_get_gtid();

  if(gtid >= 0) {
    kmp_info_t *thr = ompt_get_thread_gtid(gtid);
    if (thr == NULL) return NULL;

    kmp_taskdata_t  *taskdata = 
      __kmp_threads[ gtid ] -> th.th_current_task;
    ompt_lw_taskteam_t *lwt = NULL;
 
    lwt = taskdata->td_team->t.ompt_serialized_team_info;
    while (depth > 0) {
      if (lwt) {
        lwt = lwt->parent;
      } 
      if (!lwt && taskdata) { 
        taskdata = taskdata->td_parent;
        if (taskdata) {
          lwt = taskdata->td_team->t.ompt_serialized_team_info;
        }
      }
      depth--;
    }    

    if(lwt){
      info = &lwt->ompt_task_info;
    } else if (taskdata) {
      info = &taskdata->ompt_task_info;
    }
  }
  return info;
}


ompt_task_id_t 
__ompt_get_task_id_internal(int depth) 
{
  ompt_task_info_t *info = __ompt_get_taskinfo(depth); 
  ompt_task_id_t task_id = info ?  info->task_id : 0;
  return task_id;
}


void *__ompt_get_task_function_internal(int depth) 
{
  ompt_task_info_t *info = __ompt_get_taskinfo(depth); 
  void *function = info ? info->function : NULL;
  return function;
}


ompt_frame_t *__ompt_get_task_frame_internal(int depth) 
{
  ompt_task_info_t *info = __ompt_get_taskinfo(depth); 
  ompt_frame_t *frame = info ? frame = &info->frame : NULL;
  return frame;
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
