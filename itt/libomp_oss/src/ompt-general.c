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

// these return codes and the event macros still need some work
//
#define set_success 3
#define set_failure 0

#define get_success 1
#define get_failure 0

#define no_tool_present 0

#define OMPT_API_ROUTINE extern "C"



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

_OMP_EXTERN char **ompd_dll_locations;



/*****************************************************************************
 * forward declarations
 ****************************************************************************/

static ompt_interface_fn_t ompt_fn_lookup(const char *s);


/*****************************************************************************
 * state
 ****************************************************************************/

OMPT_API_ROUTINE int ompt_enumerate_state(int current_state, int *next_state, 
                                          const char **next_state_name)
{
  const static int len = sizeof(ompt_state_info) / sizeof(ompt_state_info_t);
  int i = 0;

  for (i = 0; i < len - 1; i++) {
    if (ompt_state_info[i].state_id == current_state) {
      *next_state = ompt_state_info[i+1].state_id;
      *next_state_name = ompt_state_info[i+1].state_name;
      return 1;
    }
  }

  return 0;
}



/*****************************************************************************
 * callbacks 
 ****************************************************************************/

OMPT_API_ROUTINE int ompt_set_callback(ompt_event_t evid, ompt_callback_t cb)
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


OMPT_API_ROUTINE int ompt_get_callback(ompt_event_t evid, ompt_callback_t *cb)
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

_OMP_EXTERN __attribute__ (( weak )) 
int ompt_initialize(ompt_function_lookup_t ompt_fn_lookup)
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

   ompd_dll_locations = (char **) malloc(sizeof(char **));
   ompd_dll_locations[0] = NULL;

   if (!ompt_env_var || ompt_env_var_is_null) {
      int ompt_init_val = ompt_initialize(ompt_fn_lookup);
      if (ompt_init_val) ompt_status = ompt_status_track_callback;
      // else remain in ready
   } else if (ompt_env_var_is_true) {
      int ompt_init_val = ompt_initialize(ompt_fn_lookup);
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
   if (ompt_status == ompt_status_track_callback) {
     if (ompt_callbacks.ompt_callback(ompt_event_runtime_shutdown)) {
       ompt_callbacks.ompt_callback(ompt_event_runtime_shutdown)();
     }
   }

   ompt_status = ompt_status_disabled;
}



/*****************************************************************************
 * parallel regions
 ****************************************************************************/

OMPT_API_ROUTINE ompt_parallel_id_t ompt_get_parallel_id(int ancestor_level)
{
   return __ompt_get_parallel_id_internal(ancestor_level);
}


OMPT_API_ROUTINE void *ompt_get_parallel_function(int ancestor_level) 
{
   return __ompt_get_parallel_function_internal(ancestor_level);
}


OMPT_API_ROUTINE ompt_state_t ompt_get_state(ompt_wait_id_t *ompt_wait_id)
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


OMPT_API_ROUTINE void *ompt_get_idle_frame()
{
   return __ompt_get_idle_frame_internal();
}



/*****************************************************************************
 * tasks
 ****************************************************************************/

OMPT_API_ROUTINE ompt_task_id_t ompt_get_task_id(int ancestor_level)
{
  return __ompt_get_task_id_internal(ancestor_level); 
}


OMPT_API_ROUTINE ompt_frame_t *ompt_get_task_frame(int ancestor_level)
{
  return __ompt_get_task_frame_internal(ancestor_level); 
}


OMPT_API_ROUTINE void *ompt_get_task_function(int ancestor_level)
{
  return __ompt_get_task_function_internal(ancestor_level); 
}



/*****************************************************************************
 * compatability
 ****************************************************************************/

OMPT_API_ROUTINE int ompt_get_ompt_version()
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
   if (ompt_status == ompt_status_track_callback) {
     if (ompt_callbacks.ompt_callback(ompt_event_control)) {
       ompt_callbacks.ompt_callback(ompt_event_control)(command, modifier);
     }
   }
}

/*----------------------------------------------------------------------------
 | runtime version
 ---------------------------------------------------------------------------*/

_OMP_EXTERN int ompt_get_runtime_version(char *buffer, int length)
{
  return __ompt_get_runtime_version_internal(buffer,length);
}



/*****************************************************************************
 * API inquiry for tool
 ****************************************************************************/

static ompt_interface_fn_t ompt_fn_lookup(const char *s) 
{
#define ompt_interface_fn(fn) \
  if (strcmp(s, #fn) == 0) return (ompt_interface_fn_t) fn; 
#include "ompt-fns.h"
  return (ompt_interface_fn_t) 0;
}
