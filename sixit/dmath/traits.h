/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Serhii Iliukhin, Dmytro Ivanchykhin
*/

#ifndef sixit_dmath_traits_h_included
#define sixit_dmath_traits_h_included

#include <cmath>
#include <stdint.h>
#include <sixit/core/units.h>
#include <bit>

#include "sixit/core/lwa.h"

namespace sixit::dmath 
{
    template <typename fp>
    struct fp_traits;

    template <>
    struct fp_traits<float>
    {
        static constexpr bool is_valid_fp = true;
        static constexpr bool is_deterministic = false;
        static constexpr bool is_fixed_point = false;
        /** whether the platform supports this kind of FP */
        static constexpr bool is_supported = true;

        static constexpr auto display_name = sixit::lwa::string_literal_helper("float");

        using intermediate_type = float;
        using fixed_point_type = void*;
        static constexpr int significant_bit_count = 23;
        static constexpr uint32_t significand_mask = 0x007fffff;

        static bool isnan(const float& val)
        {
            return std::isnan(val);
        }

        static bool isinf(const float& val)
        {
            return std::isinf(val);
        }

        static bool isfinite(const float& val)
        {
            return std::isfinite(val);
        }

        static int32_t get_exp(float val)
        { 
            int32_t rv = (sixit::lwa::bit_cast<uint32_t>(val) >> 23) & 0xff;
            rv -= 0x7f;
            return rv;
        }

        static int32_t get_mantissa(float val)
        {
            int32_t rv((*sixit::lwa::bit_cast<uint32_t*>(&val) & 0x7fffff) | uint32_t(get_exp(val) > -127) << 23);
            return val >= 0.f ? rv : -rv;
        }

        static int64_t fp2int64(float val) {
            return int64_t(val);
        }

        static bool set_exp(float& val, int exp) 
        { 
            exp += 127;
            if (exp < 0 || exp >= 256)
                return false;
            uint32_t rv = sixit::lwa::bit_cast<uint32_t>(val);
            rv &= UINT32_C(0x807fffff);
            rv |= exp << 23;
            val = sixit::lwa::bit_cast<float>(rv);
            return true;
        }

        static uint32_t bit_cast_to_ieee_uint32(const float& val)
        {
            return sixit::lwa::bit_cast<uint32_t>(val);
        }

        static bool get_sign(const float& val)
        {
            return sixit::lwa::bit_cast<uint32_t>(val) >> 31;
        }

        static bool equal_to_zero(const float& val)
        {
            return !(sixit::lwa::bit_cast<uint32_t>(val) << 1);
        }

        static float bit_cast_from_ieee_uint32(uint32_t val)
        {
            return sixit::lwa::bit_cast<float>(val);
        }

        using rw_alias_type = float;
        rw_alias_type to_rw_alias_type(const float& val) {
            return rw_alias_type(val);
        }
        float from_rw_alias_type(rw_alias_type val) {
            return { val };
        }

        static auto to_fallback(const float& val) { return val; }
    };
}

namespace sixit::units {
    template<>
    struct dimensional_scalar_rw_alias_helper<float>
    {
        using value_type = float;
        using alias_type = float;

        static alias_type value2alias(const value_type& value)
        {
            return value;
        }

        static value_type alias2value(alias_type value)
        {
            return { value };
        }
    };
} // namespace sixit::units





#endif // sixit_dmath_traits_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Serhii Iliukhin, Dmytro Ivanchykhin

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