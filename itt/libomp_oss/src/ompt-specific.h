#ifndef OMPT_SPECIFIC_H
#define OMPT_SPECIFIC_H

#include "kmp.h"

void __ompt_team_assign_id(kmp_team_t *team);
void __ompt_thread_assign_wait_id(void *variable);

/* the wait_id assignment is needed in many, many macros - so define
 * a wrapper macro which can be a noop when it is not wanted.
 */
#if OMPT_SUPPORT
#define OMPT_THREAD_ASSIGN_WAIT_ID(DATA) __ompt_thread_assign_wait_id(DATA);
#else
#define OMPT_THREAD_ASSIGN_WAIT_ID(DATA)
#endif

#endif
