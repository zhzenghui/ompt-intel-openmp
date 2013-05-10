#define _OMP_EXTERN

typedef enum {
  ompt_status_disabled,
  ompt_status_ready,
  ompt_status_track,
  ompt_status_track_callbacks,
} ompt_status_t;

#include "ompt-event-callbacks.h"

extern ompt_status_t ompt_status;
