#include "ompt-event-enum.h"
#include "ompt-event-callbacks.h"

#define set_success 0
#define set_failure -1

#define get_success 0
#define get_failure -1

#define no_tool_present 0



ompt_callbacks_t ompt_callbacks;



_OMP_EXTERN int ompt_set_callback(ompt_event_t evid, ompt_callback_t cb)
{
  switch (evid) {

#define ompt_event(event_name, callback_type, event_id, is_impl) \
	case event_name: if (is_impl) { ompt_callbacks . ompt_callback(event_name) = (callback_type) cb; return set_success; } else { return set_failure; }

#include "ompt-event.h"

#undef ompt_event

  default: return set_failure;
  }
}


_OMP_EXTERN int ompt_get_callback(ompt_event_t evid, ompt_callback_t *cb)
{
  switch (evid) {

#define ompt_event(event_name, callback_type, event_id, is_impl) \
  case event_name:  \
    if (is_impl) { \
      ompt_callback_t mycb = (ompt_callback_t) ompt_callbacks . ompt_callback(event_name); \
      if (mycb) { \
        *cb = mycb; \
         return get_success; \
      } \
    } \
    return get_failure; 

#include "ompt-event.h"

#undef ompt_event

  default: return get_failure;
  }
}
