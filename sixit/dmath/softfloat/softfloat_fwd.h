
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

#ifndef sixit_dmath_3rdparty_softfloat_inline_softfloat_fwd_h_included
#define sixit_dmath_3rdparty_softfloat_inline_softfloat_fwd_h_included
#include "softfloat_defines.h"
#include "softfloat_types.h"
#include <stdint.h>

namespace sixit::dmath::softfloat
{

inline float32_t softfloat_subMagsF32(uint_fast32_t uiA, uint_fast32_t uiB);
inline float32_t softfloat_addMagsF32(uint_fast32_t uiA, uint_fast32_t uiB);

inline uint_fast8_t softfloat_countLeadingZeros32(uint32_t a);
inline uint32_t softfloat_shiftRightJam32(uint32_t a, uint_fast16_t dist);
inline uint64_t softfloat_shortShiftRightJam64(uint64_t a, uint_fast8_t dist);

inline float32_t softfloat_roundPackToF32(bool sign, int_fast16_t exp, uint_fast32_t sig);
inline float32_t softfloat_normRoundPackToF32(bool sign, int_fast16_t exp, uint_fast32_t sig);

inline exp16_sig32 softfloat_normSubnormalF32Sig(uint_fast32_t);
#ifndef softfloat_approxRecip32_1
inline uint32_t softfloat_approxRecip32_1(uint32_t a);
#endif

// fwd for specializations:
inline uint_fast32_t softfloat_propagateNaNF32UI(uint_fast32_t uiA, uint_fast32_t uiB);
inline uint_fast16_t softfloat_propagateNaNF16UI(uint_fast16_t uiA, uint_fast16_t uiB);

#ifndef softfloat_shiftRightJamM
/*----------------------------------------------------------------------------
| Shifts the N-bit unsigned integer pointed to by 'aPtr' right by the number
| of bits given in 'dist', where N = 'size_words' * 32.  The value of 'dist'
| must not be zero.  If any nonzero bits are shifted off, they are "jammed"
| into the least-significant bit of the shifted value by setting the least-
| significant bit to 1.  This shifted-and-jammed N-bit result is stored
| at the location pointed to by 'zPtr'.  Each of 'aPtr' and 'zPtr' points
| to a 'size_words'-long array of 32-bit elements that concatenate in the
| platform's normal endian order to form an N-bit integer.
|   The value of 'dist' can be arbitrarily large.  In particular, if 'dist'
| is greater than N, the stored result will be either 0 or 1, depending on
| whether the original N bits are all zeros.
*----------------------------------------------------------------------------*/
inline void softfloat_shiftRightJamM(uint_fast8_t size_words, const uint32_t* aPtr, uint32_t dist, uint32_t* zPtr);
#endif // softfloat_shiftRightJamM

inline void softfloat_shortShiftRightJamM(uint_fast8_t size_words, const uint32_t* aPtr, uint_fast8_t dist,
                                          uint32_t* zPtr);

inline void softfloat_shortShiftRightJamM(uint_fast8_t size_words, const uint64_t* aPtr, uint_fast8_t dist,
                                          uint64_t* zPtr);

inline void softfloat_shiftRightJam256M(const uint64_t* aPtr, uint_fast32_t dist, uint64_t* zPtr);

#ifdef SOFTFLOAT_FAST_INT64
inline uint64_extra softfloat_shiftRightJam64Extra(uint64_t a, uint64_t extra, uint_fast32_t dist);
inline int_fast64_t softfloat_roundToI64(bool sign, uint_fast64_t sig, uint_fast64_t sigExtra,
                                         uint_fast8_t roundingMode, bool exact);
#else
inline int_fast64_t
 softfloat_roundMToI64(
     bool sign, uint32_t *extSigPtr, uint_fast8_t roundingMode, bool exact );
#endif

} // namespace sixit::dmath::softfloat

#endif // sixit_dmath_3rdparty_softfloat_inline_softfloat_fwd_h_included