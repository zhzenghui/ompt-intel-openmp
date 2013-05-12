#ifndef __OMPT_INTERNAL_H__
#define __OMPT_INTERNAL_H__

#include "ompt.h"

#define OMPT_VERSION 1

#define _OMP_EXTERN

typedef enum {
  ompt_status_disabled,
  ompt_status_ready,
  ompt_status_track,
  ompt_status_track_callback,
} ompt_status_t;

#include "ompt-event-callbacks.h"

typedef struct {
  ompt_state_t     ompt_state;
  ompt_wait_id_t   ompt_wait_id;
  ompt_data_t      ompt_data;             /* OMPT tool data */
} ompt_thread_state_t;

extern ompt_status_t ompt_status;

#endif
