/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Serhii Iliukhin
*/
#ifndef sixit_dmath_math_operations_fmod_h_included
#define sixit_dmath_math_operations_fmod_h_included

#include "__utils.h"

namespace sixit::dmath::mathf
{
    template <typename fp> 
    inline fp _fmod(fp val, fp max) 
    {
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC        
        if constexpr (std::is_same_v<float, fp>) {
            return std::fmod(val, max);
        } else {
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC       
            int eval = sixit::dmath::fp_traits<fp>::get_exp(val) + 0x7f;
            int emax = sixit::dmath::fp_traits<fp>::get_exp(max) + 0x7f;
            uint32_t sval = sixit::dmath::fp_traits<fp>::get_sign(val);
            uint32_t i;
            uint32_t vali = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(val);
            uint32_t maxi = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(max);

            if (sixit::dmath::fp_traits<fp>::equal_to_zero(max) || 
                sixit::dmath::fp_traits<fp>::isnan(max) || 
                sixit::dmath::fp_traits<fp>::isinf(val))
                return (val * max) / (val * max);
            if (vali << 1 <= maxi << 1) {
                if (vali << 1 == maxi << 1)
                    return fp(0.f) * val;
                return val;
            }

            /* normalize x and y */
            if (!eval) {
                for (i = vali << 9 ; i >> 31 == 0; eval--, i <<= 1);
                vali <<= -eval + 1;
            } else {
                vali &= 0xffffffff >> 9;
                vali |= 1U << 23;
            }
            if (!emax) {
                for (i = maxi << 9; i >> 31 == 0; emax--, i <<= 1);
                maxi <<= -emax + 1;
            } else {
                maxi &= 0xffffffff >> 9;
                maxi |= 1U << 23;
            }

            /* x mod y */
            for (; eval > emax; eval--) {
                i = vali - maxi;
                if (i >> 31 == 0) {
                    if (i == 0)
                        return fp(0.f) * val;
                    vali = i;
                }
                vali <<= 1;
            }
            i = vali - maxi;
            if (i >> 31 == 0) {
                if (i == 0)
                    return fp(0.f) * val;
                vali = i;
            }
            for (; vali >> 23 == 0; vali <<= 1, eval--);

            /* scale result up */
            if (eval > 0) {
                vali -= 1U << 23;
                vali |= (uint32_t) eval << 23;
            } else {
                vali >>= -eval + 1;
            }
            val = sixit::dmath::fp_traits<fp>::bit_cast_from_ieee_uint32(vali);
            return sval ? -val : val;
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC            
        }
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC               
    }

    template <typename fp>
    inline auto fmod(fp val, fp max)
    {
        return _fmod(sixit::dmath::fp_traits<fp>::to_fallback(val), sixit::dmath::fp_traits<fp>::to_fallback(max));
    }

    template <typename fp, sixit::units::physical_dimension dim_>
    inline sixit::units::dimensional_scalar<fp, dim_> fmod(sixit::units::dimensional_scalar<fp, dim_> val, sixit::units::dimensional_scalar<fp, dim_> max)
    {
        static_assert(dim_ == dim_);
        return sixit::units::dimensional_scalar<fp, dim_>({ fmod<fp>(val.value, max.value), sixit::units::internal_constructor_of_dimensional_scalar_from_fp() });
    }
} //  sixit::dmath::mathf

#endif // sixit_dmath_math_operations_fmod_h_included
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