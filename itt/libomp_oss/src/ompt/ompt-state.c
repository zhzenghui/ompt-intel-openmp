#include "ompt-openmp-interface.h"

#include "ompt-internal.h"

_OMP_EXTERN ompt_state_t ompt_get_state()
{
   return ompt_get_state_internal();
}
