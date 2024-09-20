/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Victor Istomin
*/
#ifndef thirdparty_isolation_softfloat_platform_h_included
#define thirdparty_isolation_softfloat_platform_h_included
#include "sixit/core/core.h"

/**
 * @file platform.h
 * @brief SoftFloat need platform-specific `platform.h` that set up some defines. This file provides them regardless of
 * platform.
 *
 * See examles in `3rdparty/berkeley-softfloat/build` and `3rdparty/berkeley-softfloat/doc/SoftFloat-source.html`
 */

/*
 * Options below were originally defined in the Makefile for SoftFloat:
 *
 * SOFTFLOAT_OPTS ?=
 *      -DSOFTFLOAT_ROUND_ODD -DINLINE_LEVEL=5 -DSOFTFLOAT_FAST_DIV32TO16
 *      -DSOFTFLOAT_FAST_DIV64TO32
 *
 * COMPILE_C = x86_64-w64-mingw32-gcc -c -Werror-implicit-function-declaration
 *                                    -DSOFTFLOAT_FAST_INT64 $(SOFTFLOAT_OPTS) $(C_INCLUDES) -O2 -o $@
 */

// seems like all our our architectures are LE: x86_64, aarch64 and RISC-V
#define LITTLEENDIAN 1
#define INLINE inline

#if defined(SIXIT_CPU_X64) || defined(SIXIT_CPU_ARM64) || defined(SIXIT_CPU_RISCV64) || defined(SIXIT_CPU_WASM64)
// 64-bit CPU
#define SOFTFLOAT_FAST_INT64
#define SOFTFLOAT_FAST_DIV64TO32
#define SOFTFLOAT_FAST_DIV32TO16
#elif defined(SIXIT_CPU_X86) || defined(SIXIT_CPU_RISCV32) || defined(SIXIT_CPU_WASM32)
// 32-bit CPU, no fast uint64_t operations
#define SOFTFLOAT_FAST_DIV32TO16
#else
static_assert(false, "unknown CPU architecture");
#endif

// clang-format off
#if defined(__GNUC__)
#   if defined(__apple_build_version__) || (defined __OSX__) || (defined __APPLE__)
#       define THREAD_LOCAL __thread
#   else
#       define THREAD_LOCAL __thread 
#   endif
#elif defined(_MSC_VER)
// __declspec instead if C'11 _Thread_local to avoid unnecessary /std:c11 compiler option
#    define THREAD_LOCAL __declspec(thread)
#endif
// clang-format on

#endif // thirdparty_isolation_softfloat_platform_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Victor Istomin

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/