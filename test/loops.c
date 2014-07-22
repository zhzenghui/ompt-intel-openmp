#include <omp.h>
#include <ompt.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>

#define OMPT_EVENT_DETAIL 0

#define OMPT_API_DECLARE(fn) fn ## _t fn

#define LOOKUP(lookup, fn) if(!(fn = (fn ## _t) lookup(#fn))){ printf("Error getting function %s!\n",#fn);return 0;}

OMPT_API_DECLARE(ompt_get_parallel_id);
OMPT_API_DECLARE(ompt_set_callback);
OMPT_API_DECLARE(ompt_get_thread_id);
OMPT_API_DECLARE(ompt_get_state);

typedef struct{
    char* type;
    double time;
}logEntry;

logEntry logFile[1024];
int curLog = 0;

double zeroTime = 0.;
double getTime(){
    struct timeval t;
    double ret;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000. + t.tv_usec / 1000. - zeroTime;
}

void log(char* type){
    int cur = curLog++;
    logFile[cur].type=type;
    logFile[cur].time=getTime();
}

void log2(char* t, ... ){
    char* l=(char*)malloc(50);
    va_list args;
    va_start(args, t);
    vsprintf(l, t, args);
    va_end(args);
    log(l);
}

void foo(){
    if(ompt_get_thread_id()==5) log("Foo begin");
    printf("foo");
    fflush(stdout);
    if(ompt_get_thread_id()==5) log("Foo end");
}

int main()
{
    int i;
    log("1. Loop");
    #pragma omp parallel for num_threads(16) ordered schedule(static,1)
    for(i=0; i<64; i++)
        foo();
    log("2. Loop");
    #pragma omp parallel for num_threads(16) ordered schedule(static,2)
    for(i=0; i<64; i++)
        foo();
    log("3. Loop");
    #pragma omp parallel for num_threads(16) schedule(static,1)
    for(i=0; i<64; i++)
        foo();
    log("4. Loop");
    #pragma omp parallel for num_threads(16) ordered schedule(dynamic)
    for(i=0; i<64; i++)
        foo();
    log("5. Loop");
    #pragma omp parallel for num_threads(16) schedule(dynamic)
    for(i=0; i<64; i++)
        foo();
    log("6. Loop");
    #pragma omp parallel for num_threads(16) ordered schedule(guided)
    for(i=0; i<64; i++)
        foo();
    log("7. Loop");
    #pragma omp parallel for num_threads(16) schedule(guided)
    for(i=0; i<64; i++)
        foo();
    return(0);
}


#define REGISTER(EVENT) \
if (ompt_set_callback(EVENT, (ompt_callback_t) EVENT ## _fn) == 0) { \
  fprintf(stderr,"Failed to register OMPT callback %s!\n", #EVENT); return 0; \
}

void ompt_event_thread_begin_fn(ompt_thread_type_t thread_type, ompt_thread_id_t id){
    if(id==5) log("Thread begin");
}
void ompt_event_thread_end_fn(ompt_thread_type_t thread_type, ompt_thread_id_t id){
    if(id==5) log("Thread end");
}

void ompt_event_parallel_begin_fn(ompt_task_id_t parent_task_id, ompt_frame_t *parent_task_frame,
        ompt_parallel_id_t parallel_id, void *parallel_function){
    log("Parallel begin");
}

void ompt_event_parallel_end_fn(ompt_task_id_t parent_task_id, ompt_frame_t *parent_task_frame,
        ompt_parallel_id_t parallel_id, void *parallel_function){
    log("Parallel end");
}

void ompt_event_barrier_begin_fn(ompt_parallel_id_t parallel_id, ompt_task_id_t task_id){
    if(ompt_get_thread_id()==5) log("Barrier begin");
}

void ompt_event_barrier_end_fn(ompt_parallel_id_t parallel_id, ompt_task_id_t task_id){
    if(ompt_get_thread_id()==5) log("Barrier end");
}

void ompt_event_wait_barrier_begin_fn(ompt_parallel_id_t parallel_id, ompt_task_id_t task_id){
    if(ompt_get_thread_id()==5) log("Wait barrier begin");
}

void ompt_event_wait_barrier_end_fn(ompt_parallel_id_t parallel_id, ompt_task_id_t task_id){
    if(ompt_get_thread_id()==5) log("Wait barrier end");
}

void ompt_event_idle_begin_fn(ompt_thread_id_t id){
    if(id==5) log("Idle begin");
}

void ompt_event_idle_end_fn(ompt_thread_id_t id){
    if(id==5) log("Idle end");
}

void ompt_event_loop_begin_fn(ompt_parallel_id_t parallel_id, ompt_task_id_t task_id, void *workshare_function){
    if(ompt_get_thread_id()==5) log("Loop begin");
}

void ompt_event_loop_end_fn(ompt_parallel_id_t parallel_id, ompt_task_id_t task_id, void *workshare_function){
    if(ompt_get_thread_id()==5) log("Loop end");
}

void ompt_event_runtime_shutdown_fn(){
    int i;
    for(i=0;i<curLog;i++)
        printf("%g: %s\n",logFile[i].time,logFile[i].type);
}

int ompt_initialize(ompt_function_lookup_t lookup, const char *runtime_version, int ompt_version) {
  printf("Init: %s ver %i\n",runtime_version,ompt_version);
  zeroTime = getTime();
  LOOKUP(lookup,ompt_get_parallel_id);
  LOOKUP(lookup,ompt_set_callback);
  LOOKUP(lookup,ompt_get_thread_id);
  LOOKUP(lookup,ompt_get_state);
  REGISTER(ompt_event_thread_begin);
  REGISTER(ompt_event_thread_end);
  REGISTER(ompt_event_parallel_begin);
  REGISTER(ompt_event_parallel_end);
  REGISTER(ompt_event_idle_begin);
  REGISTER(ompt_event_idle_end);
  REGISTER(ompt_event_barrier_begin);
  REGISTER(ompt_event_barrier_end);
  REGISTER(ompt_event_wait_barrier_begin);
  REGISTER(ompt_event_wait_barrier_end);
  REGISTER(ompt_event_loop_begin);
  REGISTER(ompt_event_loop_end);
  REGISTER(ompt_event_runtime_shutdown);
  return 1;
}

