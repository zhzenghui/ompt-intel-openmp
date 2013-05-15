#include "ompt-internal.h"

#ifdef __cplusplus
extern "C" {
#endif

extern ompt_state_t __ompt_get_state_internal(ompt_wait_id_t *ompt_wait_id);
extern ompt_data_t *__ompt_get_thread_data_internal(void);
extern int __ompt_get_runtime_version_internal(char *buffer, int length);


extern ompt_parallel_id_t __ompt_get_parallel_id_internal(int ancestor_level);
extern void *__ompt_get_parallel_function_internal(int ancestor_level);

extern ompt_data_t *__ompt_get_task_data_internal(int ancestor_level);
extern void *__ompt_get_task_function_internal(int ancestor_level); 
extern ompt_frame_t *__ompt_get_task_frame_internal(int ancestor_level);

#ifdef __cplusplus
};
#endif
