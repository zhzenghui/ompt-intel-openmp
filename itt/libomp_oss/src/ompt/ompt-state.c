#include "ompt-openmp-interface.h"
#include "ompt-internal.h"

_OMP_EXTERN ompt_state_t ompt_get_state(ompt_wait_id_t *ompt_wait_id)
{
   ompt_state_t thread_state = ompt_get_state_internal(ompt_wait_id);

   if (thread_state == ompt_state_undefined) {
     thread_state = ompt_state_work_serial;
   }

   return thread_state;
}
