
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

#ifndef sixit_dmath_3rdparty_softfloat_inline_softfloat_functions_h_included
#define sixit_dmath_3rdparty_softfloat_inline_softfloat_functions_h_included
#include <stdint.h>
#include "platform.h"
#include "softfloat_fwd.h"
#include "softfloat_types.h"
#include "softfloat_defines.h"
#include "softfloat_specialize.h"

#include "sixit/core/lwa.h"
#include <type_traits>

#ifndef INLINE_LEVEL
#define INLINE_LEVEL 5
#endif

namespace sixit::dmath::softfloat
{

template <typename UnsignedT, typename = std::enable_if<std::is_unsigned_v<UnsignedT>>>
inline UnsignedT neg_unsigned(UnsignedT i)
{
    return UnsignedT(0) - i;  // suppresses a warning
}

inline float32_t f32_add( float32_t a, float32_t b )
{
    union ui32_f32 uA;
    uint_fast32_t uiA;
    union ui32_f32 uB;
    uint_fast32_t uiB;
#if ! defined INLINE_LEVEL || (INLINE_LEVEL < 1)
    float32_t (*magsFuncPtr)( uint_fast32_t, uint_fast32_t );
#endif

    uA.f = a;
    uiA = uA.ui;
    uB.f = b;
    uiB = uB.ui;
#if defined INLINE_LEVEL && (1 <= INLINE_LEVEL)
    if ( signF32UI( uiA ^ uiB ) ) {
        return softfloat_subMagsF32( uiA, uiB );
    } else {
        return softfloat_addMagsF32( uiA, uiB );
    }
#else
    magsFuncPtr =
        signF32UI( uiA ^ uiB ) ? softfloat_subMagsF32 : softfloat_addMagsF32;
    return (*magsFuncPtr)( uiA, uiB );
#endif

}

inline float32_t f32_sub( float32_t a, float32_t b )
{
    union ui32_f32 uA;
    uint_fast32_t uiA;
    union ui32_f32 uB;
    uint_fast32_t uiB;
#if ! defined INLINE_LEVEL || (INLINE_LEVEL < 1)
    float32_t (*magsFuncPtr)( uint_fast32_t, uint_fast32_t );
#endif

    uA.f = a;
    uiA = uA.ui;
    uB.f = b;
    uiB = uB.ui;
#if defined INLINE_LEVEL && (1 <= INLINE_LEVEL)
    if ( signF32UI( uiA ^ uiB ) ) {
        return softfloat_addMagsF32( uiA, uiB );
    } else {
        return softfloat_subMagsF32( uiA, uiB );
    }
#else
    magsFuncPtr =
        signF32UI( uiA ^ uiB ) ? softfloat_addMagsF32 : softfloat_subMagsF32;
    return (*magsFuncPtr)( uiA, uiB );
#endif

}

inline float32_t f32_mul( float32_t a, float32_t b )
{
    union ui32_f32 uA;
    uint_fast32_t uiA;
    bool signA;
    int_fast16_t expA;
    uint_fast32_t sigA;
    union ui32_f32 uB;
    uint_fast32_t uiB;
    bool signB;
    int_fast16_t expB;
    uint_fast32_t sigB;
    bool signZ;
    uint_fast32_t magBits;
    struct exp16_sig32 normExpSig;
    int_fast16_t expZ;
    uint_fast32_t sigZ, uiZ;
    union ui32_f32 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uA.f = a;
    uiA = uA.ui;
    signA = signF32UI( uiA );
    expA  = expF32UI( uiA );
    sigA  = fracF32UI( uiA );
    uB.f = b;
    uiB = uB.ui;
    signB = signF32UI( uiB );
    expB  = expF32UI( uiB );
    sigB  = fracF32UI( uiB );
    signZ = signA ^ signB;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( expA == 0xFF ) {
        if ( sigA || ((expB == 0xFF) && sigB) ) goto propagateNaN;
        magBits = expB | sigB;
        goto infArg;
    }
    if ( expB == 0xFF ) {
        if ( sigB ) goto propagateNaN;
        magBits = expA | sigA;
        goto infArg;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( ! expA ) {
        if ( ! sigA ) goto zero;
        normExpSig = softfloat_normSubnormalF32Sig( sigA );
        expA = normExpSig.exp;
        sigA = normExpSig.sig;
    }
    if ( ! expB ) {
        if ( ! sigB ) goto zero;
        normExpSig = softfloat_normSubnormalF32Sig( sigB );
        expB = normExpSig.exp;
        sigB = normExpSig.sig;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expZ = expA + expB - 0x7F;
    sigA = (sigA | 0x00800000)<<7;
    sigB = (sigB | 0x00800000)<<8;
    sigZ = static_cast<uint_fast32_t>(softfloat_shortShiftRightJam64( (uint_fast64_t) sigA * sigB, 32 ));
    if ( sigZ < 0x40000000 ) {
        --expZ;
        sigZ <<= 1;
    }
    return softfloat_roundPackToF32( signZ, expZ, sigZ );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 propagateNaN:
    uiZ = softfloat_propagateNaNF32UI( uiA, uiB );
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 infArg:
    if ( ! magBits ) {
        softfloat_raiseFlags( softfloat_flag_invalid );
        uiZ = defaultNaNF32UI;
    } else {
        uiZ = packToF32UI( signZ, 0xFF, 0 );
    }
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 zero:
    uiZ = packToF32UI( signZ, 0, 0 );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}

inline float32_t f32_div( float32_t a, float32_t b )
{
    union ui32_f32 uA;
    uint_fast32_t uiA;
    bool signA;
    int_fast16_t expA;
    uint_fast32_t sigA;
    union ui32_f32 uB;
    uint_fast32_t uiB;
    bool signB;
    int_fast16_t expB;
    uint_fast32_t sigB;
    bool signZ;
    struct exp16_sig32 normExpSig;
    int_fast16_t expZ;
#ifdef SOFTFLOAT_FAST_DIV64TO32
    uint_fast64_t sig64A;
    uint_fast32_t sigZ;
#else
    uint_fast32_t sigZ;
    uint_fast64_t rem;
#endif
    uint_fast32_t uiZ;
    union ui32_f32 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uA.f = a;
    uiA = uA.ui;
    signA = signF32UI( uiA );
    expA  = expF32UI( uiA );
    sigA  = fracF32UI( uiA );
    uB.f = b;
    uiB = uB.ui;
    signB = signF32UI( uiB );
    expB  = expF32UI( uiB );
    sigB  = fracF32UI( uiB );
    signZ = signA ^ signB;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( expA == 0xFF ) {
        if ( sigA ) goto propagateNaN;
        if ( expB == 0xFF ) {
            if ( sigB ) goto propagateNaN;
            goto invalid;
        }
        goto infinity;
    }
    if ( expB == 0xFF ) {
        if ( sigB ) goto propagateNaN;
        goto zero;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( ! expB ) {
        if ( ! sigB ) {
            if ( ! (expA | sigA) ) goto invalid;
            softfloat_raiseFlags( softfloat_flag_infinite );
            goto infinity;
        }
        normExpSig = softfloat_normSubnormalF32Sig( sigB );
        expB = normExpSig.exp;
        sigB = normExpSig.sig;
    }
    if ( ! expA ) {
        if ( ! sigA ) goto zero;
        normExpSig = softfloat_normSubnormalF32Sig( sigA );
        expA = normExpSig.exp;
        sigA = normExpSig.sig;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expZ = expA - expB + 0x7E;
    sigA |= 0x00800000;
    sigB |= 0x00800000;
#ifdef SOFTFLOAT_FAST_DIV64TO32
    if ( sigA < sigB ) {
        --expZ;
        sig64A = (uint_fast64_t) sigA<<31;
    } else {
        sig64A = (uint_fast64_t) sigA<<30;
    }
    sigZ = static_cast<uint_fast32_t>(sig64A / sigB);
    if ( ! (sigZ & 0x3F) ) sigZ |= static_cast<uint_fast32_t>((uint_fast64_t) sigB * sigZ != sig64A);
#else
    if ( sigA < sigB ) {
        --expZ;
        sigA <<= 8;
    } else {
        sigA <<= 7;
    }
    sigB <<= 8;
    sigZ = ((uint_fast64_t) sigA * softfloat_approxRecip32_1( sigB ))>>32;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    sigZ += 2;
    if ( (sigZ & 0x3F) < 2 ) {
        sigZ &= ~3;
#ifdef SOFTFLOAT_FAST_INT64
        rem = ((uint_fast64_t) sigA<<31) - (uint_fast64_t) sigZ * sigB;
#else
        rem = ((uint_fast64_t) sigA<<32) - (uint_fast64_t) (sigZ<<1) * sigB;
#endif
        if ( rem & UINT64_C( 0x8000000000000000 ) ) {
            sigZ -= 4;
        } else {
            if ( rem ) sigZ |= 1;
        }
    }
#endif
    return softfloat_roundPackToF32( signZ, expZ, sigZ );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 propagateNaN:
    uiZ = softfloat_propagateNaNF32UI( uiA, uiB );
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 invalid:
    softfloat_raiseFlags( softfloat_flag_invalid );
    uiZ = defaultNaNF32UI;
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 infinity:
    uiZ = packToF32UI( signZ, 0xFF, 0 );
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 zero:
    uiZ = packToF32UI( signZ, 0, 0 );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}

inline bool f32_lt( float32_t a, float32_t b )
{
    union ui32_f32 uA;
    uint_fast32_t uiA;
    union ui32_f32 uB;
    uint_fast32_t uiB;
    bool signA, signB;

    uA.f = a;
    uiA = uA.ui;
    uB.f = b;
    uiB = uB.ui;
    if ( isNaNF32UI( uiA ) || isNaNF32UI( uiB ) ) {
        softfloat_raiseFlags( softfloat_flag_invalid );
        return false;
    }
    signA = signF32UI( uiA );
    signB = signF32UI( uiB );
    return
        (signA != signB) ? signA && ((uint32_t) ((uiA | uiB)<<1) != 0)
            : (uiA != uiB) && (signA ^ (uiA < uiB));

}

inline bool f32_le( float32_t a, float32_t b )
{
    union ui32_f32 uA;
    uint_fast32_t uiA;
    union ui32_f32 uB;
    uint_fast32_t uiB;
    bool signA, signB;

    uA.f = a;
    uiA = uA.ui;
    uB.f = b;
    uiB = uB.ui;
    if ( isNaNF32UI( uiA ) || isNaNF32UI( uiB ) ) {
        softfloat_raiseFlags( softfloat_flag_invalid );
        return false;
    }
    signA = signF32UI( uiA );
    signB = signF32UI( uiB );
    return
        (signA != signB) ? signA || ! (uint32_t) ((uiA | uiB)<<1)
            : (uiA == uiB) || (signA ^ (uiA < uiB));

}

inline bool f32_eq( float32_t a, float32_t b )
{
    union ui32_f32 uA;
    uint_fast32_t uiA;
    union ui32_f32 uB;
    uint_fast32_t uiB;

    uA.f = a;
    uiA = uA.ui;
    uB.f = b;
    uiB = uB.ui;
    if ( isNaNF32UI( uiA ) || isNaNF32UI( uiB ) ) {
        if (
            softfloat_isSigNaNF32UI( uiA ) || softfloat_isSigNaNF32UI( uiB )
        ) {
            softfloat_raiseFlags( softfloat_flag_invalid );
        }
        return false;
    }
    return (uiA == uiB) || ! (uint32_t) ((uiA | uiB)<<1);

}

inline bool f32_isSignalingNaN( float32_t a )
{
    union ui32_f32 uA;

    uA.f = a;
    return softfloat_isSigNaNF32UI( uA.ui );
}

inline float32_t f32_roundToInt( float32_t a, uint_fast8_t roundingMode, bool exact )
{
    union ui32_f32 uA;
    uint_fast32_t uiA;
    int_fast16_t exp;
    uint_fast32_t uiZ, lastBitMask, roundBitsMask;
    union ui32_f32 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uA.f = a;
    uiA = uA.ui;
    exp = expF32UI( uiA );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( exp <= 0x7E ) {
        if ( !(uint32_t) (uiA<<1) ) return a;
        if ( exact ) softfloat_exceptionFlags |= softfloat_flag_inexact;
        uiZ = uiA & packToF32UI( 1, 0, 0 );
        switch ( roundingMode ) {
         case softfloat_round_near_even:
            if ( !fracF32UI( uiA ) ) break;
         case softfloat_round_near_maxMag:
            if ( exp == 0x7E ) uiZ |= packToF32UI( 0, 0x7F, 0 );
            break;
         case softfloat_round_min:
            if ( uiZ ) uiZ = packToF32UI( 1, 0x7F, 0 );
            break;
         case softfloat_round_max:
            if ( !uiZ ) uiZ = packToF32UI( 0, 0x7F, 0 );
            break;
#ifdef SOFTFLOAT_ROUND_ODD
         case softfloat_round_odd:
            uiZ |= packToF32UI( 0, 0x7F, 0 );
            break;
#endif
        }
        goto uiZ;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( 0x96 <= exp ) {
        if ( (exp == 0xFF) && fracF32UI( uiA ) ) {
            uiZ = softfloat_propagateNaNF32UI( uiA, 0 );
            goto uiZ;
        }
        return a;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uiZ = uiA;
    lastBitMask = (uint_fast32_t) 1<<(0x96 - exp);
    roundBitsMask = lastBitMask - 1;
    if ( roundingMode == softfloat_round_near_maxMag ) {
        uiZ += lastBitMask>>1;
    } else if ( roundingMode == softfloat_round_near_even ) {
        uiZ += lastBitMask>>1;
        if ( !(uiZ & roundBitsMask) ) uiZ &= ~lastBitMask;
    } else if (
        roundingMode
            == (signF32UI( uiZ ) ? softfloat_round_min : softfloat_round_max)
    ) {
        uiZ += roundBitsMask;
    }
    uiZ &= ~roundBitsMask;
    if ( uiZ != uiA ) {
#ifdef SOFTFLOAT_ROUND_ODD
        if ( roundingMode == softfloat_round_odd ) uiZ |= lastBitMask;
#endif
        if ( exact ) softfloat_exceptionFlags |= softfloat_flag_inexact;
    }
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;
}

inline int_fast64_t f32_to_i64( float32_t a, uint_fast8_t roundingMode, bool exact )
{
    union ui32_f32 uA;
    uint_fast32_t uiA;
    bool sign;
    int_fast16_t exp;
    uint_fast32_t sig;
    int_fast16_t shiftDist;
#ifdef SOFTFLOAT_FAST_INT64
    uint_fast64_t sig64, extra;
    struct uint64_extra sig64Extra;
#else
    uint32_t extSig[3];
#endif

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uA.f = a;
    uiA = uA.ui;
    sign = signF32UI( uiA );
    exp  = expF32UI( uiA );
    sig  = fracF32UI( uiA );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    shiftDist = 0xBE - exp;
    if ( shiftDist < 0 ) {
        softfloat_raiseFlags( softfloat_flag_invalid );
        return
            (exp == 0xFF) && sig ? i64_fromNaN
                : sign ? i64_fromNegOverflow : i64_fromPosOverflow;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( exp ) sig |= 0x00800000;
#ifdef SOFTFLOAT_FAST_INT64
    sig64 = (uint_fast64_t) sig<<40;
    extra = 0;
    if ( shiftDist ) {
        sig64Extra = softfloat_shiftRightJam64Extra( sig64, 0, shiftDist );
        sig64 = sig64Extra.v;
        extra = sig64Extra.extra;
    }
    return softfloat_roundToI64( sign, sig64, extra, roundingMode, exact );
#else
    extSig[indexWord( 3, 2 )] = sig<<8;
    extSig[indexWord( 3, 1 )] = 0;
    extSig[indexWord( 3, 0 )] = 0;
    if ( shiftDist ) softfloat_shiftRightJam96M( extSig, shiftDist, extSig );
    return softfloat_roundMToI64( sign, extSig, roundingMode, exact );
#endif

}

// ----------- private implementation below --------------

inline float32_t softfloat_subMagsF32( uint_fast32_t uiA, uint_fast32_t uiB )
{
    int_fast16_t expA;
    uint_fast32_t sigA;
    int_fast16_t expB;
    uint_fast32_t sigB;
    int_fast16_t expDiff;
    uint_fast32_t uiZ;
    int_fast32_t sigDiff;
    bool signZ;
    int_fast8_t shiftDist;
    int_fast16_t expZ;
    uint_fast32_t sigX, sigY;
    union ui32_f32 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expA = expF32UI( uiA );
    sigA = fracF32UI( uiA );
    expB = expF32UI( uiB );
    sigB = fracF32UI( uiB );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expDiff = expA - expB;
    if ( ! expDiff ) {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/
        if ( expA == 0xFF ) {
            if ( sigA | sigB ) goto propagateNaN;
            softfloat_raiseFlags( softfloat_flag_invalid );
            uiZ = defaultNaNF32UI;
            goto uiZ;
        }
        sigDiff = sigA - sigB;
        if ( ! sigDiff ) {
            uiZ =
                packToF32UI(
                    (softfloat_roundingMode == softfloat_round_min), 0, 0 );
            goto uiZ;
        }
        if ( expA ) --expA;
        signZ = signF32UI( uiA );
        if ( sigDiff < 0 ) {
            signZ = ! signZ;
            sigDiff = -sigDiff;
        }
        shiftDist = softfloat_countLeadingZeros32( sigDiff ) - 8;
        expZ = expA - shiftDist;
        if ( expZ < 0 ) {
            shiftDist = static_cast<int_fast8_t>(expA);
            expZ = 0;
        }
        uiZ = packToF32UI( signZ, expZ, sigDiff<<shiftDist );
        goto uiZ;
    } else {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/
        signZ = signF32UI( uiA );
        sigA <<= 7;
        sigB <<= 7;
        if ( expDiff < 0 ) {
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            signZ = ! signZ;
            if ( expB == 0xFF ) {
                if ( sigB ) goto propagateNaN;
                uiZ = packToF32UI( signZ, 0xFF, 0 );
                goto uiZ;
            }
            expZ = expB - 1;
            sigX = sigB | 0x40000000;
            sigY = sigA + (expA ? 0x40000000 : sigA);
            expDiff = -expDiff;
        } else {
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            if ( expA == 0xFF ) {
                if ( sigA ) goto propagateNaN;
                uiZ = uiA;
                goto uiZ;
            }
            expZ = expA - 1;
            sigX = sigA | 0x40000000;
            sigY = sigB + (expB ? 0x40000000 : sigB);
        }
        return
            softfloat_normRoundPackToF32(
                signZ, expZ, sigX - softfloat_shiftRightJam32( sigY, expDiff )
            );
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 propagateNaN:
    uiZ = softfloat_propagateNaNF32UI( uiA, uiB );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}

#ifdef SOFTFLOAT_BUILTIN_CLZ

inline uint_fast8_t softfloat_countLeadingZeros16( uint16_t a )
    { return a ? __builtin_clz( a ) - 16 : 16; }

inline uint_fast8_t softfloat_countLeadingZeros32( uint32_t a )
    { return a ? __builtin_clz( a ) : 32; }

inline uint_fast8_t softfloat_countLeadingZeros64( uint64_t a )
    { return a ? __builtin_clzll( a ) : 64; }
#else

inline uint_fast8_t softfloat_countLeadingZeros32( uint32_t a )
{
    static constexpr uint_least8_t softfloat_countLeadingZeros8[256] = {
        8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    uint_fast8_t count = 0;
    if ( a < 0x10000 ) {
        count = 16;
        a <<= 16;
    }
    if ( a < 0x1000000 ) {
        count += 8;
        a <<= 8;
    }
    count += softfloat_countLeadingZeros8[a>>24];
    return count;
}

#endif

inline float32_t softfloat_addMagsF32( uint_fast32_t uiA, uint_fast32_t uiB )
{
    int_fast16_t expA;
    uint_fast32_t sigA;
    int_fast16_t expB;
    uint_fast32_t sigB;
    int_fast16_t expDiff;
    uint_fast32_t uiZ;
    bool signZ;
    int_fast16_t expZ;
    uint_fast32_t sigZ;
    union ui32_f32 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expA = expF32UI( uiA );
    sigA = fracF32UI( uiA );
    expB = expF32UI( uiB );
    sigB = fracF32UI( uiB );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expDiff = expA - expB;
    if ( ! expDiff ) {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/
        if ( ! expA ) {
            uiZ = uiA + sigB;
            goto uiZ;
        }
        if ( expA == 0xFF ) {
            if ( sigA | sigB ) goto propagateNaN;
            uiZ = uiA;
            goto uiZ;
        }
        signZ = signF32UI( uiA );
        expZ = expA;
        sigZ = 0x01000000 + sigA + sigB;
        if ( ! (sigZ & 1) && (expZ < 0xFE) ) {
            uiZ = packToF32UI( signZ, expZ, sigZ>>1 );
            goto uiZ;
        }
        sigZ <<= 6;
    } else {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/
        signZ = signF32UI( uiA );
        sigA <<= 6;
        sigB <<= 6;
        if ( expDiff < 0 ) {
            if ( expB == 0xFF ) {
                if ( sigB ) goto propagateNaN;
                uiZ = packToF32UI( signZ, 0xFF, 0 );
                goto uiZ;
            }
            expZ = expB;
            sigA += expA ? 0x20000000 : sigA;
            sigA = softfloat_shiftRightJam32( sigA, -expDiff );
        } else {
            if ( expA == 0xFF ) {
                if ( sigA ) goto propagateNaN;
                uiZ = uiA;
                goto uiZ;
            }
            expZ = expA;
            sigB += expB ? 0x20000000 : sigB;
            sigB = softfloat_shiftRightJam32( sigB, expDiff );
        }
        sigZ = 0x20000000 + sigA + sigB;
        if ( sigZ < 0x40000000 ) {
            --expZ;
            sigZ <<= 1;
        }
    }
    return softfloat_roundPackToF32( signZ, expZ, sigZ );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 propagateNaN:
    uiZ = softfloat_propagateNaNF32UI( uiA, uiB );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}

/*----------------------------------------------------------------------------
| Shifts 'a' right by the number of bits given in 'dist', which must not
| be zero.  If any nonzero bits are shifted off, they are "jammed" into the
| least-significant bit of the shifted value by setting the least-significant
| bit to 1.  This shifted-and-jammed value is returned.
|   The value of 'dist' can be arbitrarily large.  In particular, if 'dist' is
| greater than 32, the result will be either 0 or 1, depending on whether 'a'
| is zero or nonzero.
*----------------------------------------------------------------------------*/
inline uint32_t softfloat_shiftRightJam32( uint32_t a, uint_fast16_t dist )
{
    return (dist < 31) ? a>>dist | ((uint32_t) (a<<(neg_unsigned(dist) & 31)) != 0) : (a != 0);
}

/*----------------------------------------------------------------------------
| Shifts 'a' right by the number of bits given in 'dist', which must be in
| the range 1 to 63.  If any nonzero bits are shifted off, they are "jammed"
| into the least-significant bit of the shifted value by setting the least-
| significant bit to 1.  This shifted-and-jammed value is returned.
*----------------------------------------------------------------------------*/
inline uint64_t softfloat_shortShiftRightJam64( uint64_t a, uint_fast8_t dist )
{ 
    return a>>dist | ((a & (((uint_fast64_t) 1<<dist) - 1)) != 0); 
}

inline float32_t softfloat_roundPackToF32( bool sign, int_fast16_t exp, uint_fast32_t sig )
{
    uint_fast8_t roundingMode;
    bool roundNearEven;
    uint_fast8_t roundIncrement, roundBits;
    bool isTiny;
    uint_fast32_t uiZ;
    union ui32_f32 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    roundingMode = softfloat_roundingMode;
    roundNearEven = (roundingMode == softfloat_round_near_even);
    roundIncrement = 0x40;
    if ( ! roundNearEven && (roundingMode != softfloat_round_near_maxMag) ) {
        roundIncrement =
            (roundingMode
                 == (sign ? softfloat_round_min : softfloat_round_max))
                ? 0x7F
                : 0;
    }
    roundBits = sig & 0x7F;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( 0xFD <= (unsigned int) exp ) {
        if ( exp < 0 ) {
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            isTiny =
                (softfloat_detectTininess == softfloat_tininess_beforeRounding)
                    || (exp < -1) || (sig + roundIncrement < 0x80000000);
            sig = softfloat_shiftRightJam32( sig, -exp );
            exp = 0;
            roundBits = sig & 0x7F;
            if ( isTiny && roundBits ) {
                softfloat_raiseFlags( softfloat_flag_underflow );
            }
        } else if ( (0xFD < exp) || (0x80000000 <= sig + roundIncrement) ) {
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            softfloat_raiseFlags(
                softfloat_flag_overflow | softfloat_flag_inexact );
            uiZ = packToF32UI( sign, 0xFF, 0 ) - ! roundIncrement;
            goto uiZ;
        }
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    sig = (sig + roundIncrement)>>7;
    if ( roundBits ) {
        softfloat_exceptionFlags |= softfloat_flag_inexact;
#ifdef SOFTFLOAT_ROUND_ODD
        if ( roundingMode == softfloat_round_odd ) {
            sig |= 1;
            goto packReturn;
        }
#endif
    }
    sig &= ~(uint_fast32_t) (! (roundBits ^ 0x40) & roundNearEven);
    if ( ! sig ) exp = 0;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
#ifdef SOFTFLOAT_ROUND_ODD
 packReturn:
#endif
    uiZ = packToF32UI( sign, exp, sig );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}

inline float32_t softfloat_normRoundPackToF32( bool sign, int_fast16_t exp, uint_fast32_t sig )
{
    int_fast8_t shiftDist;
    union ui32_f32 uZ;

    shiftDist = softfloat_countLeadingZeros32( sig ) - 1;
    exp -= shiftDist;
    if ( (7 <= shiftDist) && ((unsigned int) exp < 0xFD) ) {
        uZ.ui = packToF32UI( sign, sig ? exp : 0, sig<<(shiftDist - 7) );
        return uZ.f;
    } else {
        return softfloat_roundPackToF32( sign, exp, sig<<shiftDist );
    }

}

inline exp16_sig32 softfloat_normSubnormalF32Sig( uint_fast32_t sig )
{
    int_fast8_t shiftDist;
    struct exp16_sig32 z;

    shiftDist = softfloat_countLeadingZeros32( sig ) - 8;
    z.exp = 1 - shiftDist;
    z.sig = sig<<shiftDist;
    return z;

}

#ifndef softfloat_approxRecip32_1
/*----------------------------------------------------------------------------
| Returns an approximation to the reciprocal of the number represented by 'a',
| where 'a' is interpreted as an unsigned fixed-point number with one integer
| bit and 31 fraction bits.  The 'a' input must be "normalized", meaning that
| its most-significant bit (bit 31) must be 1.  Thus, if A is the value of
| the fixed-point interpretation of 'a', then 1 <= A < 2.  The returned value
| is interpreted as a pure unsigned fraction, having no integer bits and 32
| fraction bits.  The approximation returned is never greater than the true
| reciprocal 1/A, and it differs from the true reciprocal by at most 2.006 ulp
| (units in the last place).
*----------------------------------------------------------------------------*/
inline uint32_t softfloat_approxRecip32_1( uint32_t a )
{
    static constexpr uint16_t softfloat_approxRecip_1k0s[16] = {
        0xFFC4, 0xF0BE, 0xE363, 0xD76F, 0xCCAD, 0xC2F0, 0xBA16, 0xB201,
        0xAA97, 0xA3C6, 0x9D7A, 0x97A6, 0x923C, 0x8D32, 0x887E, 0x8417
    };
    static constexpr uint16_t softfloat_approxRecip_1k1s[16] = {
        0xF0F1, 0xD62C, 0xBFA1, 0xAC77, 0x9C0A, 0x8DDB, 0x8185, 0x76BA,
        0x6D3B, 0x64D4, 0x5D5C, 0x56B1, 0x50B6, 0x4B55, 0x4679, 0x4211
    };

    int index;
    uint16_t eps, r0;
    uint32_t sigma0;
    uint_fast32_t r;
    uint32_t sqrSigma0;

    index = a>>27 & 0xF;
    eps = (uint16_t) (a>>11);
    r0 = softfloat_approxRecip_1k0s[index]
             - ((softfloat_approxRecip_1k1s[index] * (uint_fast32_t) eps)>>20);
    sigma0 = ~(uint_fast32_t) ((r0 * (uint_fast64_t) a)>>7);
    r = ((uint_fast32_t) r0<<16) + ((r0 * (uint_fast64_t) sigma0)>>24);
    sqrSigma0 = ((uint_fast64_t) sigma0 * sigma0)>>32;
    r += ((uint32_t) r * (uint_fast64_t) sqrSigma0)>>48;
    return r;

}
#endif // softfloat_approxRecip32_1

#ifndef softfloat_shiftRightJamM
#define softfloat_shiftRightJamM softfloat_shiftRightJamM
inline void
 softfloat_shiftRightJamM(
     uint_fast8_t size_words,
     const uint32_t *aPtr,
     uint32_t dist,
     uint32_t *zPtr
 )
{
    uint32_t wordJam, wordDist, *ptr;
    uint_fast8_t i, innerDist;

    wordJam = 0;
    wordDist = dist>>5;
    if ( wordDist ) {
        if ( size_words < wordDist ) wordDist = size_words;
        ptr = (uint32_t *) (aPtr + indexMultiwordLo( size_words, wordDist ));
        i = static_cast<uint_fast8_t>(wordDist);
        do {
            wordJam = *ptr++;
            if ( wordJam ) break;
            --i;
        } while ( i );
        ptr = zPtr;
    }
    if ( wordDist < size_words ) {
        aPtr += indexMultiwordHiBut( size_words, wordDist );
        innerDist = dist & 31;
        if ( innerDist ) {
            softfloat_shortShiftRightJamM(
                size_words - static_cast<uint_fast8_t>(wordDist),
                aPtr,
                innerDist,
                zPtr + indexMultiwordLoBut( size_words, wordDist )
            );
            if ( ! wordDist ) goto wordJam;
        } else {
            aPtr += indexWordLo( size_words - wordDist );
            ptr = zPtr + indexWordLo( size_words );
            for ( i = size_words - static_cast<uint_fast8_t>(wordDist); i; --i ) {
                *ptr = *aPtr;
                aPtr += wordIncr;
                ptr += wordIncr;
            }
        }
        ptr = zPtr + indexMultiwordHi( size_words, wordDist );
    }
    do {
        *ptr++ = 0;
        --wordDist;
    } while ( wordDist );
 wordJam:
    if ( wordJam ) zPtr[indexWordLo( size_words )] |= 1;

}

#endif // softfloat_shiftRightJamM

inline void
 softfloat_shiftRightJam256M(
     const uint64_t *aPtr, uint_fast32_t dist, uint64_t *zPtr )
{
    uint64_t wordJam;
    uint_fast32_t wordDist;
    uint64_t *ptr;
    uint_fast8_t i, innerDist;

    wordJam = 0;
    wordDist = dist>>6;
    if ( wordDist ) {
        if ( 4 < wordDist ) wordDist = 4;
        ptr = (uint64_t *) (aPtr + indexMultiwordLo( 4, wordDist ));
        i = static_cast<uint_fast8_t>(wordDist);
        do {
            wordJam = *ptr++;
            if ( wordJam ) break;
            --i;
        } while ( i );
        ptr = zPtr;
    }
    if ( wordDist < 4 ) {
        aPtr += indexMultiwordHiBut( 4, wordDist );
        innerDist = dist & 63;
        if ( innerDist ) {
            softfloat_shortShiftRightJamM(
                4 - static_cast<uint_fast8_t>(wordDist),
                aPtr,
                innerDist,
                zPtr + indexMultiwordLoBut( 4, wordDist )
            );
            if ( ! wordDist ) goto wordJam;
        } else {
            aPtr += indexWordLo( 4 - wordDist );
            ptr = zPtr + indexWordLo( 4 );
            for ( i = 4 - static_cast<uint_fast8_t>(wordDist); i; --i ) {
                *ptr = *aPtr;
                aPtr += wordIncr;
                ptr += wordIncr;
            }
        }
        ptr = zPtr + indexMultiwordHi( 4, wordDist );
    }
    do {
        *ptr++ = 0;
        --wordDist;
    } while ( wordDist );
 wordJam:
    if ( wordJam ) zPtr[indexWordLo( 4 )] |= 1;

}

#ifdef SOFTFLOAT_FAST_INT64

/*----------------------------------------------------------------------------
| Shifts the 128 bits formed by concatenating 'a' and 'extra' right by 64
| _plus_ the number of bits given in 'dist', which must not be zero.  This
| shifted value is at most 64 nonzero bits and is returned in the 'v' field
| of the 'struct uint64_extra' result.  The 64-bit 'extra' field of the result
| contains a value formed as follows from the bits that were shifted off:  The
| _last_ bit shifted off is the most-significant bit of the 'extra' field, and
| the other 63 bits of the 'extra' field are all zero if and only if _all_but_
| _the_last_ bits shifted off were all zero.
|   (This function makes more sense if 'a' and 'extra' are considered to form
| an unsigned fixed-point number with binary point between 'a' and 'extra'.
| This fixed-point value is shifted right by the number of bits given in
| 'dist', and the integer part of this shifted value is returned in the 'v'
| field of the result.  The fractional part of the shifted value is modified
| as described above and returned in the 'extra' field of the result.)
*----------------------------------------------------------------------------*/
inline
struct uint64_extra
 softfloat_shiftRightJam64Extra(
     uint64_t a, uint64_t extra, uint_fast32_t dist )
{
    struct uint64_extra z;
    if ( dist < 64 ) {
        z.v = a>>dist;
        z.extra = a<<(neg_unsigned(dist) & 63);
    } else {
        z.v = 0;
        z.extra = (dist == 64) ? a : (a != 0);
    }
    z.extra |= (extra != 0);
    return z;
}

inline int_fast64_t
 softfloat_roundToI64(
     bool sign,
     uint_fast64_t sig,
     uint_fast64_t sigExtra,
     uint_fast8_t roundingMode,
     bool exact
 )
{
    union { uint64_t ui; int64_t i; } uZ;
    int_fast64_t z;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (
        (roundingMode == softfloat_round_near_maxMag)
            || (roundingMode == softfloat_round_near_even)
    ) {
        if ( UINT64_C( 0x8000000000000000 ) <= sigExtra ) goto increment;
    } else {
        if (
            sigExtra
                && (sign
                        ? (roundingMode == softfloat_round_min)
#ifdef SOFTFLOAT_ROUND_ODD
                              || (roundingMode == softfloat_round_odd)
#endif
                        : (roundingMode == softfloat_round_max))
        ) {
 increment:
            ++sig;
            if ( !sig ) goto invalid;
            if (
                (sigExtra == UINT64_C( 0x8000000000000000 ))
                    && (roundingMode == softfloat_round_near_even)
            ) {
                sig &= ~(uint_fast64_t) 1;
            }
        }
    }
    uZ.ui = sign ? neg_unsigned(sig) : sig;
    z = uZ.i;
    if ( z && ((z < 0) ^ sign) ) goto invalid;
    if ( sigExtra ) {
#ifdef SOFTFLOAT_ROUND_ODD
        if ( roundingMode == softfloat_round_odd ) z |= 1;
#endif
        if ( exact ) softfloat_exceptionFlags |= softfloat_flag_inexact;
    }
    return z;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 invalid:
    softfloat_raiseFlags( softfloat_flag_invalid );
    return sign ? i64_fromNegOverflow : i64_fromPosOverflow;

}
#else // non-SOFTFLOAT_FAST_INT64
inline int_fast64_t
 softfloat_roundMToI64(
     bool sign, uint32_t *extSigPtr, uint_fast8_t roundingMode, bool exact )
{
    uint64_t sig;
    uint32_t sigExtra;
    union { uint64_t ui; int64_t i; } uZ;
    int64_t z;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    sig =
        (uint64_t) extSigPtr[indexWord( 3, 2 )]<<32
            | extSigPtr[indexWord( 3, 1 )];
    sigExtra = extSigPtr[indexWordLo( 3 )];
    if (
        (roundingMode == softfloat_round_near_maxMag)
            || (roundingMode == softfloat_round_near_even)
    ) {
        if ( 0x80000000 <= sigExtra ) goto increment;
    } else {
        if (
            sigExtra
                && (sign
                        ? (roundingMode == softfloat_round_min)
#ifdef SOFTFLOAT_ROUND_ODD
                              || (roundingMode == softfloat_round_odd)
#endif
                        : (roundingMode == softfloat_round_max))
        ) {
 increment:
            ++sig;
            if ( !sig ) goto invalid;
            if (
                (sigExtra == 0x80000000)
                    && (roundingMode == softfloat_round_near_even)
            ) {
                sig &= ~(uint_fast64_t) 1;
            }
        }
    }
    uZ.ui = sign ? -sig : sig;
    z = uZ.i;
    if ( z && ((z < 0) ^ sign) ) goto invalid;
    if ( sigExtra ) {
#ifdef SOFTFLOAT_ROUND_ODD
        if ( roundingMode == softfloat_round_odd ) z |= 1;
#endif
        if ( exact ) softfloat_exceptionFlags |= softfloat_flag_inexact;
    }
    return z;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 invalid:
    softfloat_raiseFlags( softfloat_flag_invalid );
    return sign ? i64_fromNegOverflow : i64_fromPosOverflow;

}
#endif // SOFTFLOAT_FAST_INT64


/*----------------------------------------------------------------------------
| Shifts the N-bit unsigned integer pointed to by 'aPtr' right by the number
| of bits given in 'dist', where N = 'size_words' * 32.  The value of 'dist'
| must be in the range 1 to 31.  If any nonzero bits are shifted off, they are
| "jammed" into the least-significant bit of the shifted value by setting the
| least-significant bit to 1.  This shifted-and-jammed N-bit result is stored
| at the location pointed to by 'zPtr'.  Each of 'aPtr' and 'zPtr' points
| to a 'size_words'-long array of 32-bit elements that concatenate in the
| platform's normal endian order to form an N-bit integer.
*----------------------------------------------------------------------------*/
inline void softfloat_shortShiftRightJamM(
     uint_fast8_t size_words,
     const uint32_t *aPtr,
     uint_fast8_t dist,
     uint32_t *zPtr
 )
{
    uint_fast8_t uNegDist;
    unsigned int index, lastIndex;
    uint32_t partWordZ, wordA;

    uNegDist = -dist;
    index = indexWordLo( size_words );
    lastIndex = indexWordHi( size_words );
    wordA = aPtr[index];
    partWordZ = wordA>>dist;
    if ( partWordZ<<dist != wordA ) partWordZ |= 1;
    while ( index != lastIndex ) {
        wordA = aPtr[index + wordIncr];
        zPtr[index] = wordA<<(uNegDist & 31) | partWordZ;
        index += wordIncr;
        partWordZ = wordA>>dist;
    }
    zPtr[index] = partWordZ;

}

inline
 void
  softfloat_shortShiftRightJamM(
      uint_fast8_t size_words,
      const uint64_t *aPtr,
      uint_fast8_t dist,
      uint64_t *zPtr
  )
{
    uint_fast8_t uNegDist;
    unsigned int index, lastIndex;
    uint64_t partWordZ, wordA;

    uNegDist = -dist;
    index = indexWordLo( size_words );
    lastIndex = indexWordHi( size_words );
    wordA = aPtr[index];
    partWordZ = wordA>>dist;
    if ( partWordZ<<dist != wordA ) partWordZ |= 1;
    while ( index != lastIndex ) {
        wordA = aPtr[index + wordIncr];
        zPtr[index] = wordA<<(uNegDist & 63) | partWordZ;
        index += wordIncr;
        partWordZ = wordA>>dist;
    }
    zPtr[index] = partWordZ;

}

} // namespace sixit::dmath::softfloat

#endif // sixit_dmath_3rdparty_softfloat_inline_softfloat_functions_h_included