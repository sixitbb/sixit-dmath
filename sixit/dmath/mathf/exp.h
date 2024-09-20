/*
 * Single-precision e^x function.
 *
 * Copyright (c) 2017-2018, Arm Limited.
 * SPDX-License-Identifier: MIT
 */
/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Serhii Iliukhin
*/
#ifndef sixit_dmath_math_operations_exp_h_included
#define sixit_dmath_math_operations_exp_h_included

#include "__utils.h"

namespace sixit::dmath::mathf
{
    /*ULP error: 0.502 (nearest rounding.)
    Relative error: 1.69 * 2^-34 in [-ln2/64, ln2/64] (before rounding.)
    Wrong count: 170635 (all nearest rounding wrong results with fma.)
    Non-nearest ULP error: 1 (rounded ULP error)
    */
    constexpr int EXP2F_TABLE_BITS = 5;
    constexpr int EXP2F_POLY_ORDER = 3;

    constexpr int N = 1 << EXP2F_TABLE_BITS;

    template <typename fp>
    struct exp_data {
        /* tab[i] = uint(2^(i/N)) - (i << 52-BITS)
            used for computing 2^(k/N) for an int |k| < 150 N as
            double(tab[k%N] + (k << 52-BITS)) */
        static constexpr uint64_t tab[1 << EXP2F_TABLE_BITS] = {
            0x3ff0000000000000, 0x3fefd9b0d3158574, 0x3fefb5586cf9890f, 0x3fef9301d0125b51,
            0x3fef72b83c7d517b, 0x3fef54873168b9aa, 0x3fef387a6e756238, 0x3fef1e9df51fdee1,
            0x3fef06fe0a31b715, 0x3feef1a7373aa9cb, 0x3feedea64c123422, 0x3feece086061892d,
            0x3feebfdad5362a27, 0x3feeb42b569d4f82, 0x3feeab07dd485429, 0x3feea47eb03a5585,
            0x3feea09e667f3bcd, 0x3fee9f75e8ec5f74, 0x3feea11473eb0187, 0x3feea589994cce13,
            0x3feeace5422aa0db, 0x3feeb737b0cdc5e5, 0x3feec49182a3f090, 0x3feed503b23e255d,
            0x3feee89f995ad3ad, 0x3feeff76f2fb5e47, 0x3fef199bdd85529c, 0x3fef3720dcef9069,
            0x3fef5818dcfba487, 0x3fef7c97337b9b5f, 0x3fefa4afa2a490da, 0x3fefd0765b6e4540,
        };

        static constexpr double shift_scaled = 0x1.8p+52 / N;
        static constexpr double poly[EXP2F_POLY_ORDER] = {
            0x1.c6af84b912394p-5, 0x1.ebfce50fac4f3p-3, 0x1.62e42ff0c52d6p-1,
        };
        static constexpr double shift = 0x1.8p+52;
        static constexpr double invln2_scaled = 0x1.71547652b82fep+0 * N;
        static constexpr fp poly_scaled[EXP2F_POLY_ORDER] = {fp(float(0x1.c6af84b912394p-5/N/N/N)), fp(float(0x1.ebfce50fac4f3p-3/N/N)), fp(float(0x1.62e42ff0c52d6p-1/N))}; // must_be_double
    };


    template <typename fp>
    fp _exp(fp x)
    {
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC        
        if constexpr (std::is_same_v<float, fp>) {
            return std::exp(x);
        } else {
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC                   
            
            uint32_t abstop;
            uint64_t ki, t;
            fp r, r2, y, s;  // must_be_double

            double xd = sixit::lwa::bit_cast<float>(sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(x));
            double z, kd;

            abstop = top12(x) & 0x7ff;
            if (predict_false(abstop >= top12(88.0f))) {
                /* |x| >= 88 or x is nan.  */
                if (sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(x) == 
                        sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(/*-INFINITY*/-std::numeric_limits<float>::infinity()))
                    return 0.0f;
                if (abstop >= top12(std::numeric_limits<float>::infinity()))
                    return x + x;
                if (x > fp(0x1.62e42ep6f)) /* x > log(0x1p128) ~= 88.72 */
                    return __math_oflowf<fp>(0);
                if (x < fp(-0x1.9fe368p6f)) /* x < log(0x1p-150) ~= -103.97 */
                    return __math_uflowf<fp>(0);
            }

            /* x*N/Ln2 = k + r with r in [-1/2, 1/2] and int k.  */
            z = exp_data<fp>::invln2_scaled * xd;

            /* Round and convert z to int, the result is in [-150*N, 128*N] and
            ideally ties-to-even rule is used, otherwise the magnitude of r
            can be bigger which gives larger approximation error.  */
            kd = eval_as_double(z + exp_data<fp>::shift);
            ki = sixit::dmath::mathf::asuint64(kd);
            kd -= exp_data<fp>::shift;
            r = fp(float(z - kd));

            /* exp(x) = 2^(k/N) * 2^(r/N) ~= s * (C0*r^3 + C1*r^2 + C2*r + 1) */
            t = exp_data<fp>::tab[ki % N];
            t += ki << (52 - EXP2F_TABLE_BITS);
            s = fp(float(sixit::dmath::mathf::asdouble<fp>(t)));
            fp z1 = exp_data<fp>::poly_scaled[0] * r + exp_data<fp>::poly_scaled[1];
            r2 = r * r;
            y = exp_data<fp>::poly_scaled[2] * r + fp(1.f);
            y = z1 * r2 + y;
            y = y * s;
            return eval_as_fp(y);
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC
        }
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC               
    }

    template <typename fp>
    auto exp(fp x)
    {
        return _exp(sixit::dmath::fp_traits<fp>::to_fallback(x));
    }

    template <typename fp, sixit::units::physical_dimension dim_>
    inline sixit::units::dimensional_scalar<fp, dim_> exp(sixit::units::dimensional_scalar<fp, dim_> val)
    {
        static_assert(dim_ == dim_ * dim_);
        return sixit::units::dimensional_scalar<fp, dim_>({ exp<fp>(val.value), sixit::units::internal_constructor_of_dimensional_scalar_from_fp() });
    }
} //  sixit::dmath::mathf

#endif // sixit_dmath_math_operations_exp_h_included
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