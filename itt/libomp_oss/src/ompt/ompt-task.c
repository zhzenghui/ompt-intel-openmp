#include "ompt-openmp-interface.h"


ompt_data_t *ompt_get_task_data(int ancestor_level)
{
  return __ompt_get_task_data_internal(ancestor_level); 
}

ompt_frame_t *ompt_get_task_frame(int ancestor_level)
{
  return __ompt_get_task_frame_internal(ancestor_level); 
}

void *ompt_get_task_function(int ancestor_level)
{
  return __ompt_get_task_function_internal(ancestor_level); 
}

