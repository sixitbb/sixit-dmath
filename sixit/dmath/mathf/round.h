/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Serhii Iliukhin
*/
#ifndef sixit_dmath_math_operations_round_h_included
#define sixit_dmath_math_operations_round_h_included

#include "__utils.h"

namespace sixit::dmath::mathf
{
    template <typename fp>
    inline fp _round(fp val)
    {
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC        
        if constexpr (std::is_same_v<float, fp>)
        {
            return std::round(val);
        } else {
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC                  

            int e = sixit::dmath::fp_traits<fp>::get_exp(val);
            fp y;

            if (e >= 23)
                return val;
            if (sixit::dmath::fp_traits<fp>::get_sign(val))
                val = -val;
            if (e < -1) {
                if constexpr (std::is_same<fp, float>())
                    force_eval_fp<fp>(val + fp(0x1p120f));
                return  fp(0.f) * val;
            }
            y = val + fp(0x1p120f) - fp(0x1p120f) - val;
            if (y > fp(0.5f))
                y = y + val - fp(1.f);
            else if (y <= fp(-0.5f))
                y = y + val + fp(1.f);
            else
                y = y + val;
            if (sixit::dmath::fp_traits<fp>::get_sign(val))
                y = -y;
            return y;
#ifndef SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC            
        }
#endif // SIXIT_DMATH_USE_SIXIT_FOR_NON_DETERMINISTIC               
    }

    template <typename fp>
    inline auto round(fp val)
    {
        return _round(sixit::dmath::fp_traits<fp>::to_fallback(val));
    }

    template <typename fp, sixit::units::physical_dimension dim_>
    inline sixit::units::dimensional_scalar<fp, dim_> round(sixit::units::dimensional_scalar<fp, dim_> val)
    {
        return sixit::units::dimensional_scalar<fp, dim_>({ round<fp>(val.value), sixit::units::internal_constructor_of_dimensional_scalar_from_fp() });
    }
} //  sixit::dmath::mathf

#endif // sixit_dmath_math_operations_round_h_included
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