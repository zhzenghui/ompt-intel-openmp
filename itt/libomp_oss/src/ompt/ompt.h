#include "ompt-event-enum.h"
#include "ompt-types.h"

extern const char *ompt_debugger_plugin;

int ompt_enumerate_state(int current_state, int *next_state, const char **next_state_name);

ompt_parallel_id_t ompt_get_parallel_id(int ancestor_level);
void *ompt_get_parallel_function(int ancestor_level);

ompt_data_t *ompt_get_task_data(int ancestor_level);
ompt_frame_t *ompt_get_task_frame(int ancestor_level);
void *ompt_get_task_function(int ancestor_level);

void ompt_control(uint64_t command, uint64_t modifier);

ompt_state_t ompt_get_state(void);
ompt_data_t *ompt_get_thread_data(void);

extern int ompt_initialize(void);

int ompt_set_callback(ompt_event_t evid, ompt_callback_t cb);
int ompt_get_callback(ompt_event_t evid, ompt_callback_t *cb);

int ompt_get_ompt_version(void);
int ompt_get_runtime_version(char *buffer, int length);

