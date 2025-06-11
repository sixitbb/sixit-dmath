/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */
/*
 * See comments in log10.c.
 */
/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Serhii Iliukhin
*/
#ifndef sixit_dmath_math_operations_log10_h_included
#define sixit_dmath_math_operations_log10_h_included

#include <sixit/dmath/traits.h>

#include <cmath>

namespace sixit::dmath::mathf
{
    template <typename fp>
    struct __log10_data {
        static constexpr fp ivln10hi  =  fp(float(4.3432617188e-01)); /* 0x3ede6000 */
        static constexpr fp ivln10lo  =  fp(float(-3.1689971365e-05)); /* 0xb804ead9 */
        static constexpr fp log10_2hi =  fp(float(3.0102920532e-01)); /* 0x3e9a2080 */
        static constexpr fp log10_2lo =  fp(float(7.9034151668e-07)); /* 0x355427db */
        /* |(log(1+s)-log(1-s))/s - Lg(s)| < 2**-34.24 (~[-4.95e-11, 4.97e-11]). */
        static constexpr fp Lg1 = fp(float(0xaaaaaa.0p-24)); /* 0.66666662693 */
        static constexpr fp Lg2 = fp(float(0xccce13.0p-25)); /* 0.40000972152 */
        static constexpr fp Lg3 = fp(float(0x91e9ee.0p-25)); /* 0.28498786688 */
        static constexpr fp Lg4 = fp(float(0xf89e26.0p-26)); /* 0.24279078841 */
    };

    template <typename fp>
    fp _log10(fp x)
    {
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC        
        if constexpr (std::is_same_v<float, fp>) {
            return std::log10(x);
        } else {
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC       
            fp uf = x;
            fp hfsq,f,s,z,R,w,t1,t2,dk,hi,lo;
            uint32_t ix;
            int k;

            ix = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(uf);
            k = 0;
            if (ix < 0x00800000 || ix>>31) {  /* x < 2**-126  */
                if (ix<<1 == 0)
                    return fp(-1.f) / (x * x);  /* log(+-0)=-inf */
                if (ix >> 31)
                    return sixit::dmath::fp_traits<fp>::bit_cast_from_ieee_uint32(0x7fc0'0000);// (x - x) / fp(0.0f); /* log(-#) = NaN */
                /* subnormal number, scale up x */
                k -= 25;
                x *= 0x1p25f;
                uf = x;
                ix = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(uf);
            } else if (ix >= 0x7f800000) {
                return x;
            } else if (ix == 0x3f800000)
                return 0;

            /* reduce x into [sqrt(2)/2, sqrt(2)] */
            ix += 0x3f800000 - 0x3f3504f3;
            k += (int)(ix>>23) - 0x7f;
            ix = (ix&0x007fffff) + 0x3f3504f3;
            uf = sixit::dmath::fp_traits<fp>::bit_cast_from_ieee_uint32(ix);
            x = uf;

            f = x - fp(1.0f);
            s = f / (fp(2.0f) + f);
            z = s*s;
            w = z*z;
            t1= w * (__log10_data<fp>::Lg2 + w * __log10_data<fp>::Lg4);
            t2= z * (__log10_data<fp>::Lg1 + w * __log10_data<fp>::Lg3);
            R = t2 + t1;
            hfsq = fp(0.5f) * f * f;

            hi = f - hfsq;
            ix = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(hi);
            ix &= 0xfffff000;
            hi = sixit::dmath::fp_traits<fp>::bit_cast_from_ieee_uint32(ix);
            lo = f - hi - hfsq + s*(hfsq+R);
            dk = fp(float(k));
            return dk * __log10_data<fp>::log10_2lo + (lo + hi) * __log10_data<fp>::ivln10lo + 
                lo * __log10_data<fp>::ivln10hi + hi * __log10_data<fp>::ivln10hi + dk * __log10_data<fp>::log10_2hi;
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC                
        }
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC               
    }

    template <typename fp>
    auto log10(fp x)
    {
        return _log10(sixit::dmath::fp_traits<fp>::to_fallback(x));
    }

    template <typename fp, sixit::units::physical_dimension dim_>
    inline sixit::units::dimensional_scalar<fp, dim_> log10(sixit::units::dimensional_scalar<fp, dim_> val)
    {
        static_assert(dim_ == dim_ * dim_);
        return sixit::units::dimensional_scalar<fp, dim_>({ log10<fp>(val.value), sixit::units::internal_constructor_of_dimensional_scalar_from_fp() });
    }
} //  sixit::dmath::mathf

#endif // sixit_dmath_math_operations_log10_h_included
/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Serhii Iliukhin

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
