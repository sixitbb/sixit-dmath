
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

#ifndef sixit_dmath_3rdparty_softfloat_inline_softfloat_inline_h_included
#define sixit_dmath_3rdparty_softfloat_inline_softfloat_inline_h_included
#include <stdint.h>
#include "platform.h"

#include "softfloat_types.h"
#include "softfloat_defines.h"
#include "softfloat_functions.h"
#include "softfloat_specialize.h"

namespace sixit::dmath::softfloat
{

inline float32_t f32_add( float32_t a, float32_t b );
inline float32_t f32_sub( float32_t a, float32_t b );
inline float32_t f32_mul( float32_t a, float32_t b );
inline float32_t f32_div( float32_t a, float32_t b );

inline bool f32_lt( float32_t a, float32_t b );
inline bool f32_le( float32_t a, float32_t b );
inline bool f32_eq( float32_t a, float32_t b );
inline bool f32_isSignalingNaN( float32_t a );

inline float32_t f32_roundToInt( float32_t a, uint_fast8_t roundingMode, bool exact );
inline int_fast64_t f32_to_i64( float32_t a, uint_fast8_t roundingMode, bool exact );


} // namespace sixit::dmath::softfloat

#endif // sixit_dmath_3rdparty_softfloat_inline_softfloat_inline_h_included