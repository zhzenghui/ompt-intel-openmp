#include "ompt-event-enum.h"
#include "ompt-event-callbacks.h"

#define set_success 0
#define set_failure -1

#define get_success 0
#define get_failure -1

#define no_tool_present 0


_OMP_EXTERN int ompt_set_callback(ompt_event_t evid, ompt_callback_t cb)
{
  switch (evid) {

#define ompt_event(event_name, callback_type, event_id) \
	case event_name: ompt_callbacks . ompt_callback(event_name) = (callback_type) cb; return set_success;

#include "ompt-event.h"

#undef ompt_event

  default:
    return set_failure;
  }
}


_OMP_EXTERN int ompt_get_callback(ompt_event_t evid, ompt_callback_t *cb)
{
  switch (evid) {

#define ompt_event(event_name, callback_type, event_id) \
	case event_name:  *cb = (ompt_callback_t) ompt_callbacks . ompt_callback(event_name); return get_success;

#include "ompt-event.h"

#undef ompt_event

  default:
    return get_failure;
  }
}


_OMP_EXTERN __attribute__ (( weak )) int ompt_initialize()
{
  return no_tool_present;
}
