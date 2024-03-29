/*
 * kmp_debug.c -- debug utilities for the Guide library
 * $Revision: 42150 $
 * $Date: 2013-03-15 15:40:38 -0500 (Fri, 15 Mar 2013) $
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

#include "kmp.h"
#include "kmp_debug.h" /* really necessary? */
#include "kmp_i18n.h"
#include "kmp_io.h"

#ifdef KMP_DEBUG
void
__kmp_debug_printf_stdout( char const * format, ... )
{
    va_list ap;
    va_start( ap, format );

    __kmp_vprintf( kmp_out, format, ap );

    va_end(ap);
}
#endif

void
__kmp_debug_printf( char const * format, ... )
{
    va_list ap;
    va_start( ap, format );

    __kmp_vprintf( kmp_err, format, ap );

    va_end( ap );
}

#ifdef KMP_USE_ASSERT
    int
    __kmp_debug_assert(
        char const *  msg,
        char const *  file,
        int           line
    ) {

        if ( file == NULL ) {
            file = KMP_I18N_STR( UnknownFile );
        } else {
            // Remove directories from path, leave only file name. File name is enough, there is no need
            // in bothering developers and customers with full paths.
            char const * slash = strrchr( file, '/' );
            if ( slash != NULL ) {
                file = slash + 1;
            }; // if
        }; // if

        #ifdef KMP_DEBUG
            __kmp_acquire_bootstrap_lock( & __kmp_stdio_lock );
            __kmp_debug_printf( "Assertion failure at %s(%d): %s.\n", file, line, msg );
            __kmp_release_bootstrap_lock( & __kmp_stdio_lock );
            #ifdef USE_ASSERT_BREAK
                #if KMP_OS_WINDOWS
                    DebugBreak();
                #endif
            #endif // USE_ASSERT_BREAK
            #ifdef USE_ASSERT_STALL
                /*    __kmp_infinite_loop(); */
                for(;;);
            #endif // USE_ASSERT_STALL
            #ifdef USE_ASSERT_SEG
                {
                    int volatile * ZERO = (int*) 0;
                    ++ (*ZERO);
                }
            #endif // USE_ASSERT_SEG
        #endif

        __kmp_msg(
            kmp_ms_fatal,
            KMP_MSG( AssertionFailure, file, line ),
            KMP_HNT( SubmitBugReport ),
            __kmp_msg_null
        );

        return 0;

    } // __kmp_debug_assert

#endif // KMP_USE_ASSERT

/* Dump debugging buffer to stderr */
void
__kmp_dump_debug_buffer( void )
{
    if ( __kmp_debug_buffer != NULL ) {
        int i;
        int dc = __kmp_debug_count;
        char *db = & __kmp_debug_buffer[ (dc % __kmp_debug_buf_lines) * __kmp_debug_buf_chars ];
        char *db_end = & __kmp_debug_buffer[ __kmp_debug_buf_lines * __kmp_debug_buf_chars ];
        char *db2;

        __kmp_acquire_bootstrap_lock( & __kmp_stdio_lock );
        __kmp_printf_no_lock( "\nStart dump of debugging buffer (entry=%d):\n",
                 dc % __kmp_debug_buf_lines );

        for ( i = 0; i < __kmp_debug_buf_lines; i++ ) {

            if ( *db != '\0' ) {
                /* Fix up where no carriage return before string termination char */
                for ( db2 = db + 1; db2 < db + __kmp_debug_buf_chars - 1; db2 ++) {
                    if ( *db2 == '\0' ) {
                        if ( *(db2-1) != '\n' ) { *db2 = '\n'; *(db2+1) = '\0'; }
                        break;
                    }
                }
                /* Handle case at end by shortening the printed message by one char if necessary */
                if ( db2 == db + __kmp_debug_buf_chars - 1 &&
                     *db2 == '\0' && *(db2-1) != '\n' ) {
                    *(db2-1) = '\n';
                }

                __kmp_printf_no_lock( "%4d: %.*s", i, __kmp_debug_buf_chars, db );
                *db = '\0'; /* only let it print once! */
            }

            db += __kmp_debug_buf_chars;
            if ( db >= db_end )
                db = __kmp_debug_buffer;
        }

        __kmp_printf_no_lock( "End dump of debugging buffer (entry=%d).\n\n",
                 ( dc+i-1 ) % __kmp_debug_buf_lines );
        __kmp_release_bootstrap_lock( & __kmp_stdio_lock );
    }
}
