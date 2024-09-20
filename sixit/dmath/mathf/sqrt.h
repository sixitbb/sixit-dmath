/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Serhii Iliukhin
*/

#ifndef sixit_dmath_mathf_sqrt_h_included
#define sixit_dmath_mathf_sqrt_h_included

#include <cstdint>
#include "__utils.h"
#include "iostream"

namespace sixit::dmath::mathf
{
    const uint16_t __rsqrt_tab[128] = {
        0xb451,0xb2f0,0xb196,0xb044,0xaef9,0xadb6,0xac79,0xab43,
        0xaa14,0xa8eb,0xa7c8,0xa6aa,0xa592,0xa480,0xa373,0xa26b,
        0xa168,0xa06a,0x9f70,0x9e7b,0x9d8a,0x9c9d,0x9bb5,0x9ad1,
        0x99f0,0x9913,0x983a,0x9765,0x9693,0x95c4,0x94f8,0x9430,
        0x936b,0x92a9,0x91ea,0x912e,0x9075,0x8fbe,0x8f0a,0x8e59,
        0x8daa,0x8cfe,0x8c54,0x8bac,0x8b07,0x8a64,0x89c4,0x8925,
        0x8889,0x87ee,0x8756,0x86c0,0x862b,0x8599,0x8508,0x8479,
        0x83ec,0x8361,0x82d8,0x8250,0x81c9,0x8145,0x80c2,0x8040,
        0xff02,0xfd0e,0xfb25,0xf947,0xf773,0xf5aa,0xf3ea,0xf234,
        0xf087,0xeee3,0xed47,0xebb3,0xea27,0xe8a3,0xe727,0xe5b2,
        0xe443,0xe2dc,0xe17a,0xe020,0xdecb,0xdd7d,0xdc34,0xdaf1,
        0xd9b3,0xd87b,0xd748,0xd61a,0xd4f1,0xd3cd,0xd2ad,0xd192,
        0xd07b,0xcf69,0xce5b,0xcd51,0xcc4a,0xcb48,0xca4a,0xc94f,
        0xc858,0xc764,0xc674,0xc587,0xc49d,0xc3b7,0xc2d4,0xc1f4,
        0xc116,0xc03c,0xbf65,0xbe90,0xbdbe,0xbcef,0xbc23,0xbb59,
        0xba91,0xb9cc,0xb90a,0xb84a,0xb78c,0xb6d0,0xb617,0xb560,
    };
        
    template <typename fp>
    inline fp _sqrt(fp x)
    {
        // if constexpr (std::is_same_v<float, fp>)
        //     return std::sqrt(x);
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC
        if constexpr (std::is_same_v<fp, float>) {
            volatile uint32_t rvu = sixit::lwa::bit_cast<uint32_t>(x);
            // rvu += 1 << 31;
            // rvu += 1 << 31;
            volatile float rvf = std::sqrt(sixit::lwa::bit_cast<float>(rvu));
            return rvf;
        } else {
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC                   

            uint32_t ix, m, m1, m0, even, ey;

            auto mul32 = [](uint32_t a, uint32_t b)
            {
                return uint32_t(((uint64_t)a * b) >> 32);
            };

            ix = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(x);
            if (predict_false(ix - 0x00800000 >= 0x7f800000 - 0x00800000)) {
                /* x < 0x1p-126 or inf or nan.  */
                if (ix * 2 == 0)
                    return x;
                if (ix == 0x7f800000)
                    return x;
                if (ix > 0x7f800000)
                    return (x - x) / (x - x);
                /* x is subnormal, normalize it.  */
                ix = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(x * fp(0x1p23f));
                ix -= 23 << 23;
            }

            /* x = 4^e m; with int e and m in [1, 4).  */
            even = ix & 0x00800000;
            m1 = (ix << 8) | 0x80000000;
            m0 = (ix << 7) & 0x7fffffff;
            m = even ? m0 : m1;

            /* 2^e is the exponent part of the return value.  */
            ey = ix >> 1;
            ey += 0x3f800000 >> 1;
            ey &= 0x7f800000;

            /* compute r ~ 1/sqrt(m), s ~ sqrt(m) with 2 goldschmidt iterations.  */
            static const uint32_t three = 0xc0000000;
            uint32_t r, s, d, u, i;
            i = (ix >> 17) % 128;
            r = (uint32_t)__rsqrt_tab[i] << 16;
            /* |r*sqrt(m) - 1| < 0x1p-8 */
            s = mul32(m, r);
            /* |s/sqrt(m) - 1| < 0x1p-8 */
            d = mul32(s, r);
            u = three - d;
            r = mul32(r, u) << 1;
            /* |r*sqrt(m) - 1| < 0x1.7bp-16 */
            s = mul32(s, u) << 1;
            /* |s/sqrt(m) - 1| < 0x1.7bp-16 */
            d = mul32(s, r);
            u = three - d;
            s = mul32(s, u);
            /* -0x1.03p-28 < s/sqrt(m) - 1 < 0x1.fp-31 */
            s = (s - 1)>>6;
            /* s < sqrt(m) < s + 0x1.08p-23 */

            /* compute nearest rounded result.  */
            uint32_t d0, d1, d2;
            fp y, t;
            d0 = (m << 16) - s*s;
            d1 = s - d0;
            d2 = d1 + s + 1;
            s += d1 >> 31;
            s &= 0x007fffff;
            s |= ey;
            y = sixit::dmath::fp_traits<fp>::bit_cast_from_ieee_uint32(s);
            /* handle rounding and inexact exception. */
            uint32_t tiny = predict_false(d2==0) ? 0 : 0x01000000;
            tiny |= (d1^d2) & 0x80000000;
            t = sixit::dmath::fp_traits<fp>::bit_cast_from_ieee_uint32(tiny);
            y = eval_as_fp(y + t);

            return y;
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC            
        }
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC               
    }

    template <typename fp>
    inline auto sqrt(fp x)
    {
        return _sqrt(sixit::dmath::fp_traits<fp>::to_fallback(x));
    }

    template <typename fp, sixit::units::physical_dimension dim_>
    inline auto sqrt(sixit::units::dimensional_scalar<fp, dim_> val)
    {
        return sixit::units::dimensional_scalar<fp, sixit::units::sqrt<dim_>()>({ sqrt<fp>(val.value), sixit::units::internal_constructor_of_dimensional_scalar_from_fp() });
    }
} //  sixit::dmath::mathf

#endif //sixit_dmath_mathf_sqrt_h_included

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