#ifndef __OMPT_TYPES_H__
#define __OMPT_TYPES_H__

#include <stdint.h>

/* -------------- OMPT state type -------------- */

typedef enum {
#define ompt_state(state, code) state = code,
#include "ompt-state.h"
#undef ompt_state
} ompt_state_t;


/* -------------- OMPT data types -------------- */

typedef uint64_t ompt_parallel_id_t;
typedef uint64_t ompt_wait_id_t;

typedef union ompt_data_u {
  uint64_t value;               /* data under tool control    */
  void *ptr;                    /* pointer under tool control */
} ompt_data_t;

typedef struct ompt_frame_s {
   void *exit_runtime_frame;    /* next frame is user code     */
   void *reenter_runtime_frame; /* previous frame is user code */
} ompt_frame_t;


/* -------------- OMPT callback interfaces -------------- */

typedef void (*ompt_thread_callback_t) (
  ompt_data_t *thread_data           /* tool data for thread       */
  );

typedef void (*ompt_parallel_callback_t) (
  ompt_data_t *task_data,           /* tool data for a task        */
  ompt_parallel_id_t parallel_id    /* id of parallel region       */
  );

typedef void (*ompt_new_parallel_callback_t) (
  ompt_data_t  *parent_task_data,   /* tool data for parent task   */
  ompt_frame_t *parent_task_frame,  /* frame data of parent task   */
  ompt_parallel_id_t parallel_id    /* id of parallel region       */
  );

typedef void (*ompt_task_callback_t) (
  ompt_data_t *task_data            /* tool data for task          */
  );

typedef void (*ompt_task_switch_callback_t) (
  ompt_data_t *suspended_task_data, /* tool data for suspended task */
  ompt_data_t *resumed_task_data    /* tool data for resumed task   */
  );

typedef void (*ompt_new_task_callback_t) (
  ompt_data_t  *parent_task_data,   /* tool data for parent task    */
  ompt_frame_t *parent_task_frame,  /* frame data for parent task   */
  ompt_data_t  *new_task_data       /* tool data for created task   */
  );

typedef void (*ompt_wait_callback_t) (
  ompt_wait_id_t wait_id            /* wait id                      */
  );

typedef void (*ompt_control_callback_t) (
  uint64_t command,                /* command of control call      */
  uint64_t modifier                /* modifier of control call     */
  );

typedef void (*ompt_callback_t) (
  );

#endif
