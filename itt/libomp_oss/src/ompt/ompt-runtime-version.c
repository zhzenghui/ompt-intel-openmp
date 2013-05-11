#include "ompt-openmp-interface.h"

#include "ompt.h"
#include "ompt-internal.h"

_OMP_EXTERN int ompt_get_runtime_version(char *buffer, int length)
{
  return ompt_get_runtime_version_internal(buffer,length);
}
