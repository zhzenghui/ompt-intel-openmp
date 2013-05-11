#include "ompt-openmp-interface.h"

#include "ompt-internal.h"

_OMP_EXTERN ompt_data_t *ompt_get_thread_data()
{
   return ompt_get_thread_data_internal();
}
