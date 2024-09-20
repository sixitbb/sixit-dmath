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
#ifndef sixit_dmath_math_operations_sin_h_included
#define sixit_dmath_math_operations_sin_h_included

#include "__utils.h"
#include "__sindf.h"
#include "__cosdf.h"
#include "__rem_pio2f.h"

namespace sixit::dmath::mathf
{
    template <typename fp>
    struct __sin_data
    {
        static constexpr fp s1pio2 = fp(float(1.0 * PI2)); /* 0x3FF921FB, 0x54442D18 */ // must_be_double
        static constexpr fp s2pio2 = fp(float(2.0 * PI2)); /* 0x400921FB, 0x54442D18 */ // must_be_double
        static constexpr fp s3pio2 = fp(float(3.0 * PI2)); /* 0x4012D97C, 0x7F3321D2 */ // must_be_double
        static constexpr fp s4pio2 = fp(float(4.0 * PI2)); /* 0x401921FB, 0x54442D18 */ // must_be_double
    };
    
    template <typename fp>
    fp _sin(fp x)
    {
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC
        if constexpr (std::is_same_v<float, fp>)
        {
            return std::sin(x);
        } else {
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC                   
            fp y; // must_be_double
            uint32_t ix;
            int n, sign;

            ix = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(x);
            sign = ix >> 31;
            ix &= 0x7fffffff;

            if (ix <= 0x3f490fda) {  /* |x| ~<= pi/4 */
                if (ix < 0x39800000) {  /* |x| < 2**-12 */
                    /* raise inexact if x!=0 and underflow if subnormal */
                    if constexpr (std::is_same_v<fp, float>) 
                        force_eval_fp(ix < 0x00800000 ? x/0x1p120f : x+0x1p120f);
                    return x;
                }
                return __sindf(x);
            }
            if (ix <= 0x407b53d1) {  /* |x| ~<= 5*pi/4 */
                if (ix <= 0x4016cbe3) {  /* |x| ~<= 3pi/4 */
                    if (sign)
                        return -__cosdf(x + __sin_data<fp>::s1pio2);
                    else
                        return __cosdf(x - __sin_data<fp>::s1pio2);
                }
                return __sindf(sign ? -(x + __sin_data<fp>::s2pio2) : -(x - __sin_data<fp>::s2pio2));
            }
            if (ix <= 0x40e231d5) {  /* |x| ~<= 9*pi/4 */
                if (ix <= 0x40afeddf) {  /* |x| ~<= 7*pi/4 */
                    if (sign)
                        return __cosdf(x + __sin_data<fp>::s3pio2);
                    else
                        return -__cosdf(x - __sin_data<fp>::s3pio2);
                }
                return __sindf(sign ? x + __sin_data<fp>::s4pio2 : x - __sin_data<fp>::s4pio2);
            }

            /* sin(Inf or NaN) is NaN */
            if (ix >= 0x7f800000)
                return x - x;

            /* general argument reduction needed */
            n = __rem_pio2f(x, &y);
            switch (n&3) {
            case 0: return  __sindf(y);
            case 1: return  __cosdf(y);
            case 2: return  __sindf(-y);
            default:
                return -__cosdf(y);
            }
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC            
        }
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC               
    }

    template <typename fp>
    auto sin(fp x)
    {
        return _sin(sixit::dmath::fp_traits<fp>::to_fallback(x));
    }

    template <typename fp, sixit::units::physical_dimension dim_>
    inline sixit::units::dimensional_scalar<fp, dim_> sin(sixit::units::dimensional_scalar<fp, dim_> val)
    {
        static_assert(dim_ == dim_ * dim_);
        return sixit::units::dimensional_scalar<fp, dim_>({ sin<fp>(val.value), sixit::units::internal_constructor_of_dimensional_scalar_from_fp() });
    }
} //  sixit::dmath::mathf

#endif // sixit_dmath_math_operations_sin_h_included
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