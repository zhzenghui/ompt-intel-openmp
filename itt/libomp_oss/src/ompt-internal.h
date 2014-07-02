#ifndef __OMPT_INTERNAL_H__
#define __OMPT_INTERNAL_H__

#include "ompt.h"
#include "ompt-event-specific.h"

#define OMPT_VERSION 1

#define _OMP_EXTERN extern "C"



#define ompt_callback(e) e ## _callback

/* track and track_callback share a bit so that one can test whether either is set 
 * by anding a bit.
 */
typedef enum {
  ompt_status_disabled       = 0x0,
  ompt_status_ready          = 0x1,
  ompt_status_track          = 0x2,
  ompt_status_track_callback = 0x6,
} ompt_status_t;


typedef struct ompt_callbacks_s {
#define ompt_event(event, callback, eventid, is_impl) callback ompt_callback(event); 
#include "ompt-event.h"
} ompt_callbacks_t;


typedef struct {
  ompt_frame_t        frame;
  ompt_task_id_t      task_id;
} ompt_task_info_t;


typedef struct {
  ompt_parallel_id_t  parallel_id;
  void                *microtask;
} ompt_team_info_t;


typedef struct ompt_lw_taskteam_s {
  ompt_team_info_t    ompt_team_info;
  ompt_task_info_t    ompt_task_info;
  struct ompt_lw_taskteam_s *parent;  
} ompt_lw_taskteam_t;


typedef struct ompt_parallel_info_s {
  ompt_task_id_t parent_task_id;    /* id of parent task            */
  ompt_parallel_id_t parallel_id;   /* id of parallel region        */
  ompt_frame_t *parent_task_frame;  /* frame data of parent task    */
  void *parallel_function;          /* pointer to outlined function */
} ompt_parallel_info_t;


typedef struct {
  ompt_state_t        state;
  ompt_wait_id_t      wait_id;
  void                *idle_frame;
  ompt_lw_taskteam_t  *lw_taskteam;  
#if 0
  // use shared variables instead of these. repeated initializations of thread 
  // data structures make it hard to maintain persistent thread-private data.
  uint64_t            next_parallel_id;
  uint64_t            next_task_id;
#endif
} ompt_thread_info_t;


extern ompt_status_t ompt_status;
extern ompt_callbacks_t ompt_callbacks;

#ifdef __cplusplus
extern "C" {
#endif

void ompt_init(void);
void ompt_fini(void);

#ifdef __cplusplus
};
#endif

#if 0
/* OMPT wrapper for __kmp_invoke_microtask */
#define __kmp_invoke_microtask_wrapper( pkfn, gtid, npr, argc, argv) \
{ \
   void *dummy; \
   void **exit_runtime_p; \
   ompt_lw_taskteam_t lw_taskteam; \
   if (ompt_status & ompt_status_track) { \
      lw_taskteam.ompt_task_info.task_id = __ompt_task_id_new(gtid); \
      exit_runtime_p = &(lw_taskteam.ompt_task_info.frame.exit_runtime_frame); \
      __ompt_lw_taskteam_init(&lw_taskteam, master_th, gtid, microtask, ompt_parallel_id); \
      __ompt_lw_taskteam_link(&lw_taskteam, master_th); \
      master_th->th.ompt_thread_info.state = ompt_state_work_parallel; \
      if ((ompt_status == ompt_status_track_callback) && \
            ompt_callbacks.ompt_callback(ompt_event_parallel_begin)) { \
         ompt_callbacks.ompt_callback(ompt_event_parallel_begin)( \
               ompt_task_id, ompt_frame, \
               ompt_parallel_id, (void *) microtask); \
      } \
   } else { \
      exit_runtime_p = &dummy; \
   } \
   __kmp_invoke_microtask( pkfn, gtid, 0, argc, argv, exit_runtime_p); \
   if (ompt_status & ompt_status_track) { \
    lw_taskteam.ompt_task_info.frame.exit_runtime_frame = 0; \
    __ompt_lw_taskteam_unlink(&lw_taskteam, master_th); \
    lw_taskteam.ompt_task_info.task_id = ompt_task_id_none; \
    if ((ompt_status == ompt_status_track_callback) && \
      ompt_callbacks.ompt_callback(ompt_event_parallel_end)) { \
        ompt_callbacks.ompt_callback(ompt_event_parallel_end)( \
          lw_taskteam.ompt_task_info.task_id, \
          &lw_taskteam.ompt_task_info.frame, \
          lw_taskteam.ompt_team_info.parallel_id, \
          (void *) microtask \
        ); \
    } \
    master_th->th.ompt_thread_info.state = ompt_state_overhead; \
   } \
}

#endif

#endif
