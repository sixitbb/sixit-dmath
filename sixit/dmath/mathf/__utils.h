/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Serhii Iliukhin
*/
#ifndef sixit_dmath_math_operations___utils_h_included
#define sixit_dmath_math_operations___utils_h_included

#include <cstdint>
#include "sixit/dmath/traits.h"

#ifdef __GNUC__
#define predict_true(x) __builtin_expect(!!(x), 1)
#define predict_false(x) __builtin_expect(x, 0)
#else
#define predict_true(x) (x)
#define predict_false(x) (x)
#endif


namespace sixit::dmath::mathf {
    constexpr double PI2 = 1.5707963267948966;

    template <typename fp>
    inline fp eval_as_fp(fp x)
    {
        fp y = x;
        return y;
    }

    template <typename fp>
    inline void force_eval_fp(const fp x)
    {
        volatile fp y;
        y = x;
    }

    template <typename fp>
    inline uint32_t top12(fp x)
    {
        return sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(x) >> 20;
    }

    inline float fp_barrierf(float x)
    {
        // __asm__ __volatile__ ("" : "+w"(x)); 
        return x;
    }


        
    template <typename fp>
    inline fp __math_xflowf(uint32_t sign, float y)
    {
        fp rv = fp(fp_barrierf(sign ? -y : y));
        // fp fp_y = fp(y);
        // TODO rv = rv * fp_y;
        return eval_as_fp(rv);
    }

    template <typename fp>
    inline fp __math_oflowf(uint32_t sign)
    {
        return __math_xflowf<fp>(sign, 0x1p97f);
    }

    template <typename fp>
    inline fp __math_uflowf(uint32_t sign)
    {
        return __math_xflowf<fp>(sign, 0x1p-95f);
    }

    template <typename fp>
    inline double asdouble(uint64_t val)
    {
        return sixit::lwa::bit_cast<double>(val);
    }

    inline uint64_t asuint64(double val) {
        return sixit::lwa::bit_cast<uint64_t>(val);
    }

    inline double eval_as_double(double x)
    {
        double y = x;
        return y;
    }

    template <typename fp>
    inline fp __math_divzerof(uint32_t sign)
    {
        return fp(sign ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity());
        //return fp(fp_barrierf(sign ? -1.0f : 1.0f) / 0.0f);
    }
}

namespace sixit::units {
    struct physical_dimension;

    template<class fp, physical_dimension dim_>
    class dimensional_scalar;

    template<physical_dimension dim>
    constexpr physical_dimension sqrt();

    struct internal_constructor_of_dimensional_scalar_from_fp;
}

#endif // sixit_dmath_math_operations___utils_h_included

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