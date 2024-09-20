
/*============================================================================

This C header file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3e, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 The Regents of the
University of California.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions, and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. Neither the name of the University nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS", AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE
DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
Contributors: Sherry Ignatchenko, Victor Istomin
*/

#ifndef sixit_dmath_3rdparty_softfloat_inline_softfloat_specialize_h_included
#define sixit_dmath_3rdparty_softfloat_inline_softfloat_specialize_h_included

#include "sixit/core/core.h"

#if defined(SIXIT_CPU_X64) || defined(SIXIT_CPU_X86) || defined(SIXIT_CPU_WASM64)
#include "softfloat_specialize_x86_64.h"
#elif defined(SIXIT_CPU_ARM64)
#include "softfloat_specialize_arm64.h"
#elif defined(SIXIT_CPU_RISCV32) || defined(SIXIT_CPU_RISCV64)
#include "softfloat_specialize_risc_v.h"
#elif defined(SIXIT_CPU_WASM32)
#include "softfloat_specialize_x86.h"
#else
static_assert(false, "unknown architecture");
#endif

namespace sixit::dmath::softfloat
{
#ifdef SOFTFLOAT_FAST_INT64
union extF80M_extF80 {
    struct extFloat80M fM;
    extFloat80_t f;
};
union ui128_f128 {
    struct uint128 ui;
    float128_t f;
};
#endif
} // namespace sixit::dmath::softfloat

#endif // sixit_dmath_3rdparty_softfloat_inline_softfloat_specialize_h_included