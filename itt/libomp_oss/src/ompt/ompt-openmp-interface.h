#include "ompt-internal.h"

extern ompt_state_t ompt_get_state_internal(ompt_wait_id_t *ompt_wait_id)
ompt_data_t *ompt_get_thread_data_internal(void);

int ompt_get_runtime_version_internal(char *buffer, int length);


ompt_parallel_id_t ompt_get_parallel_id_internal(int ancestor_level);
void *ompt_get_parallel_function_internal(int ancestor_level);
