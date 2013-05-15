#ifndef __OMPT___
#define __OMPT___

#include "ompt-event-enum.h"
#include "ompt-types.h"

#ifdef  __cplusplus
extern "C" {
#endif 

/****************************************************************************
 * ompt data 
 ***************************************************************************/

/* debugger interface */
extern const char *ompt_debugger_plugin; /* unimplemented */


/****************************************************************************
 * ompt API 
 ***************************************************************************/

/* initialization */
extern int 			ompt_initialize(void);
extern int 			ompt_set_callback(ompt_event_t evid, ompt_callback_t cb);
extern int 			ompt_get_callback(ompt_event_t evid, ompt_callback_t *cb);

/* state */
extern ompt_state_t 		ompt_get_state(ompt_wait_id_t *ompt_wait_id);

/* thread */
extern ompt_data_t *		ompt_get_thread_data(void);


/* parallel region */
extern ompt_parallel_id_t 	ompt_get_parallel_id(int ancestor_level);
extern void *			ompt_get_parallel_function(int ancestor_level);

/* task */
extern ompt_data_t *		ompt_get_task_data(int ancestor_level);
extern ompt_frame_t *		ompt_get_task_frame(int ancestor_level);
extern void *			ompt_get_task_function(int ancestor_level);

/* library inquiry */
extern int 			ompt_get_runtime_version(char *buffer, int length);
extern int 			ompt_enumerate_state(int current_state, int *next_state, const char **next_state_name);
extern int 			ompt_get_ompt_version(void);

/* control */
extern void 			ompt_control(uint64_t command, uint64_t modifier);

#ifdef  __cplusplus
};
#endif 




#endif

