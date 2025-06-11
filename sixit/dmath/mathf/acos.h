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
#ifndef sixit_dmath_math_operations_acos_h_included
#define sixit_dmath_math_operations_acos_h_included

#include "sqrt.h"
#include "__utils.h"

namespace sixit::dmath::mathf
{


    template <typename fp>
    struct __acos_data 
    {
        static constexpr fp pio2_hi = fp(float(1.5707962513e+00)); /* 0x3fc90fda */
        static constexpr fp pio2_lo = fp(float(7.5497894159e-08)); /* 0x33a22168 */
        static constexpr fp pS0 = fp(float( 1.6666586697e-01));
        static constexpr fp pS1 = fp(float(-4.2743422091e-02));
        static constexpr fp pS2 = fp(float(-8.6563630030e-03));
        static constexpr fp qS1 = fp(float(-7.0662963390e-01));

        static fp R(fp z)
        {
            fp p = z * (__acos_data<fp>::pS0 + z * (__acos_data<fp>::pS1 + z * __acos_data<fp>::pS2));
            fp q = fp(1.0f) + z * __acos_data<fp>::qS1;
            return p/q;
        }
    };

    template <typename fp>
    fp _acos(fp x)
    {
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC        
        if constexpr (std::is_same_v<float, fp>) {
            return std::acos(x);
        } else {
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC                   
            fp z,w,s,c,df;
            uint32_t hx = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(x);
            uint32_t ix;

            ix = hx & 0x7fffffff;
            /* |x| >= 1 or nan */
            if (ix >= 0x3f800000) {
                if (ix == 0x3f800000) {
                    if (hx >> 31)
                        return fp(2.f) * __acos_data<fp>::pio2_hi + fp(0x1p-120f);
                    return 0;
                }
                return fp(0.f) / (x - x);
            }
            /* |x| < 0.5 */
            if (ix < 0x3f000000) {
                if (ix <= 0x32800000) /* |x| < 2**-26 */
                    return __acos_data<fp>::pio2_hi + fp(0x1p-120f);
                return __acos_data<fp>::pio2_hi - (x - (__acos_data<fp>::pio2_lo-x * __acos_data<fp>::R(x*x)));
            }
            /* x < -0.5 */
            if (hx >> 31) {
                z = (fp(1.f) + x) * fp(0.5f);
                s = sqrt(z);
                w = __acos_data<fp>::R(z) * s- __acos_data<fp>::pio2_lo;
                return fp(2.f) * (__acos_data<fp>::pio2_hi - (s + w));
            }
            /* x > 0.5 */
            z = (fp(1.f) - x) * fp(0.5f);
            s = sqrt(z);
            hx = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(s);
            df = sixit::dmath::fp_traits<fp>::bit_cast_from_ieee_uint32(hx & 0xfffff000);
            c = (z -df * df) / (s + df);
            w = __acos_data<fp>::R(z) * s + c;
            return fp(2.f) * (df + w);
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC            
        }
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC               
    }

    template <typename fp>
    auto acos(fp x)
    {
        return _acos(sixit::dmath::fp_traits<fp>::to_fallback(x));
    }

    template <typename fp, sixit::units::physical_dimension dim_>
    inline sixit::units::dimensional_scalar<fp, dim_> acos(sixit::units::dimensional_scalar<fp, dim_> val)
    {
        static_assert(dim_ == dim_ * dim_);
        return sixit::units::dimensional_scalar<fp, dim_>({ acos<fp>(val.value), sixit::units::internal_constructor_of_dimensional_scalar_from_fp() });
    }
} //  sixit::dmath::mathf

#endif // sixit_dmath_math_operations_acos_h_included
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
