/*
 * kmp_ftn_entry.h -- Fortran entry linkage support for OpenMP.
 * $Revision: 42221 $
 * $Date: 2013-04-01 07:06:15 -0500 (Mon, 01 Apr 2013) $
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


------------------------------------------------------------------------

    Portions of this software are protected under the following patents:
        U.S. Patent 5,812,852
        U.S. Patent 6,792,599
        U.S. Patent 7,069,556
        U.S. Patent 7,328,433
        U.S. Patent 7,500,242

</copyright> */

#ifndef FTN_STDCALL
# error The support file kmp_ftn_entry.h should not be compiled by itself.
#endif

#ifdef KMP_STUB
    #include "kmp_stub.h"
#endif

#include "kmp_i18n.h"

#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

/*
 * For compatiblity with the Gnu/MS Open MP codegen, omp_set_num_threads(),
 * omp_set_nested(), and omp_set_dynamic() [in lowercase on MS, and w/o
 * a trailing underscore on Linux* OS] take call by value integer arguments.
 * + omp_set_max_active_levels()
 * + omp_set_schedule()
 *
 * For backward compatiblity with 9.1 and previous Intel compiler, these
 * entry points take call by reference integer arguments.
 */
#ifdef KMP_GOMP_COMPAT
# if (KMP_FTN_ENTRIES == KMP_FTN_PLAIN) || (KMP_FTN_ENTRIES == KMP_FTN_UPPER)
#  define PASS_ARGS_BY_VALUE 1
# endif
#endif
#if KMP_OS_WINDOWS
# if (KMP_FTN_ENTRIES == KMP_FTN_PLAIN) || (KMP_FTN_ENTRIES == KMP_FTN_APPEND)
#  define PASS_ARGS_BY_VALUE 1
# endif
#endif

// This macro helps to reduce code duplication.
#ifdef PASS_ARGS_BY_VALUE
    #define KMP_DEREF
#else
    #define KMP_DEREF *
#endif

void  FTN_STDCALL
FTN_SET_STACKSIZE( int KMP_DEREF arg )
{
    #ifdef KMP_STUB
        __kmps_set_stacksize( KMP_DEREF arg );
    #else
        // __kmp_aux_set_stacksize initializes the library if needed
        __kmp_aux_set_stacksize( (size_t) KMP_DEREF arg );
    #endif
}

void  FTN_STDCALL
FTN_SET_STACKSIZE_S( size_t KMP_DEREF arg )
{
    #ifdef KMP_STUB
        __kmps_set_stacksize( KMP_DEREF arg );
    #else
        // __kmp_aux_set_stacksize initializes the library if needed
        __kmp_aux_set_stacksize( KMP_DEREF arg );
    #endif
}

int FTN_STDCALL
FTN_GET_STACKSIZE( void )
{
    #ifdef KMP_STUB
        return __kmps_get_stacksize();
    #else
        if ( ! __kmp_init_serial ) {
            __kmp_serial_initialize();
        };
        return (int)__kmp_stksize;
    #endif
}

size_t FTN_STDCALL
FTN_GET_STACKSIZE_S( void )
{
    #ifdef KMP_STUB
        return __kmps_get_stacksize();
    #else
        if ( ! __kmp_init_serial ) {
            __kmp_serial_initialize();
        };
        return __kmp_stksize;
    #endif
}

void FTN_STDCALL
FTN_SET_BLOCKTIME( int KMP_DEREF arg )
{
    #ifdef KMP_STUB
        __kmps_set_blocktime( KMP_DEREF arg );
    #else
	int gtid, tid;
	kmp_info_t *thread;

	gtid = __kmp_entry_gtid();
	tid = __kmp_tid_from_gtid(gtid);
	thread = __kmp_thread_from_gtid(gtid);

        __kmp_aux_set_blocktime( KMP_DEREF arg, thread, tid );
    #endif
}

int FTN_STDCALL
FTN_GET_BLOCKTIME( void )
{
    #ifdef KMP_STUB
        return __kmps_get_blocktime();
    #else
	int gtid, tid;
	kmp_info_t *thread;
        kmp_team_p *team;

	gtid = __kmp_entry_gtid();
	tid = __kmp_tid_from_gtid(gtid);
	thread = __kmp_thread_from_gtid(gtid);
        team = __kmp_threads[ gtid ] -> th.th_team;

        /* These must match the settings used in __kmp_wait_sleep() */
        if ( __kmp_dflt_blocktime == KMP_MAX_BLOCKTIME ) {
	    KF_TRACE(10, ( "kmp_get_blocktime: T#%d(%d:%d), blocktime=%d\n",
			  gtid, team->t.t_id, tid, KMP_MAX_BLOCKTIME) );
            return KMP_MAX_BLOCKTIME;
        }
#ifdef KMP_ADJUST_BLOCKTIME
        else if ( __kmp_zero_bt && !get__bt_set( team, tid ) ) {
	    KF_TRACE(10, ( "kmp_get_blocktime: T#%d(%d:%d), blocktime=%d\n",
			  gtid, team->t.t_id, tid, 0) );
            return 0;
        }
#endif /* KMP_ADJUST_BLOCKTIME */
        else {
	    KF_TRACE(10, ( "kmp_get_blocktime: T#%d(%d:%d), blocktime=%d\n",
              gtid, team->t.t_id, tid, get__blocktime( team, tid ) ) );
            return get__blocktime( team, tid );
        };
    #endif
}

void FTN_STDCALL
FTN_SET_LIBRARY_SERIAL( void )
{
    #ifdef KMP_STUB
        __kmps_set_library( library_serial );
    #else
        // __kmp_user_set_library initializes the library if needed
        __kmp_user_set_library( library_serial );
    #endif
}

void FTN_STDCALL
FTN_SET_LIBRARY_TURNAROUND( void )
{
    #ifdef KMP_STUB
        __kmps_set_library( library_turnaround );
    #else
        // __kmp_user_set_library initializes the library if needed
        __kmp_user_set_library( library_turnaround );
    #endif
}

void FTN_STDCALL
FTN_SET_LIBRARY_THROUGHPUT( void )
{
    #ifdef KMP_STUB
        __kmps_set_library( library_throughput );
    #else
        // __kmp_user_set_library initializes the library if needed
        __kmp_user_set_library( library_throughput );
    #endif
}

void FTN_STDCALL
FTN_SET_LIBRARY( int KMP_DEREF arg )
{
    #ifdef KMP_STUB
        __kmps_set_library( KMP_DEREF arg );
    #else
        enum library_type lib;
        lib = (enum library_type) KMP_DEREF arg;
        // __kmp_user_set_library initializes the library if needed
        __kmp_user_set_library( lib );
    #endif
}

int FTN_STDCALL
FTN_GET_LIBRARY (void)
{
    #ifdef KMP_STUB
        return __kmps_get_library();
    #else
        if ( ! __kmp_init_serial ) {
            __kmp_serial_initialize();
        }
        return ((int) __kmp_library);
    #endif
}

#if OMP_30_ENABLED

int FTN_STDCALL
FTN_SET_AFFINITY( void **mask )
{
    #if defined(KMP_STUB) || !(KMP_OS_WINDOWS || KMP_OS_LINUX)
        return -1;
    #else
        if ( ! TCR_4(__kmp_init_middle) ) {
            __kmp_middle_initialize();
        }
        return __kmp_aux_set_affinity( mask );
    #endif
}

int FTN_STDCALL
FTN_GET_AFFINITY( void **mask )
{
    #if defined(KMP_STUB) || !(KMP_OS_WINDOWS || KMP_OS_LINUX)
        return -1;
    #else
        if ( ! TCR_4(__kmp_init_middle) ) {
            __kmp_middle_initialize();
        }
        return __kmp_aux_get_affinity( mask );
    #endif
}

int FTN_STDCALL
FTN_GET_AFFINITY_MAX_PROC( void )
{
    #if defined(KMP_STUB) || !(KMP_OS_WINDOWS || KMP_OS_LINUX)
        return 0;
    #else
        //
        // We really only NEED serial initialization here.
        //
        if ( ! TCR_4(__kmp_init_middle) ) {
            __kmp_middle_initialize();
        }
        if ( ! ( KMP_AFFINITY_CAPABLE() ) ) {
            return 0;
        }

    #if KMP_OS_WINDOWS && KMP_ARCH_X86_64
        if ( __kmp_num_proc_groups <= 1 ) {
            return KMP_CPU_SETSIZE;
        }
    #endif /* KMP_OS_WINDOWS && KMP_ARCH_X86_64 */
        return __kmp_xproc;
    #endif
}

void FTN_STDCALL
FTN_CREATE_AFFINITY_MASK( void **mask )
{
    #if defined(KMP_STUB) || !(KMP_OS_WINDOWS || KMP_OS_LINUX)
        *mask = NULL;
    #else
        //
        // We really only NEED serial initialization here.
        //
        if ( ! TCR_4(__kmp_init_middle) ) {
            __kmp_middle_initialize();
        }
        *mask = kmpc_malloc( __kmp_affin_mask_size );
        KMP_CPU_ZERO( (kmp_affin_mask_t *)(*mask) );
    #endif
}

void FTN_STDCALL
FTN_DESTROY_AFFINITY_MASK( void **mask )
{
    #if defined(KMP_STUB) || !(KMP_OS_WINDOWS || KMP_OS_LINUX)
        // Nothing
    #else
        //
        // We really only NEED serial initialization here.
        //
        if ( ! TCR_4(__kmp_init_middle) ) {
            __kmp_middle_initialize();
        }
        if ( __kmp_env_consistency_check ) {
            if ( *mask == NULL ) {
	        KMP_FATAL( AffinityInvalidMask, "kmp_destroy_affinity_mask" );
	    }
        }
        kmpc_free( *mask );
        *mask = NULL;
    #endif
}

int FTN_STDCALL
FTN_SET_AFFINITY_MASK_PROC( int KMP_DEREF proc, void **mask )
{
    #if defined(KMP_STUB) || !(KMP_OS_WINDOWS || KMP_OS_LINUX)
        return -1;
    #else
        if ( ! TCR_4(__kmp_init_middle) ) {
            __kmp_middle_initialize();
        }
        return __kmp_aux_set_affinity_mask_proc( KMP_DEREF proc, mask );
    #endif
}

int FTN_STDCALL
FTN_UNSET_AFFINITY_MASK_PROC( int KMP_DEREF proc, void **mask )
{
    #if defined(KMP_STUB) || !(KMP_OS_WINDOWS || KMP_OS_LINUX)
        return -1;
    #else
        if ( ! TCR_4(__kmp_init_middle) ) {
            __kmp_middle_initialize();
        }
        return __kmp_aux_unset_affinity_mask_proc( KMP_DEREF proc, mask );
    #endif
}

int FTN_STDCALL
FTN_GET_AFFINITY_MASK_PROC( int KMP_DEREF proc, void **mask )
{
    #if defined(KMP_STUB) || !(KMP_OS_WINDOWS || KMP_OS_LINUX)
        return -1;
    #else
        if ( ! TCR_4(__kmp_init_middle) ) {
            __kmp_middle_initialize();
        }
        return __kmp_aux_get_affinity_mask_proc( KMP_DEREF proc, mask );
    #endif
}

#endif /* OMP_30_ENABLED */


/* ------------------------------------------------------------------------ */

/* sets the requested number of threads for the next parallel region */

void FTN_STDCALL
FTN_SET_NUM_THREADS( int KMP_DEREF arg )
{
    #ifdef KMP_STUB
        // Nothing.
    #else
        __kmp_set_num_threads( KMP_DEREF arg, __kmp_entry_gtid() );
    #endif
}


/* returns the number of threads in current team */
int FTN_STDCALL
FTN_GET_NUM_THREADS( void )
{
    #ifdef KMP_STUB
        return 1;
    #else
        // __kmpc_bound_num_threads initializes the library if needed
        return __kmpc_bound_num_threads(NULL);
    #endif
}

int FTN_STDCALL
FTN_GET_MAX_THREADS( void )
{
    #ifdef KMP_STUB
        return 1;
    #else
        int         gtid;
        kmp_info_t *thread;
        if ( ! TCR_4(__kmp_init_middle) ) {
            __kmp_middle_initialize();
        }
        gtid   = __kmp_entry_gtid();
        thread = __kmp_threads[ gtid ];
        #if OMP_30_ENABLED
        //return thread -> th.th_team -> t.t_current_task[ thread->th.th_info.ds.ds_tid ] -> icvs.nproc;
	return thread -> th.th_current_task -> td_icvs.nproc;
        #else
        return thread -> th.th_team -> t.t_set_nproc[ thread->th.th_info.ds.ds_tid ];
        #endif
    #endif
}

int FTN_STDCALL
FTN_GET_THREAD_NUM( void )
{
    #ifdef KMP_STUB
        return 0;
    #else
        int gtid;

        #if KMP_OS_DARWIN
            gtid = __kmp_entry_gtid();
        #elif KMP_OS_WINDOWS
            if (!__kmp_init_parallel ||
                (gtid = ((kmp_intptr_t)TlsGetValue( __kmp_gtid_threadprivate_key ))) == 0) {
                // Either library isn't initialized or thread is not registered
                // 0 is the correct TID in this case
                return 0;
            }
            --gtid; // We keep (gtid+1) in TLS
        #elif KMP_OS_LINUX
            #ifdef KMP_TDATA_GTID
            if ( __kmp_gtid_mode >= 3 ) {
                if ((gtid = __kmp_gtid) == KMP_GTID_DNE) {
                    return 0;
                }
            } else {
            #endif
                if (!__kmp_init_parallel ||
                    (gtid = (kmp_intptr_t)(pthread_getspecific( __kmp_gtid_threadprivate_key ))) == 0) {
                    return 0;
                }
                --gtid;
            #ifdef KMP_TDATA_GTID
            }
            #endif
        #else
            #error Unknown or unsupported OS
        #endif

        return __kmp_tid_from_gtid( gtid );
    #endif
}

int FTN_STDCALL
FTN_GET_NUM_KNOWN_THREADS( void )
{
    #ifdef KMP_STUB
        return 1;
    #else
        if ( ! __kmp_init_serial ) {
            __kmp_serial_initialize();
        }
        /* NOTE: this is not syncronized, so it can change at any moment */
        /* NOTE: this number also includes threads preallocated in hot-teams */
        return TCR_4(__kmp_nth);
    #endif
}

int FTN_STDCALL
FTN_GET_NUM_PROCS( void )
{
    #ifdef KMP_STUB
        return 1;
    #else
        int gtid;
        if ( ! TCR_4(__kmp_init_middle) ) {
            __kmp_middle_initialize();
        }
        return __kmp_avail_proc;
    #endif
}

void FTN_STDCALL
FTN_SET_NESTED( int KMP_DEREF flag )
{
    #ifdef KMP_STUB
        __kmps_set_nested( KMP_DEREF flag );
    #else
        kmp_info_t *thread;
        /* For the thread-private internal controls implementation */
        thread = __kmp_entry_thread();
        __kmp_save_internal_controls( thread );
        set__nested( thread, ( (KMP_DEREF flag) ? TRUE : FALSE ) );
    #endif
}


int FTN_STDCALL
FTN_GET_NESTED( void )
{
    #ifdef KMP_STUB
        return __kmps_get_nested();
    #else
        kmp_info_t *thread;
        thread = __kmp_entry_thread();
        return get__nested( thread );
    #endif
}

void FTN_STDCALL
FTN_SET_DYNAMIC( int KMP_DEREF flag )
{
    #ifdef KMP_STUB
        __kmps_set_dynamic( KMP_DEREF flag ? TRUE : FALSE );
    #else
        kmp_info_t *thread;
        /* For the thread-private implementation of the internal controls */
        thread = __kmp_entry_thread();
        // !!! What if foreign thread calls it?
        __kmp_save_internal_controls( thread );
        set__dynamic( thread, KMP_DEREF flag ? TRUE : FALSE );
    #endif
}


int FTN_STDCALL
FTN_GET_DYNAMIC( void )
{
    #ifdef KMP_STUB
        return __kmps_get_dynamic();
    #else
        kmp_info_t *thread;
        thread = __kmp_entry_thread();
        return get__dynamic( thread );
    #endif
}

int FTN_STDCALL
FTN_IN_PARALLEL( void )
{
    #ifdef KMP_STUB
        return 0;
    #else
        return ( __kmp_entry_thread() -> th.th_root ->
                    r.r_in_parallel ? FTN_TRUE : FTN_FALSE );
    #endif
}

#if OMP_30_ENABLED

void FTN_STDCALL
FTN_SET_SCHEDULE( kmp_sched_t KMP_DEREF kind, int KMP_DEREF modifier )
{
    #ifdef KMP_STUB
        __kmps_set_schedule( KMP_DEREF kind, KMP_DEREF modifier );
    #else
	/*  TO DO  */
        /* For the per-task implementation of the internal controls */
        __kmp_set_schedule( __kmp_entry_gtid(), KMP_DEREF kind, KMP_DEREF modifier );
    #endif
}

void FTN_STDCALL
FTN_GET_SCHEDULE( kmp_sched_t * kind, int * modifier )
{
    #ifdef KMP_STUB
        __kmps_get_schedule( kind, modifier );
    #else
	/*  TO DO  */
	/* For the per-task implementation of the internal controls */
        __kmp_get_schedule( __kmp_entry_gtid(), kind, modifier );
    #endif
}

void FTN_STDCALL
FTN_SET_MAX_ACTIVE_LEVELS( int KMP_DEREF arg )
{
    #ifdef KMP_STUB
	// Nothing.
    #else
	/*  TO DO  */
        /* We want per-task implementation of this internal control */
        __kmp_set_max_active_levels( __kmp_entry_gtid(), KMP_DEREF arg );
    #endif
}

int FTN_STDCALL
FTN_GET_MAX_ACTIVE_LEVELS( void )
{
    #ifdef KMP_STUB
	return 0;
    #else
	/*  TO DO  */
	/* We want per-task implementation of this internal control */
	return __kmp_get_max_active_levels( __kmp_entry_gtid() );
    #endif
}

int FTN_STDCALL
FTN_GET_ACTIVE_LEVEL( void )
{
    #ifdef KMP_STUB
	return 0; // returns 0 if it is called from the sequential part of the program
    #else
	/*  TO DO  */
	/* For the per-task implementation of the internal controls */
        return __kmp_entry_thread() -> th.th_team -> t.t_active_level;
    #endif
}

int FTN_STDCALL
FTN_GET_LEVEL( void )
{
    #ifdef KMP_STUB
	return 0; // returns 0 if it is called from the sequential part of the program
    #else
	/*  TO DO  */
	/* For the per-task implementation of the internal controls */
        return __kmp_entry_thread() -> th.th_team -> t.t_level;
    #endif
}

int FTN_STDCALL
FTN_GET_ANCESTOR_THREAD_NUM( int KMP_DEREF level )
{
    #ifdef KMP_STUB
	return ( KMP_DEREF level ) ? ( -1 ) : ( 0 );
    #else
	return __kmp_get_ancestor_thread_num( __kmp_entry_gtid(), KMP_DEREF level );
    #endif
}

int FTN_STDCALL
FTN_GET_TEAM_SIZE( int KMP_DEREF level )
{
    #ifdef KMP_STUB
        return ( KMP_DEREF level ) ? ( -1 ) : ( 1 );
    #else
        return __kmp_get_team_size( __kmp_entry_gtid(), KMP_DEREF level );
    #endif
}

int FTN_STDCALL
FTN_GET_THREAD_LIMIT( void )
{
    #ifdef KMP_STUB
	return 1;   // TO DO: clarify whether it returns 1 or 0?
    #else
        if ( ! __kmp_init_serial ) {
            __kmp_serial_initialize();
        };
        /* global ICV */
	return __kmp_max_nth;
    #endif
}

int FTN_STDCALL
FTN_IN_FINAL( void )
{
    #ifdef KMP_STUB
	return 0;   // TO DO: clarify whether it returns 1 or 0?
    #else
        if ( ! TCR_4(__kmp_init_parallel) ) {
            return 0;
        }
	return __kmp_entry_thread() -> th.th_current_task -> td_flags.final;
    #endif
}

#endif // OMP_30_ENABLED

#if OMP_40_ENABLED


kmp_proc_bind_t FTN_STDCALL
FTN_GET_PROC_BIND( void )
{
    #ifdef KMP_STUB
        return __kmps_get_proc_bind();
    #else
        return get__proc_bind( __kmp_entry_thread() );
    #endif
}

#endif // OMP_40_ENABLED

#ifdef KMP_STUB
typedef enum { UNINIT = -1, UNLOCKED, LOCKED } kmp_stub_lock_t;
#endif /* KMP_STUB */

/* initialize the lock */
void FTN_STDCALL
FTN_INIT_LOCK( void **user_lock )
{
    #ifdef KMP_STUB
        *((kmp_stub_lock_t *)user_lock) = UNLOCKED;
    #else
        __kmpc_init_lock( NULL, __kmp_entry_gtid(), user_lock );
    #endif
}

/* initialize the lock */
void FTN_STDCALL
FTN_INIT_NEST_LOCK( void **user_lock )
{
    #ifdef KMP_STUB
        *((kmp_stub_lock_t *)user_lock) = UNLOCKED;
    #else
        __kmpc_init_nest_lock( NULL, __kmp_entry_gtid(), user_lock );
    #endif
}

void FTN_STDCALL
FTN_DESTROY_LOCK( void **user_lock )
{
    #ifdef KMP_STUB
        *((kmp_stub_lock_t *)user_lock) = UNINIT;
    #else
        __kmpc_destroy_lock( NULL, __kmp_entry_gtid(), user_lock );
    #endif
}

void FTN_STDCALL
FTN_DESTROY_NEST_LOCK( void **user_lock )
{
    #ifdef KMP_STUB
        *((kmp_stub_lock_t *)user_lock) = UNINIT;
    #else
        __kmpc_destroy_nest_lock( NULL, __kmp_entry_gtid(), user_lock );
    #endif
}

void FTN_STDCALL
FTN_SET_LOCK( void **user_lock )
{
    #ifdef KMP_STUB
        if ( *((kmp_stub_lock_t *)user_lock) == UNINIT ) {
            // TODO: Issue an error.
        }; // if
        if ( *((kmp_stub_lock_t *)user_lock) != UNLOCKED ) {
            // TODO: Issue an error.
        }; // if
        *((kmp_stub_lock_t *)user_lock) = LOCKED;
    #else
        __kmpc_set_lock( NULL, __kmp_entry_gtid(), user_lock );
    #endif
}

void FTN_STDCALL
FTN_SET_NEST_LOCK( void **user_lock )
{
    #ifdef KMP_STUB
        if ( *((kmp_stub_lock_t *)user_lock) == UNINIT ) {
            // TODO: Issue an error.
        }; // if
        (*((int *)user_lock))++;
    #else
        __kmpc_set_nest_lock( NULL, __kmp_entry_gtid(), user_lock );
    #endif
}

void FTN_STDCALL
FTN_UNSET_LOCK( void **user_lock )
{
    #ifdef KMP_STUB
        if ( *((kmp_stub_lock_t *)user_lock) == UNINIT ) {
            // TODO: Issue an error.
        }; // if
        if ( *((kmp_stub_lock_t *)user_lock) == UNLOCKED ) {
            // TODO: Issue an error.
        }; // if
        *((kmp_stub_lock_t *)user_lock) = UNLOCKED;
    #else
        __kmpc_unset_lock( NULL, __kmp_entry_gtid(), user_lock );
    #endif
}

void FTN_STDCALL
FTN_UNSET_NEST_LOCK( void **user_lock )
{
    #ifdef KMP_STUB
        if ( *((kmp_stub_lock_t *)user_lock) == UNINIT ) {
            // TODO: Issue an error.
        }; // if
        if ( *((kmp_stub_lock_t *)user_lock) == UNLOCKED ) {
            // TODO: Issue an error.
        }; // if
        (*((int *)user_lock))--;
    #else
        __kmpc_unset_nest_lock( NULL, __kmp_entry_gtid(), user_lock );
    #endif
}

int FTN_STDCALL
FTN_TEST_LOCK( void **user_lock )
{
    #ifdef KMP_STUB
        if ( *((kmp_stub_lock_t *)user_lock) == UNINIT ) {
            // TODO: Issue an error.
        }; // if
        if ( *((kmp_stub_lock_t *)user_lock) == LOCKED ) {
            return 0;
        }; // if
        *((kmp_stub_lock_t *)user_lock) = LOCKED;
        return 1;
    #else
        return __kmpc_test_lock( NULL, __kmp_entry_gtid(), user_lock );
    #endif
}

int FTN_STDCALL
FTN_TEST_NEST_LOCK( void **user_lock )
{
    #ifdef KMP_STUB
        if ( *((kmp_stub_lock_t *)user_lock) == UNINIT ) {
            // TODO: Issue an error.
        }; // if
        return ++(*((int *)user_lock));
    #else
        return __kmpc_test_nest_lock( NULL, __kmp_entry_gtid(), user_lock );
    #endif
}

double FTN_STDCALL
FTN_GET_WTIME( void )
{
    #ifdef KMP_STUB
        return __kmps_get_wtime();
    #else
        double data;
        #if ! KMP_OS_LINUX
        // We don't need library initialization to get the time on Linux* OS.
        // The routine can be used to measure library initialization time on Linux* OS now.
        if ( ! __kmp_init_serial ) {
            __kmp_serial_initialize();
        };
        #endif
        __kmp_elapsed( & data );
        return data;
    #endif
}

double FTN_STDCALL
FTN_GET_WTICK( void )
{
    #ifdef KMP_STUB
        return __kmps_get_wtick();
    #else
        double data;
        if ( ! __kmp_init_serial ) {
            __kmp_serial_initialize();
        };
        __kmp_elapsed_tick( & data );
        return data;
    #endif
}

/* ------------------------------------------------------------------------ */

void * FTN_STDCALL
FTN_MALLOC( size_t KMP_DEREF size )
{
    // kmpc_malloc initializes the library if needed
    return kmpc_malloc( KMP_DEREF size );
}

void * FTN_STDCALL
FTN_CALLOC( size_t KMP_DEREF nelem, size_t KMP_DEREF elsize )
{
    // kmpc_calloc initializes the library if needed
    return kmpc_calloc( KMP_DEREF nelem, KMP_DEREF elsize );
}

void * FTN_STDCALL
FTN_REALLOC( void * KMP_DEREF ptr, size_t KMP_DEREF size )
{
    // kmpc_realloc initializes the library if needed
    return kmpc_realloc( KMP_DEREF ptr, KMP_DEREF size );
}

void FTN_STDCALL
FTN_FREE( void * KMP_DEREF ptr )
{
    // does nothing if the library is not initialized
    kmpc_free( KMP_DEREF ptr );
}

void FTN_STDCALL
FTN_SET_WARNINGS_ON( void )
{
    #ifndef KMP_STUB
        __kmp_generate_warnings = kmp_warnings_explicit;
    #endif
}

void FTN_STDCALL
FTN_SET_WARNINGS_OFF( void )
{
    #ifndef KMP_STUB
        __kmp_generate_warnings = FALSE;
    #endif
}

void FTN_STDCALL
FTN_SET_DEFAULTS( char const * str
    #ifndef PASS_ARGS_BY_VALUE
        , int len
    #endif
)
{
    #ifndef KMP_STUB
        #ifdef PASS_ARGS_BY_VALUE
            int len = strlen( str );
        #endif
        __kmp_aux_set_defaults( str, len );
    #endif
}

/* ------------------------------------------------------------------------ */


#ifdef __cplusplus
    } //extern "C"
#endif // __cplusplus

// end of file //
