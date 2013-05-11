#include <assert.h>

#include "ompt-internal.h"


typedef struct {
  char *state_name;
  ompt_state_t  state_id;
} ompt_state_info_t;


ompt_state_info_t ompt_state_info[] = {
#define ompt_state(state, code) { # state, state },
#include "ompt-state.h"
#undef ompt_state
};


_OMP_EXTERN int ompt_enumerate_state(int current_state, int *next_state, const char **next_state_name)
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
