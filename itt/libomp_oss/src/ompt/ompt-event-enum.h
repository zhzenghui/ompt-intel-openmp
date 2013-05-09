#ifndef __OMPT_EVENT_ENUM_H__
#define __OMPT_EVENT_ENUM_H__

typedef enum {

#define ompt_event(event, callback, eventid) event = eventid,

#include "ompt-event.h"

#undef ompt_event

} ompt_event_t;

#endif
