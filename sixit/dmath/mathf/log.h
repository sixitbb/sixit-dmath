/*
 * Single-precision log function.
 *
 * Copyright (c) 2017-2018, Arm Limited.
 * SPDX-License-Identifier: MIT
 */
/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Serhii Iliukhin
*/
#ifndef sixit_dmath_math_operations_log_h_included
#define sixit_dmath_math_operations_log_h_included

#include <sixit/dmath/traits.h>
#include "__utils.h"

#include <cmath>

namespace sixit::dmath::mathf
{
    template <typename fp>   
    struct __log_data {
        static constexpr fp tab[][2] = {            // must_be_double
        { fp(float(0x1.661ec79f8f3bep+0)), fp(float(-0x1.57bf7808caadep-2)) },
        { fp(float(0x1.571ed4aaf883dp+0)), fp(float(-0x1.2bef0a7c06ddbp-2)) },
        { fp(float(0x1.49539f0f010bp+0)), fp(float(-0x1.01eae7f513a67p-2)) },
        { fp(float(0x1.3c995b0b80385p+0)), fp(float(-0x1.b31d8a68224e9p-3)) },
        { fp(float(0x1.30d190c8864a5p+0)), fp(float(-0x1.6574f0ac07758p-3)) },
        { fp(float(0x1.25e227b0b8eap+0)), fp(float(-0x1.1aa2bc79c81p-3)) },
        { fp(float(0x1.1bb4a4a1a343fp+0)), fp(float(-0x1.a4e76ce8c0e5ep-4)) },
        { fp(float(0x1.12358f08ae5bap+0)), fp(float(-0x1.1973c5a611cccp-4)) },
        { fp(float(0x1.0953f419900a7p+0)), fp(float(-0x1.252f438e10c1ep-5)) },
        { fp(float(0x1p+0)), fp(float(0x0p+0)) },
        { fp(float(0x1.e608cfd9a47acp-1)), fp(float(0x1.aa5aa5df25984p-5)) },
        { fp(float(0x1.ca4b31f026aap-1)), fp(float(0x1.c5e53aa362eb4p-4)) },
        { fp(float(0x1.b2036576afce6p-1)), fp(float(0x1.526e57720db08p-3)) },
        { fp(float(0x1.9c2d163a1aa2dp-1)), fp(float(0x1.bc2860d22477p-3)) },
        { fp(float(0x1.886e6037841edp-1)), fp(float(0x1.1058bc8a07ee1p-2)) },
        { fp(float(0x1.767dcf5534862p-1)), fp(float(0x1.4043057b6ee09p-2)) },
        };
        static constexpr fp ln2 = fp(float(0x1.62e42fefa39efp-1)); // must_be_double
        static constexpr fp poly[3] = {fp(float(- 0x1.00ea348b88334p-2)), fp(float(0x1.5575b0be00b6ap-2)), fp(float(- 0x1.ffffef20a4123p-2))}; // must_be_double
    };

    constexpr int LOGF_TABLE_BITS = 4;
    constexpr int LOGF_POLY_ORDER = 4;

    template <typename fp>
    fp _log(fp x)
    {
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC        
        if constexpr (std::is_same_v<float, fp>) {
            return std::log(x);
        } else {
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC                   
            fp z, r, r2, y, y0, invc, logc;       // must_be_double
            uint32_t ix, iz, tmp;
            int k, i;

            ix = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(x);
            /* Fix sign of zero with downward rounding when x==1.  */
            if (predict_false(ix == 0x3f800000))
                return 0;
            if (predict_false(ix - 0x00800000 >= 0x7f800000 - 0x00800000)) {
                /* x < 0x1p-126 or inf or nan.  */
                if (ix * 2 == 0)
                    return __math_divzerof<fp>(1);
                if (ix == 0x7f800000) /* log(inf) == inf.  */
                    return x;
                if ((ix & 0x80000000) || ix * 2 >= 0xff000000)
                    return (x - x) / (x - x);
                /* x is subnormal, normalize it.  */
                ix = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(x * fp(0x1p23f));
                ix -= 23 << 23;
            }

            /* x = 2^k z; where z is in range [OFF,2*OFF] and exact.
            The range is split into N subintervals.
            The ith subinterval contains z and c is near its center.  */
            tmp = ix - 0x3f330000;
            i = (tmp >> (23 - LOGF_TABLE_BITS)) % (1 << LOGF_TABLE_BITS);
            k = (int32_t)tmp >> 23; /* arithmetic shift */
            iz = ix - (tmp & 0xff800000);
            invc = __log_data<fp>::tab[i][0];
            logc = __log_data<fp>::tab[i][1];
            z = /*(double_t) must_be_double*/ sixit::dmath::fp_traits<fp>::bit_cast_from_ieee_uint32(iz);

            /* log(x) = log1p(z/c-1) + log(c) + k*Ln2 */
            r = z * invc - fp(1.f);
            y0 = logc + /*(double_t) must_be_double*/ fp(float(k)) * __log_data<fp>::ln2;

            /* Pipelined polynomial evaluation to approximate log1p(r).  */
            r2 = r * r;
            y = __log_data<fp>::poly[1] * r + __log_data<fp>::poly[2];
            y = __log_data<fp>::poly[0] * r2 + y;
            y = y * r2 + (y0 + r); 
            return eval_as_fp(y);
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC            
        }
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC               
    }

    template <typename fp>
    auto log(fp x)
    {
        return _log(sixit::dmath::fp_traits<fp>::to_fallback(x));
    }

    template <typename fp, sixit::units::physical_dimension dim_>
    inline sixit::units::dimensional_scalar<fp, dim_> log(sixit::units::dimensional_scalar<fp, dim_> val)
    {
        static_assert(dim_ == dim_ * dim_);
        return sixit::units::dimensional_scalar<fp, dim_>({ log<fp>(val.value), sixit::units::internal_constructor_of_dimensional_scalar_from_fp() });
    }
} //  sixit::dmath::mathf

#endif // sixit_dmath_math_operations_log_h_included
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
