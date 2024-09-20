/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Serhii Iliukhin
*/

#ifndef sixit_dmath_gamefloat_float_with_sixit_h_included
#define sixit_dmath_gamefloat_float_with_sixit_h_included

#include <sixit/dmath/traits.h>

namespace sixit::dmath {
    class float_with_sixit {
    private:
        struct rw_alias
        {
            using value_type = float_with_sixit;
            using alias_type = float;
            using type = float; // vi_todo: ask DI for rw refactoring to use alias_type instead?

            static alias_type value2alias(const value_type& value)
            {
                return value.to_float();
            }

            static value_type alias2value(alias_type value)
            {
                return {value};
            }
        };

        friend struct sixit::units::dimensional_scalar_rw_alias_helper<sixit::dmath::float_with_sixit>;
        friend struct sixit::rw::member_type_alias<float_with_sixit>;

        template <typename fp>
        friend struct sixit::dmath::fp_traits;
    public:
        // operator float() = delete;
        float to_float() const 
        {
            return f;
        }
    
        constexpr float_with_sixit(): f(0.f) {};
        constexpr float_with_sixit(float val) : f(val) {};
        constexpr float_with_sixit(const float_with_sixit& val) = default;
        constexpr float_with_sixit(float_with_sixit&& val) = default;
        constexpr float_with_sixit& operator=(const float_with_sixit& val) = default;

        float_with_sixit& operator *= (const float_with_sixit& val) { f *= val.f; return *this; }
        float_with_sixit& operator /= (const float_with_sixit& val) { f /= val.f; return *this; }
        float_with_sixit& operator += (const float_with_sixit& val) { f += val.f; return *this; }
        float_with_sixit& operator -= (const float_with_sixit& val) { f -= val.f; return *this; }

        float_with_sixit operator * (const float_with_sixit& val) const { return f * val.f; }
        float_with_sixit operator / (const float_with_sixit& val) const { return f / val.f; }
        float_with_sixit operator + (const float_with_sixit& val) const { return f + val.f; }
        float_with_sixit operator - (const float_with_sixit& val) const { return f - val.f; }

        bool operator<(const float_with_sixit& other) const { return f < other.f; } 
        bool operator>(const float_with_sixit& other) const { return f > other.f; } 
        bool operator<=(const float_with_sixit& other) const { return f <= other.f; } 
        bool operator>=(const float_with_sixit& other) const { return f >= other.f; } 

        bool operator==(const float_with_sixit& other) const = default;
        bool operator!=(const float_with_sixit& other) const = default;

        float_with_sixit operator-() const {return -f;}
   
    public:
        float f;
    };
} // sixit::dmath

namespace sixit::dmath {
    template <>
    struct fp_traits<float_with_sixit>
    {
        static constexpr bool is_valid_fp = true;
        static constexpr bool is_deterministic = false;
        static constexpr bool is_fixed_point = false;
        static constexpr bool is_supported = true;

        static constexpr auto display_name = sixit::lwa::string_literal_helper("float_with_sixit");

        using intermediate_type = float_with_sixit;
        using fixed_point_type = void*;

        static bool isnan(const float_with_sixit& val)
        {
            return std::isnan(val.f);
        }

        static bool isinf(const float_with_sixit& val)
        {
            return std::isinf(val.f);
        }

        static bool isfinite(const float_with_sixit& val)
        {
            return std::isfinite(val.f);
        }

        static int32_t get_exp(float_with_sixit val)
        { 
            int32_t rv = (sixit::lwa::bit_cast<uint32_t>(val.f) >> 23) & 0xff;
            rv -= 0x7f;
            return rv;
        }

        static int32_t get_mantissa(float_with_sixit val)
        {
            int32_t rv((*sixit::lwa::bit_cast<uint32_t*>(&val.f) & 0x7fffff) | uint32_t(get_exp(val) > -127) << 23);
            return val >= 0.f ? rv : -rv;
        }

        static int64_t fp2int64(float_with_sixit val) {
            return int64_t(val.f);
        }

        static bool set_exp(float_with_sixit& val, int exp) 
        { 
            exp += 127;
            if (exp < 0 || exp >= 256)
                return false;
            uint32_t rv = sixit::lwa::bit_cast<uint32_t>(val.f);
            rv &= UINT32_C(0x807fffff);
            rv |= exp << 23;
            val.f = sixit::lwa::bit_cast<float>(rv);
            return true;
        }

        static uint32_t bit_cast_to_ieee_uint32(const float_with_sixit& val)
        {
            return sixit::lwa::bit_cast<uint32_t>(val.f);
        }

        static bool get_sign(const float_with_sixit& val)
        {
            return sixit::lwa::bit_cast<uint32_t>(val.f) >> 31;
        }

        static bool equal_to_zero(const float_with_sixit& val)
        {
            return !(sixit::lwa::bit_cast<uint32_t>(val.f) << 1);
        }

        static float_with_sixit bit_cast_from_ieee_uint32(uint32_t val)
        {
            return sixit::lwa::bit_cast<float>(val);
        }

        static auto to_fallback(const float_with_sixit& val) { return val; }
    };

} // sixit::dmath


template <>
struct sixit::units::dimensional_scalar_rw_alias_helper<sixit::dmath::float_with_sixit>
    : sixit::dmath::float_with_sixit::rw_alias
{
};

template <>
struct sixit::rw::member_type_alias<sixit::dmath::float_with_sixit> : sixit::dmath::float_with_sixit::rw_alias
{
};

#endif // sixit_dmath_gamefloat_float_with_sixit_h_included

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