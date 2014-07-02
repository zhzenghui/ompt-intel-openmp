/*
 * kmp_taskdeps.cpp
 * $Revision: 42539 $
 * $Date: 2013-07-17 11:20:01 -0500 (Wed, 17 Jul 2013) $
 */

/* <copyright>
    Copyright (c) 1997-2013 Intel Corporation.  All Rights Reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of Intel Corporation nor the names of its
        contributors may be used to endorse or promote products derived
        from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

</copyright> */

//#define KMP_SUPPORT_GRAPH_OUTPUT 1

#include "kmp.h"
#include "kmp_io.h"

#if OMP_40_ENABLED

//TODO: Improve memory allocation? keep a list of pre-allocated structures? allocate in blocks? re-use list finished list entries?
//TODO: don't use atomic ref counters for stack-allocated nodes.
//TODO: find an alternate to atomic refs for heap-allocated nodes?
//TODO: Finish graph output support
//TODO: kmp_lock_t seems a tad to big (and heavy weight) for this. Check other runtime locks
//TODO: Any ITT support needed?

#ifdef KMP_SUPPORT_GRAPH_OUTPUT
static kmp_int32 kmp_node_id_seed = 0;
#endif

static void
__kmp_init_node ( kmp_depnode_t *node )
{
    node->dn.task = NULL; // set to null initially, it will point to the right task once dependences have been processed
    node->dn.successors = NULL;
    __kmp_init_lock(&node->dn.lock);
    node->dn.nrefs = 1; // init creates the first reference to the node
#ifdef KMP_SUPPORT_GRAPH_OUTPUT
    node->dn.id = KMP_TEST_THEN_INC32(&kmp_node_id_seed);
#endif
}

static inline kmp_depnode_t *
__kmp_node_ref ( kmp_depnode_t *node )
{
    KMP_TEST_THEN_INC32(&node->dn.nrefs);
    return node;
}

static inline void
__kmp_node_deref ( kmp_info_t *thread, kmp_depnode_t *node )
{
    if (!node) return;

    kmp_int32 n = KMP_TEST_THEN_DEC32(&node->dn.nrefs) - 1;
    if ( n == 0 ) {
        KMP_ASSERT(node->dn.nrefs == 0);
#if USE_FAST_MEMORY
        __kmp_fast_free(thread,node);
#else
        __kmp_thread_free(thread,node);
#endif
    }
}

#define KMP_ACQUIRE_DEPNODE(gtid,n) __kmp_acquire_lock(&(n)->dn.lock,(gtid))
#define KMP_RELEASE_DEPNODE(gtid,n) __kmp_release_lock(&(n)->dn.lock,(gtid))

static void
__kmp_depnode_list_free ( kmp_info_t *thread, kmp_depnode_list *list );

static const kmp_int32 kmp_dephash_log2 = 6;
static const kmp_int32 kmp_dephash_size = (1 << kmp_dephash_log2);

static inline kmp_int32
__kmp_dephash_hash ( kmp_intptr_t addr )
{
    //TODO alternate to try: set = (((Addr64)(addrUsefulBits * 9.618)) % m_num_sets );
    return ((addr >> kmp_dephash_log2) ^ addr) % kmp_dephash_size;
}

static kmp_dephash_t *
__kmp_dephash_create ( kmp_info_t *thread )
{
    kmp_dephash_t *h;
	
    kmp_int32 size = kmp_dephash_size * sizeof(kmp_dephash_entry_t) + sizeof(kmp_dephash_t);
	 	
#if USE_FAST_MEMORY
    h = (kmp_dephash_t *) __kmp_fast_allocate( thread, size );
#else
    h = (kmp_dephash_t *) __kmp_thread_malloc( thread, size );
#endif

#ifdef KMP_DEBUG	
    h->nelements = 0;
#endif
    h->buckets = (kmp_dephash_entry **)(h+1);
	
    for ( kmp_int32 i = 0; i < kmp_dephash_size; i++ )
        h->buckets[i] = 0;

    return h;
}

static void
__kmp_dephash_free ( kmp_info_t *thread, kmp_dephash_t *h )
{
    for ( kmp_int32 i=0; i < kmp_dephash_size; i++ ) {
        if ( h->buckets[i] ) {
            kmp_dephash_entry_t *next;
            for ( kmp_dephash_entry_t *entry = h->buckets[i]; entry; entry = next ) {
                next = entry->next_in_bucket;
                __kmp_depnode_list_free(thread,entry->last_ins);
                __kmp_node_deref(thread,entry->last_out);
#if USE_FAST_MEMORY
                __kmp_fast_free(thread,entry);
#else
                __kmp_thread_free(thread,entry);
#endif
            }
        }
    }
#if USE_FAST_MEMORY
    __kmp_fast_free(thread,h);
#else
    __kmp_thread_free(thread,h);
#endif
}

static kmp_dephash_entry *
__kmp_dephash_find ( kmp_info_t *thread, kmp_dephash_t *h, kmp_intptr_t addr )
{
    kmp_int32 bucket = __kmp_dephash_hash(addr);
	
    kmp_dephash_entry_t *entry;
    for ( entry = h->buckets[bucket]; entry; entry = entry->next_in_bucket )
        if ( entry->addr == addr ) break;
	
    if ( entry == NULL ) {
        // create entry. This is only done by one thread so no locking required
#if USE_FAST_MEMORY
        entry = (kmp_dephash_entry_t *) __kmp_fast_allocate( thread, sizeof(kmp_dephash_entry_t) );
#else
        entry = (kmp_dephash_entry_t *) __kmp_thread_malloc( thread, sizeof(kmp_dephash_entry_t) );
#endif
        entry->addr = addr;
        entry->last_out = NULL;
        entry->last_ins = NULL;
        entry->next_in_bucket = h->buckets[bucket];
        h->buckets[bucket] = entry;
#ifdef KMP_DEBUG
        h->nelements++;
        if ( entry->next_in_bucket ) h->nconflicts++;
#endif
    }
    return entry;
}

static kmp_depnode_list_t *
__kmp_add_node ( kmp_info_t *thread, kmp_depnode_list_t *list, kmp_depnode_t *node )
{
    kmp_depnode_list_t *new_head;

#if USE_FAST_MEMORY
    new_head = (kmp_depnode_list_t *) __kmp_fast_allocate(thread,sizeof(kmp_depnode_list_t));
#else
    new_head = (kmp_depnode_list_t *) __kmp_thread_malloc(thread,sizeof(kmp_depnode_list_t));
#endif

    new_head->node = __kmp_node_ref(node);
    new_head->next = list;

    return new_head;
}

static void
__kmp_depnode_list_free ( kmp_info_t *thread, kmp_depnode_list *list )
{
    kmp_depnode_list *next;

    for ( ; list ; list = next ) {
        next = list->next;

        __kmp_node_deref(thread,list->node);
#if USE_FAST_MEMORY
        __kmp_fast_free(thread,list);
#else
        __kmp_thread_free(thread,list);
#endif
    }
}

static inline void
__kmp_track_dependence ( kmp_depnode_t *source, kmp_depnode_t *sink )
{
#ifdef KMP_SUPPORT_GRAPH_OUTPUT
    kmp_taskdata_t * task_source = KMP_TASK_TO_TASKDATA(source->dn.task);
    kmp_taskdata_t * task_sink = KMP_TASK_TO_TASKDATA(sink->dn.task);    // this can be NULL when if(0) ...

    __kmp_printf("%d(%s) -> %d(%s)\n", source->dn.id, task_source->td_ident->psource, sink->dn.id, task_sink->td_ident->psource);
#endif
}

template< bool filter >
static inline kmp_int32
__kmp_process_deps ( kmp_int32 gtid, kmp_depnode_t *node, kmp_dephash_t *hash,
                     bool dep_barrier,kmp_int32 ndeps, kmp_depend_info_t *dep_list)
{
    kmp_info_t *thread = __kmp_threads[ gtid ];
    kmp_int32 npredecessors=0;
    for ( kmp_int32 i = 0; i < ndeps ; i++ ) {
        const kmp_depend_info_t * dep = &dep_list[i];

        KMP_DEBUG_ASSERT(dep->flags.in);

        if ( filter && dep->base_addr == 0 ) continue; // skip filtered entries

        kmp_dephash_entry_t *info = __kmp_dephash_find(thread,hash,dep->base_addr);
        kmp_depnode_t *last_out = info->last_out;

        if ( dep->flags.out && info->last_ins ) {
            for ( kmp_depnode_list_t * p = info->last_ins; p; p = p->next ) {
                kmp_depnode_t * indep = p->node;
                if ( indep->dn.task ) {
                    KMP_ACQUIRE_DEPNODE(gtid,indep);
                    if ( indep->dn.task ) {
                        __kmp_track_dependence(indep,node);
                        indep->dn.successors = __kmp_add_node(thread, indep->dn.successors, node);
                        npredecessors++;
                    }
                    KMP_RELEASE_DEPNODE(gtid,indep);
                }
            }

            __kmp_depnode_list_free(thread,info->last_ins);
            info->last_ins = NULL;

        } else if ( last_out && last_out->dn.task ) {
            KMP_ACQUIRE_DEPNODE(gtid,last_out);
            if ( last_out->dn.task ) {
                __kmp_track_dependence(last_out,node);
                last_out->dn.successors = __kmp_add_node(thread, last_out->dn.successors, node);
                npredecessors++;
            }
            KMP_RELEASE_DEPNODE(gtid,last_out);
        }

        if ( dep_barrier ) {
            // if this is a sync point in the serial sequence and previous outputs are guaranteed to be completed after
            // the execution of this task so the previous output nodes can be cleared.
            __kmp_node_deref(thread,last_out);
            info->last_out = NULL;
        } else {
            if ( dep->flags.out ) {
                __kmp_node_deref(thread,last_out);
                info->last_out = __kmp_node_ref(node);
            } else
                info->last_ins = __kmp_add_node(thread, info->last_ins, node);
        }

    }
    return npredecessors;
}

#define NO_DEP_BARRIER (false)
#define DEP_BARRIER (true)

// returns true if the task has any outstanding dependence
static bool
__kmp_check_deps ( kmp_int32 gtid, kmp_depnode_t *node, kmp_task_t *task, kmp_dephash_t *hash, bool dep_barrier,
                   kmp_int32 ndeps, kmp_depend_info_t *dep_list,
                   kmp_int32 ndeps_noalias, kmp_depend_info_t *noalias_dep_list )
{
    int i;
   	
    // Filter deps in dep_list
    // TODO: Different algorithm for large dep_list ( > 10 ? )
    for ( i = 0; i < ndeps; i ++ ) {
        if ( dep_list[i].base_addr != 0 )
            for ( int j = i+1; j < ndeps; j++ )
                if ( dep_list[i].base_addr == dep_list[j].base_addr ) {
                    dep_list[i].flags.in |= dep_list[j].flags.in;
                    dep_list[i].flags.out |= dep_list[j].flags.out;
                    dep_list[j].base_addr = 0; // Mark j element as void
                }
    }

    // doesn't need to be atomic as no other thread is going to be accessing this node just yet
    // npredecessors is set 1 to ensure that none of the releasing tasks queues this task before we have finished processing all the dependencies
    node->dn.npredecessors = 1;

    // used to pack all npredecessors additions into a single atomic operation at the end
    int npredecessors;

    npredecessors = __kmp_process_deps<true>(gtid, node, hash, dep_barrier, ndeps, dep_list);
    npredecessors += __kmp_process_deps<false>(gtid, node, hash, dep_barrier, ndeps_noalias, noalias_dep_list);

    KMP_TEST_THEN_ADD32(&node->dn.npredecessors, npredecessors);

    // Remove the fake predecessor and find out if there's any outstanding dependence (some tasks may have finished while we processed the dependences)
    node->dn.task = task;
    KMP_MB();
    npredecessors = KMP_TEST_THEN_DEC32(&node->dn.npredecessors) - 1;

    // beyond this point the task could be queued (and executed) by a releasing task...
    return npredecessors > 0 ? true : false;
}

void
__kmp_release_deps ( kmp_int32 gtid, kmp_taskdata_t *task )
{
    kmp_info_t *thread = __kmp_threads[ gtid ];
    kmp_depnode_t *node = task->td_depnode;

    if ( task->td_dephash )
        __kmp_dephash_free(thread,task->td_dephash);

    if ( !node ) return;

    KMP_ACQUIRE_DEPNODE(gtid,node);
    node->dn.task = NULL; // mark this task as finished, so no new dependencies are generated
    KMP_RELEASE_DEPNODE(gtid,node);

    kmp_depnode_list_t *next;
    for ( kmp_depnode_list_t *p = node->dn.successors; p; p = next ) {
        kmp_depnode_t *successor = p->node;
        kmp_int32 npredecessors = KMP_TEST_THEN_DEC32(&successor->dn.npredecessors) - 1;

        // successor task can be NULL for wait_depends or because deps are still being processed
        if ( npredecessors == 0 ) {
            KMP_MB();
            if ( successor->dn.task )
            // loc_ref was already stored in successor's task_data
                __kmpc_omp_task(NULL,gtid,successor->dn.task);
        }

        next = p->next;
        __kmp_node_deref(thread,p->node);
#if USE_FAST_MEMORY
        __kmp_fast_free(thread,p);
#else
        __kmp_thread_free(thread,p);
#endif
    }

    __kmp_node_deref(thread,node);
}

/*!
@ingroup TASKING
@param loc_ref location of the original task directive
@param gtid Global Thread ID of encountering thread
@param new_task task thunk allocated by __kmp_omp_task_alloc() for the ''new task''
@param ndeps Number of depend items with possible aliasing
@param dep_list List of depend items with possible aliasing
@param ndeps_noalias Number of depend items with no aliasing
@param noalias_dep_list List of depend items with no aliasing

@return Returns either TASK_CURRENT_NOT_QUEUED if the current task was not suspendend and queued, or TASK_CURRENT_QUEUED if it was suspended and queued

Schedule a non-thread-switchable task with dependences for execution
*/
kmp_int32
__kmpc_omp_task_with_deps( ident_t *loc_ref, kmp_int32 gtid, kmp_task_t * new_task,
                                 kmp_int32 ndeps, kmp_depend_info_t *dep_list,
				 kmp_int32 ndeps_noalias, kmp_depend_info_t *noalias_dep_list )
{
    kmp_info_t *thread = __kmp_threads[ gtid ];
    kmp_taskdata_t * current_task = thread->th.th_current_task;

    bool serial = current_task->td_flags.team_serial || current_task->td_flags.tasking_ser || current_task->td_flags.final;

    if ( !serial && ( ndeps > 0 || ndeps_noalias > 0 )) {	   		
        /* if no dependencies have been tracked yet, create the dependence hash */
        if ( current_task->td_dephash == NULL )
            current_task->td_dephash = __kmp_dephash_create(thread);

#if USE_FAST_MEMORY
        kmp_depnode_t *node = (kmp_depnode_t *) __kmp_fast_allocate(thread,sizeof(kmp_depnode_t));
#else
        kmp_depnode_t *node = (kmp_depnode_t *) __kmp_thread_malloc(thread,sizeof(kmp_depnode_t));
#endif

        __kmp_init_node(node);
        KMP_TASK_TO_TASKDATA(new_task)->td_depnode = node;

        if ( __kmp_check_deps( gtid, node, new_task, current_task->td_dephash, NO_DEP_BARRIER,
                               ndeps, dep_list, ndeps_noalias,noalias_dep_list ) )
            return TASK_CURRENT_NOT_QUEUED;
    }

    return __kmpc_omp_task(loc_ref,gtid,new_task);
}

/*!
@ingroup TASKING
@param loc_ref location of the original task directive
@param gtid Global Thread ID of encountering thread
@param ndeps Number of depend items with possible aliasing
@param dep_list List of depend items with possible aliasing
@param ndeps_noalias Number of depend items with no aliasing
@param noalias_dep_list List of depend items with no aliasing

Blocks the current task until all specifies dependencies have been fulfilled.
*/
void
__kmpc_omp_wait_deps ( ident_t *loc_ref, kmp_int32 gtid, kmp_int32 ndeps, kmp_depend_info_t *dep_list,
                       kmp_int32 ndeps_noalias, kmp_depend_info_t *noalias_dep_list )
{
    if ( ndeps == 0 && ndeps_noalias == 0 ) return;

    kmp_info_t *thread = __kmp_threads[ gtid ];
    kmp_taskdata_t * current_task = thread->th.th_current_task;

    // dependences are not computed in serial teams
    if ( current_task->td_flags.team_serial || current_task->td_flags.tasking_ser || current_task->td_flags.final)
        return;

    // if the dephash is not yet created it means we have nothing to wait for
    if ( current_task->td_dephash == NULL ) return;

    kmp_depnode_t node;
    __kmp_init_node(&node);

    if (!__kmp_check_deps( gtid, &node, NULL, current_task->td_dephash, DEP_BARRIER,
                           ndeps, dep_list, ndeps_noalias, noalias_dep_list ))
        return;

    int thread_finished = FALSE;
    while ( node.dn.npredecessors > 0 ) {
        __kmp_execute_tasks( thread, gtid, (volatile kmp_uint32 *)&(node.dn.npredecessors),
                             0, FALSE, &thread_finished,
#if USE_ITT_BUILD
                             NULL,
#endif
                             __kmp_task_stealing_constraint );
    }

}

#endif /* OMP_40_ENABLED */
