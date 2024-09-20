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

#ifndef sixit_dmath_mathf_asin_h_included
#define sixit_dmath_mathf_asin_h_included

#include "__utils.h"
#include "abs.h"

namespace sixit::dmath::mathf
{
    template <typename fp>
    struct __asin_data
    {
        static constexpr fp pio2 = fp(float(1.570796326794896558e+00)); // must_be_double

        static constexpr fp pS0 = fp(float( 1.6666586697e-01));
        static constexpr fp pS1 = fp(float(-4.2743422091e-02));
        static constexpr fp pS2 = fp(float(-8.6563630030e-03));
        static constexpr fp qS1 = fp(float(-7.0662963390e-01));

        static fp R(fp z)
        {
            fp p = z * (__asin_data<fp>::pS0+z * (__asin_data<fp>::pS1 + z * __asin_data<fp>::pS2));
            fp q = fp(1.0f) + z * __asin_data<fp>::qS1;
            return p / q;
        }
    };

    template <typename fp>
    fp _asin(fp x)
    {
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC        
        if constexpr (std::is_same_v<float, fp>) {
            return std::asin(x);
        } else {
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC                   
            fp s;               // must_be_double

            uint32_t hx = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(x);
            uint32_t ix = hx & 0x7fffffff;
            if (ix >= 0x3f800000) {  /* |x| >= 1 */
                if (ix == 0x3f800000)  /* |x| == 1 */
                    return x * __asin_data<fp>::pio2 + fp(0x1p-120f);  /* asin(+-1) = +-pi/2 with inexact */
                return fp(0.f) / (x - x);  /* asin(|x|>1) is NaN */
            }
            if (ix < 0x3f000000) {  /* |x| < 0.5 */
                /* if 0x1p-126 <= |x| < 0x1p-12, avoid raising underflow */
                if (ix < 0x39800000 && ix >= 0x00800000)
                    return x;
                return x + x * __asin_data<fp>::R(x * x);
            }
            /* 1 > |x| >= 0.5 */
            fp z = (fp(1.f) - abs(x)) * fp(0.5f);
            s = sqrt(z);
            x = __asin_data<fp>::pio2 - fp(2.f) * (s + s * __asin_data<fp>::R(z));
            if (hx >> 31)
                return -x;
            return x;
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC            
        }
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC               
    }

    template <typename fp>
    auto asin(fp x)
    {
        return _asin(sixit::dmath::fp_traits<fp>::to_fallback(x));
    }

    template <typename fp, sixit::units::physical_dimension dim_>
    inline sixit::units::dimensional_scalar<fp, dim_> asin(sixit::units::dimensional_scalar<fp, dim_> val)
    {
        static_assert(dim_ == dim_ * dim_);
        return sixit::units::dimensional_scalar<fp, dim_>({ asin<fp>(val.value), sixit::units::internal_constructor_of_dimensional_scalar_from_fp() });
    }
} //  sixit::dmath::mathf

#endif //sixit_dmath_mathf_asin_h_included

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