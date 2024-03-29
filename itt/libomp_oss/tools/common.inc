# <copyright>
#    Copyright (c) 2013 Intel Corporation.  All Rights Reserved.
#
#    Redistribution and use in source and binary forms, with or without
#    modification, are permitted provided that the following conditions
#    are met:
#
#      * Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#      * Redistributions in binary form must reproduce the above copyright
#        notice, this list of conditions and the following disclaimer in the
#        documentation and/or other materials provided with the distribution.
#      * Neither the name of Intel Corporation nor the names of its
#        contributors may be used to endorse or promote products derived
#        from this software without specific prior written permission.
#
#    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#
#------------------------------------------------------------------------
#
#    Portions of this software are protected under the following patents:
#        U.S. Patent 5,812,852
#        U.S. Patent 6,792,599
#        U.S. Patent 7,069,556
#        U.S. Patent 7,328,433
#        U.S. Patent 7,500,242
#
# </copyright>

ifndef omp_os

  # Windows sets environment variable OS; for other systems, ask uname
  ifeq ($(OS),)
    OS:=$(shell uname)
    ifeq ($(OS),)
      $(error "Cannot detect operating system")
    endif
    export omp_os=$(OS)
  endif

  ifeq ($(OS), Windows_NT)
    export omp_os=windows
  endif
  ifeq ($(OS), Linux)
    export omp_os=linux
  endif
  ifeq ($(OS), Darwin)
    export omp_os=macos
  endif

endif # !omp_os

# Compiling for the Intel(R) Many Integrated Core architecture is non-trivial at the next layer
# of script down, but we can make it consistent here.
ifeq "$(arch)" "mic"
    # I really do mean this...
    override arch:=32e
    override mic:=yes
else
    override mic:=no
endif

ifeq (,$(wildcard $(omp_root)/tools/$(omp_os).inc))
  $(error "$(omp_os)" is not supported. Add tools/$(omp_os).inc file with os-specific settings )
endif

# detect arch and runtime versions, provide common host-specific definitions
include $(omp_root)/tools/$(omp_os).inc

ifeq ($(arch),)
 $(error Architecture not detected)
endif

# Setting defaults
mode?=release

ifeq "$(omp_os)" "windows"
    compiler?=icl
else
    compiler?=icc
endif

ifneq "$(mic)" "no"
    ifeq "$(compiler)" "gcc"
	$(error Compiling the runtime with gcc is not supported on Intel\(R\) Many Integrated Core Architecture)
    endif

    # Magic flags for the build script!
    build_args += --os=lrb --mic-arch=knc --mic-os=lin --mic-comp=offload

    # Check that the binutils for Intel(R) Many Integrated Core Architecture are available
    # First we see whether the objdump on the user's path supports the k1om architecture.
    hask1om = $(shell if (objdump --help | grep -s k1om); then echo OK; else echo KO; fi)
    ifneq "$(hask1om)" "OK"
        # Appropriate binutils are not already set up, so try to add them from the default place.
        micBinPath = /usr/linux-k1om-4.7/x86_64-k1om-linux/bin
        micBinPresent = $(shell if test -d $(micBinPath); then echo OK; else echo KO; fi)
        ifneq "$(micBinPresent)" "OK"
            # We can't find them in the normal place, so complain.
            $(error Compiling for Intel\(R\) Many Integrated Core Architecture requires that the cross-hosted binutils are available in $(micBinPath).\
See the Tools tab at http://software.intel.com/mic-developer)
        endif
        export PATH := $(micBinPath):${PATH}
    endif
endif

export BUILD_COMPILER := $(compiler)


