#include <stdlib.h>
#include <stdio.h>

#include "ompt.h"


_OMP_EXTERN void ompt_fini()
{
   ompt_status = ompt_status_disabled;

   if (ompt_callbacks.ompt_callback(ompt_event_shutdown)) {
     ompt_callbacks.ompt_callback(ompt_event_shutdown)();
   }
}
