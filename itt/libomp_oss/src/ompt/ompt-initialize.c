#include "ompt-internal.h"

#define no_tool_present 0

_OMP_EXTERN __attribute__ (( weak )) int ompt_initialize()
{
  return no_tool_present;
}
