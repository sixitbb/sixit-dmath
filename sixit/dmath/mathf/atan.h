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

#ifndef sixit_dmath_mathf_atan_h_included
#define sixit_dmath_mathf_atan_h_included

#include "__utils.h"

namespace sixit::dmath::mathf
{
    template <typename fp>
    struct __atan_data { 
        static constexpr fp atanhi[] = {
            fp(float(4.6364760399e-01)), /* atan(0.5)hi 0x3eed6338 */
            fp(float(7.8539812565e-01)), /* atan(1.0)hi 0x3f490fda */
            fp(float(9.8279368877e-01)), /* atan(1.5)hi 0x3f7b985e */
            fp(float(1.5707962513e+00)), /* atan(inf)hi 0x3fc90fda */
        };

        static constexpr fp atanlo[] = {
            fp(float(5.0121582440e-09)), /* atan(0.5)lo 0x31ac3769 */
            fp(float(3.7748947079e-08)), /* atan(1.0)lo 0x33222168 */
            fp(float(3.4473217170e-08)), /* atan(1.5)lo 0x33140fb4 */
            fp(float(7.5497894159e-08)), /* atan(inf)lo 0x33a22168 */
        };

        static constexpr fp aT[] = {
            fp(float(3.3333328366e-01)),
            fp(float(-1.9999158382e-01)),
            fp(float(1.4253635705e-01)),
            fp(float(-1.0648017377e-01)),
            fp(float(6.1687607318e-02)),
        };
    };

    template <typename fp>
    fp _atan(fp x)
    {
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC        
        if constexpr (std::is_same_v<float, fp>) {
            return std::atan(x);
        } else {
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC                   
            fp w,s1,s2,z;
            uint32_t ix,sign;
            int id;

            ix = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(x);
            sign = ix>>31;
            ix &= 0x7fffffff;
            if (ix >= 0x4c800000) {  /* if |x| >= 2**26 */
                if (sixit::dmath::fp_traits<fp>::isnan(x))
                    return x;
                z = __atan_data<fp>::atanhi[3] + fp(0x1p-120f);
                return sign ? -z : z;
            }
            if (ix < 0x3ee00000) {   /* |x| < 0.4375 */
                if (ix < 0x39800000) {  /* |x| < 2**-12 */
                    if (ix < 0x00800000) {
                        /* raise underflow for subnormal x */
                        if constexpr (std::is_same_v<fp, float>)
                            force_eval_fp(x * x);
                    }
                    return x;
                }
                id = -1;
            } else {
                x = abs(x);
                if (ix < 0x3f980000) {  /* |x| < 1.1875 */
                    if (ix < 0x3f300000) {  /*  7/16 <= |x| < 11/16 */
                        id = 0;
                        x = (fp(2.f) * x - fp(1.f)) / (fp(2.f) + x);
                    } else {                /* 11/16 <= |x| < 19/16 */
                        id = 1;
                        x = (x - fp(1.0f)) / (x + fp(1.0f));
                    }
                } else {
                    if (ix < 0x401c0000) {  /* |x| < 2.4375 */
                        id = 2;
                        x = (x - fp(1.5f)) / (fp(1.0f) + fp(1.5f) * x);
                    } else {                /* 2.4375 <= |x| < 2**26 */
                        id = 3;
                        x = fp(-1.0f) / x;
                    }
                }
            }
            /* end of argument reduction */
            z = x*x;
            w = z*z;
            /* break sum from i=0 to 10 aT[i]z**(i+1) into odd and even poly */
            s1 = z*(__atan_data<fp>::aT[0] + w * (__atan_data<fp>::aT[2] + w * __atan_data<fp>::aT[4]));
            s2 = w*(__atan_data<fp>::aT[1] + w * __atan_data<fp>::aT[3]);
            if (id < 0)
                return x - x * (s1 + s2);
            z = __atan_data<fp>::atanhi[id] - ((x * (s1 + s2) - __atan_data<fp>::atanlo[id]) - x);
            return sign ? -z : z;
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC            
        }
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC               
    }

    template <typename fp>
    auto atan(fp x)
    {
        return _atan(sixit::dmath::fp_traits<fp>::to_fallback(x));
    }

    template <typename fp, sixit::units::physical_dimension dim_>
    inline sixit::units::dimensional_scalar<fp, dim_> atan(sixit::units::dimensional_scalar<fp, dim_> val)
    {
        static_assert(dim_ == dim_ * dim_);
        return sixit::units::dimensional_scalar<fp, dim_>({ atan<fp>(val.value), sixit::units::internal_constructor_of_dimensional_scalar_from_fp() });
    }
} //  sixit::dmath::mathf

#endif //sixit_dmath_mathf_atan_h_included

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