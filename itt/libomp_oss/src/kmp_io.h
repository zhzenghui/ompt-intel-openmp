/*
 * kmp_io.h -- RTL IO header file.
 * $Revision: 42061 $
 * $Date: 2013-02-28 16:36:24 -0600 (Thu, 28 Feb 2013) $
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

#ifndef KMP_IO_H
#define KMP_IO_H

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */

enum kmp_io {
    kmp_out = 0,
    kmp_err
};

extern kmp_bootstrap_lock_t __kmp_stdio_lock;     /* Control stdio functions */
extern kmp_bootstrap_lock_t __kmp_console_lock;   /* Control console initialization */

extern void __kmp_vprintf( enum kmp_io __kmp_io, char const * format, va_list ap );
extern void __kmp_printf( char const * format, ... );
extern void __kmp_printf_no_lock( char const * format, ... );
extern void __kmp_close_console( void );

#ifdef __cplusplus
}
#endif

#endif /* KMP_IO_H */

