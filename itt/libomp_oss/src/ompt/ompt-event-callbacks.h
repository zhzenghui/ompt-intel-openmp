#ifndef __OMPT_EVENT_CALLBACKS_H__
#define __OMPT_EVENT_CALLBACKS_H__

#include "ompt-types.h"

typedef struct ompt_callbacks_s {

#define ompt_event(event, callback, eventid) callback event ## _callback; 

#include "ompt-event.h"

#undef ompt_event

} ompt_callbacks_t;

extern ompt_callbacks_t ompt_callbacks;

#endif
