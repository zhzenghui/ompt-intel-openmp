#include "ompt-event-enum.h"
#include "ompt-event-callbacks.h"

#define set_success 0
#define set_failure -1

#define get_success 0
#define get_failure -1


int
ompt_set_callback(ompt_event_t evid, ompt_callback_t cb)
{
  switch (evid) {

#define ompt_event(event_name, callback_type, event_id) \
	case event_name: ompt_callbacks.event_name ## _callback = (callback_type) cb; return set_success;

#include "ompt-event.h"

#undef ompt_event

  default:
    return set_failure;
  }
}


int
ompt_get_callback(ompt_event_t evid, ompt_callback_t *cb)
{
  switch (evid) {

#define ompt_event(event_name, callback_type, event_id) \
	case event_name:  *cb = (ompt_callback_t) ompt_callbacks.event_name ## _callback; return get_success;

#include "ompt-event.h"

#undef ompt_event

  default:
    return get_failure;
  }
}

__attribute__ (( weak ))
ompt_initialize()
{
}
