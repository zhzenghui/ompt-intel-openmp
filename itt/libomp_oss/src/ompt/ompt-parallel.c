#include "ompt-openmp-interface.h"

#include "ompt-internal.h"

_OMP_EXTERN ompt_parallel_id_t ompt_get_parallel_id(int ancestor_level)
{
   return __ompt_get_parallel_id_internal(ancestor_level);
}

_OMP_EXTERN void *ompt_get_parallel_function(int ancestor_level) 
{
   return __ompt_get_parallel_function_internal(ancestor_level);
}
