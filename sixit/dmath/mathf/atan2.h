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
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Serhii Iliukhin
*/

#ifndef sixit_dmath_mathf_atan2_h_included
#define sixit_dmath_mathf_atan2_h_included

#include "__utils.h"

namespace sixit::dmath::mathf
{
    template <typename fp>
    struct __atan2_data {
        static constexpr fp pi     = fp(float(3.1415927410e+00)); /* 0x40490fdb */
        static constexpr fp pi_lo  = fp(float(-8.7422776573e-08)); /* 0xb3bbbd2e */
    };
    
    template <typename fp>
    fp _atan2(fp y, fp x)
    {
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC        
        if constexpr (std::is_same_v<float, fp>) {
            return std::atan2(y, x);
        } else {
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC                   
            fp z;
            uint32_t m,ix,iy;

            if (sixit::dmath::fp_traits<fp>::isnan(x) || 
                sixit::dmath::fp_traits<fp>::isnan(y))
                return x + y;
            ix = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(x);
            iy = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(y);
            if (ix == 0x3f800000)  /* x=1.0 */
                return atan(y);
            
            m = ((iy>>31)&1) | ((ix>>30)&2);  /* 2*sign(x)+sign(y) */
            ix &= 0x7fffffff;
            iy &= 0x7fffffff;

            /* when y = 0 */
            if (iy == 0) {
                switch (m) {
                case 0:
                case 1: return y;   /* atan(+-0,+anything)=+-0 */
                case 2: return  __atan2_data<fp>::pi; /* atan(+0,-anything) = pi */
                case 3: return -__atan2_data<fp>::pi; /* atan(-0,-anything) =-pi */
                }
            }
            /* when x = 0 */
            if (ix == 0)
                return m&1 ? - __atan2_data<fp>::pi / fp(2.f) : __atan2_data<fp>::pi / fp(2.f);
            /* when x is INF */
            if (ix == 0x7f800000) {
                if (iy == 0x7f800000) {
                    switch (m) {
                    case 0: return  __atan2_data<fp>::pi / fp(4.f); /* atan(+INF,+INF) */
                    case 1: return -__atan2_data<fp>::pi / fp(4.f); /* atan(-INF,+INF) */
                    case 2: return fp(3.f) * __atan2_data<fp>::pi / fp(4.f);  /*atan(+INF,-INF)*/
                    case 3: return fp(-3.f) * __atan2_data<fp>::pi / fp(4.f); /*atan(-INF,-INF)*/
                    }
                } else {
                    switch (m) {
                    case 0: return fp(0.0f);    /* atan(+...,+INF) */
                    case 1: return fp(-0.0f);    /* atan(-...,+INF) */
                    case 2: return  __atan2_data<fp>::pi; /* atan(+...,-INF) */
                    case 3: return -__atan2_data<fp>::pi; /* atan(-...,-INF) */
                    }
                }
            }
            /* |y/x| > 0x1p26 */
            if (ix+(26<<23) < iy || iy == 0x7f800000)
                return m&1 ? -__atan2_data<fp>::pi / fp(2.f) : __atan2_data<fp>::pi / fp(2.f);

            /* z = atan(|y/x|) with correct underflow */
            if ((m&2) && iy+(26<<23) < ix)  /*|y/x| < 0x1p-26, x < 0 */
                z = 0.0;
            else
                z = atan(abs(y / x));
            switch (m) {
            case 0: return z;              /* atan(+,+) */
            case 1: return -z;             /* atan(-,+) */
            case 2: return __atan2_data<fp>::pi - (z - __atan2_data<fp>::pi_lo); /* atan(+,-) */
            default: /* case 3 */
                return (z - __atan2_data<fp>::pi_lo) - __atan2_data<fp>::pi; /* atan(-,-) */
            }
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC            
        }
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC               
    }

    template <typename fp>
    auto atan2(fp y, fp x)
    {
        return _atan2(sixit::dmath::fp_traits<fp>::to_fallback(y), sixit::dmath::fp_traits<fp>::to_fallback(x));
    }

    template <typename fp, sixit::units::physical_dimension dim_>
    inline sixit::units::dimensional_scalar<fp, dim_ / dim_> atan2(sixit::units::dimensional_scalar<fp, dim_> a, sixit::units::dimensional_scalar<fp, dim_> b)
    {
        return sixit::units::dimensional_scalar<fp, dim_ / dim_>({ atan2<fp>(a.value, b.value), sixit::units::internal_constructor_of_dimensional_scalar_from_fp() });
    }
} //  sixit::dmath::mathf

#endif //sixit_dmath_mathf_atan2_h_included

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