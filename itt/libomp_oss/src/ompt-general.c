/*****************************************************************************
 * system include files
 ****************************************************************************/

#include <assert.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*****************************************************************************
 * ompt include files
 ****************************************************************************/

#include "ompt-internal.h"
#include "ompt-specific.c"



/*****************************************************************************
 * macros
 ****************************************************************************/

#define set_success 0
#define set_failure -1

#define get_success 0
#define get_failure -1

#define no_tool_present 0



/*****************************************************************************
 * types
 ****************************************************************************/

typedef struct {
  char *state_name;
  ompt_state_t  state_id;
} ompt_state_info_t;



/*****************************************************************************
 * global variables
 ****************************************************************************/

ompt_status_t ompt_status = ompt_status_ready;


ompt_state_info_t ompt_state_info[] = {
#define ompt_state(state, code) { # state, state },
#include "ompt-state.h"
};


ompt_callbacks_t ompt_callbacks;



/*****************************************************************************
 * state
 ****************************************************************************/



_OMP_EXTERN int ompt_enumerate_state(int current_state, int *next_state, 
                                     const char **next_state_name)
{
  int i = 0;
  int len = sizeof(ompt_state_info) / sizeof(ompt_state_info_t);

  assert(current_state != ompt_state_last);

  for (i = 0; i < len; i++) {
    if (ompt_state_info[i].state_id == current_state) {
      *next_state = ompt_state_info[i+1].state_id;
      *next_state_name = ompt_state_info[i+1].state_name;
      return 1;
    }
  }

  return 0;
}


#ifdef __OMPT_TEST_ENUMERATE_STATE__
#include <stdio.h>
main()
{
  int state;
  const char *state_name;
  int ok;
  for (ok = ompt_enumerate_state(ompt_state_first, &state, &state_name);
       ok && state != ompt_state_last;
       ompt_enumerate_state(state, &state, &state_name)) {
    printf("state name = %s, id = %x\n", state_name, state);
  }
}

#endif



/*****************************************************************************
 * callbacks 
 ****************************************************************************/

_OMP_EXTERN int ompt_set_callback(ompt_event_t evid, ompt_callback_t cb)
{
  switch (evid) {

#define ompt_event(event_name, callback_type, event_id, is_impl) \
  case event_name: \
    if (is_impl) { \
      ompt_callbacks.ompt_callback(event_name) = (callback_type) cb; \
      return set_success; \
    } \
    return set_failure; 

#include "ompt-event.h"

  default: return set_failure;
  }
}


_OMP_EXTERN int ompt_get_callback(ompt_event_t evid, ompt_callback_t *cb)
{
  switch (evid) {

#define ompt_event(event_name, callback_type, event_id, is_impl) \
  case event_name:  \
    if (is_impl) { \
      ompt_callback_t mycb = \
        (ompt_callback_t) ompt_callbacks.ompt_callback(event_name); \
      if (mycb) { \
        *cb = mycb; \
         return get_success; \
      } \
    } \
    return get_failure; 

#include "ompt-event.h"

  default: return get_failure;
  }
}



/*****************************************************************************
 * intialization/finalization
 ****************************************************************************/

_OMP_EXTERN __attribute__ (( weak )) int ompt_initialize()
{
  return no_tool_present;
}


void ompt_init()
{
   char *ompt_env_var = getenv("OMPT_INITIALIZE");

   int ompt_env_var_is_true = ompt_env_var && !strcmp(ompt_env_var, "true");
   int ompt_env_var_is_disabled = ompt_env_var && !strcmp(ompt_env_var, "disabled");
   int ompt_env_var_is_false = ompt_env_var && !strcmp(ompt_env_var, "false");
   int ompt_env_var_is_null = ompt_env_var && !strcmp(ompt_env_var, "");

   if (!ompt_env_var || ompt_env_var_is_null) {
      int ompt_init_val = ompt_initialize();
      if (ompt_init_val) ompt_status = ompt_status_track_callback;
      // else remain in ready
   } else if (ompt_env_var_is_true) {
      int ompt_init_val = ompt_initialize();
      ompt_status = (ompt_init_val ? ompt_status_track_callback : ompt_status_track);
   } else if (ompt_env_var_is_false) {
      // no-op: remain in ready
   } else if (ompt_env_var_is_disabled) {
      ompt_status = ompt_status_disabled;
   } else {
      fprintf(stderr,"OMPT: warning: OMPT_INITIALIZE has invalid value.\n"
                     "      legal values are (NULL,\"\",\"true\",\"false\",\"disabled\").\n");
   }
}


void ompt_fini()
{
   ompt_status = ompt_status_disabled;

   if (ompt_callbacks.ompt_callback(ompt_event_runtime_shutdown)) {
     ompt_callbacks.ompt_callback(ompt_event_runtime_shutdown)();
   }
}



/*****************************************************************************
 * parallel regions
 ****************************************************************************/

_OMP_EXTERN ompt_parallel_id_t ompt_get_parallel_id(int ancestor_level)
{
   return __ompt_get_parallel_id_internal(ancestor_level);
}


_OMP_EXTERN void *ompt_get_parallel_function(int ancestor_level) 
{
   return __ompt_get_parallel_function_internal(ancestor_level);
}


_OMP_EXTERN ompt_state_t ompt_get_state(ompt_wait_id_t *ompt_wait_id)
{
   ompt_state_t thread_state = __ompt_get_state_internal(ompt_wait_id);

   if (thread_state == ompt_state_undefined) {
     thread_state = ompt_state_work_serial;
   }

   return thread_state;
}



/*****************************************************************************
 * threads
 ****************************************************************************/

_OMP_EXTERN ompt_data_t *ompt_get_thread_data()
{
   return __ompt_get_thread_data_internal();
}



/*****************************************************************************
 * tasks
 ****************************************************************************/

_OMP_EXTERN ompt_data_t *ompt_get_task_data(int ancestor_level)
{
  return __ompt_get_task_data_internal(ancestor_level); 
}


_OMP_EXTERN ompt_frame_t *ompt_get_task_frame(int ancestor_level)
{
  return __ompt_get_task_frame_internal(ancestor_level); 
}


_OMP_EXTERN void *ompt_get_task_function(int ancestor_level)
{
  return __ompt_get_task_function_internal(ancestor_level); 
}



/*****************************************************************************
 * compatability
 ****************************************************************************/

_OMP_EXTERN int ompt_get_ompt_version()
{
  return OMPT_VERSION;
}



/*****************************************************************************
 * application-facing API
 ****************************************************************************/


/*----------------------------------------------------------------------------
 | control
 ---------------------------------------------------------------------------*/

_OMP_EXTERN void ompt_control(uint64_t command, uint64_t modifier)
{
   if (ompt_callbacks.ompt_callback(ompt_event_control)) {
     ompt_callbacks.ompt_callback(ompt_event_control)(command, modifier);
   }
}

/*----------------------------------------------------------------------------
 | runtime version
 ---------------------------------------------------------------------------*/

_OMP_EXTERN int ompt_get_runtime_version(char *buffer, int length)
{
  return __ompt_get_runtime_version_internal(buffer,length);
}

