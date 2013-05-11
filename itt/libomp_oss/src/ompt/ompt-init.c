#include <stdlib.h>
#include <stdio.h>

#include "ompt-internal.h"

ompt_status_t ompt_status = ompt_status_ready;

_OMP_EXTERN void ompt_init()
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
