/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Victor Istomin
*/

#ifndef sixit_dmath_gamefloat_ieee_float_soft_h_included
#define sixit_dmath_gamefloat_ieee_float_soft_h_included

#include <cstdint>
#include <limits>
#include <type_traits>

#include "sixit/core/guidelines.h"
#include "sixit/core/lwa.h"
#include "sixit/dmath/traits.h"

#include "sixit/dmath/softfloat/softfloat_inline.h"

namespace sixit::rw
{
// forward declaration to avoid sixit::rw dependency
template <typename T>
struct member_type_alias;
} // namespace sixit::rw

namespace sixit::units
{
// forward declatation of helper for sixit::units library
template <typename Fp>
struct dimensional_scalar_rw_alias_helper;
} // namespace sixit::units

namespace sixit::dmath
{

/**
 * @brief IEEE-compatible deterministic floating point class based on adaptation of Berkeley's SoftFloat
 * 
 * See `sixit/dmath/softfloat/README.md` for details on adaptation.
 */
class ieee_float_soft
{
    static int32_t f32bits_to_exponent(uint32_t bits)
    {
        return ((bits >> 23) & 0xFF) - 127;
    }

    static int32_t f32bits_to_mantissa(uint32_t bits)
    {
        int32_t rv = bits & 0x7f'ffff;
        uint32_t implicit_bit = static_cast<uint32_t>(f32bits_to_exponent(bits) > -127) << 23;

        rv |= implicit_bit;
        bool is_negative = bits & INT32_C(0x8000'0000);
        return is_negative ? -rv : rv;
    }

    static uint32_t f32bits_set_exponent(uint32_t bits, int32_t new_exp)
    {
        auto biased_exp = new_exp + 127;
        SIXIT_ASSUMERT(biased_exp >= 0 && biased_exp < 0x100);

        bits &= UINT32_C(0x807f'ffff);
        bits |= biased_exp << 23;
        return bits;
    }

    using soft_float_t = sixit::dmath::softfloat::float32_t;

    ieee_float_soft(soft_float_t f32) : data(f32)
    {
    }

  public:
    float to_float() const
    {
        return sixit::lwa::bit_cast<float>(data);
    }

    constexpr ieee_float_soft() noexcept = default;
    ieee_float_soft(const ieee_float_soft& other) noexcept = default;
    ieee_float_soft(ieee_float_soft&& other) noexcept = default;
    ieee_float_soft& operator=(const ieee_float_soft& other) noexcept = default;

    constexpr ieee_float_soft(float f) : data(sixit::lwa::bit_cast<soft_float_t>(f)) {};

    ieee_float_soft operator+(ieee_float_soft other) const
    {
        return sixit::dmath::softfloat::f32_add(data, other.data);
    }

    ieee_float_soft operator-(ieee_float_soft other) const
    {
        return sixit::dmath::softfloat::f32_sub(data, other.data);
    }
    
    ieee_float_soft operator*(ieee_float_soft other) const
    {
        return sixit::dmath::softfloat::f32_mul(data, other.data);
    }

    ieee_float_soft operator/(ieee_float_soft other) const
    {
        return sixit::dmath::softfloat::f32_div(data, other.data);
    }

    ieee_float_soft& operator+=(ieee_float_soft other)
    {
        return *this = sixit::dmath::softfloat::f32_add(data, other.data);
    }

    ieee_float_soft& operator-=(ieee_float_soft other)
    {
        return *this = sixit::dmath::softfloat::f32_sub(data, other.data);
    }

    ieee_float_soft& operator*=(ieee_float_soft other)
    {
        return *this = sixit::dmath::softfloat::f32_mul(data, other.data);
    }

    ieee_float_soft& operator/=(ieee_float_soft other)
    {
        return *this = sixit::dmath::softfloat::f32_div(data, other.data);
    }

    bool operator<(ieee_float_soft other) const
    {
        return sixit::dmath::softfloat::f32_lt(data, other.data);
    }

    bool operator<=(ieee_float_soft other) const
    {
        return sixit::dmath::softfloat::f32_le(data, other.data);
    }

    bool operator==(ieee_float_soft other) const
    {
        return sixit::dmath::softfloat::f32_eq(data, other.data);
    }

    bool operator>(ieee_float_soft other) const
    {
        return other < *this;
    }

    bool operator>=(ieee_float_soft other) const
    {
        return other <= *this;
    }

    bool operator!=(ieee_float_soft other) const
    {
        return !(*this == other);
    }

    ieee_float_soft operator-() const
    {
        static constexpr uint32_t sign_bit = 0x8000'0000;
        uint32_t bits = sixit::lwa::bit_cast<uint32_t>(to_float());
        return sixit::lwa::bit_cast<float>(bits ^ sign_bit);
    }

  private:
    soft_float_t data = {};

    template <typename fp>
    friend struct sixit::dmath::fp_traits;

    struct rw_alias
    {
        using value_type = ieee_float_soft;
        using alias_type = float;
        using type = float; // todo: ask DI for rw refactoring to use alias_type instead?

        static alias_type value2alias(const value_type& value)
        {
            return value.to_float();
        }

        static value_type alias2value(alias_type value)
        {
            return {value};
        }
    };

    friend struct sixit::units::dimensional_scalar_rw_alias_helper<sixit::dmath::ieee_float_soft>;
    friend struct sixit::rw::member_type_alias<sixit::dmath::ieee_float_soft>;
};

template <>
struct fp_traits<ieee_float_soft>
{
    static constexpr bool is_valid_fp = true;
    static constexpr bool is_deterministic = true;
    static constexpr bool is_fixed_point = false;
    static constexpr bool is_supported = true;

    static constexpr auto display_name = sixit::lwa::string_literal_helper("ieee_float_soft");

    using intermediate_type = float;
    using fixed_point_type = void*;

    static bool isnan(ieee_float_soft val)
    {
        return sixit::dmath::softfloat::f32_isSignalingNaN(val.data) || (val != val);
    }

    static bool isinf(ieee_float_soft val)
    {
        static constexpr auto pos_inf = ieee_float_soft(std::numeric_limits<float>::infinity());
        static constexpr auto neg_inf = ieee_float_soft(-std::numeric_limits<float>::infinity());
        return val == pos_inf || val == neg_inf;
    }

    static bool isfinite(ieee_float_soft val)
    {
        static constexpr int special_exp = 128;    // exponent of infinite and NaN
        return get_exp(val) != special_exp;
    }

    static int32_t get_exp(ieee_float_soft val)
    {
        uint32_t bits = fp_traits<ieee_float_soft>::bit_cast_to_ieee_uint32(val);
        return ieee_float_soft::f32bits_to_exponent(bits);
    }

    static int32_t get_mantissa(ieee_float_soft val)
    {
        uint32_t bits = fp_traits<ieee_float_soft>::bit_cast_to_ieee_uint32(val);
        return ieee_float_soft::f32bits_to_mantissa(bits);
    }

    static bool set_exp(ieee_float_soft& val, int exp)
    {
        SIXIT_ASSUMERT(exp >= -127 && exp <= 127);
        if (exp < -127 || exp > 127)
            return false;

        uint32_t bits = fp_traits<ieee_float_soft>::bit_cast_to_ieee_uint32(val);
        bits = ieee_float_soft::f32bits_set_exponent(bits, exp);
        val = fp_traits<ieee_float_soft>::bit_cast_from_ieee_uint32(bits);
        return true;
    }

    static int64_t fp2int64(ieee_float_soft val)
    {
        return sixit::dmath::softfloat::f32_to_i64(val.data, sixit::dmath::softfloat::softfloat_round_minMag, false);
    }

    static uint32_t bit_cast_to_ieee_uint32(ieee_float_soft val)
    {
        return sixit::lwa::bit_cast<uint32_t>(val.to_float());
    }
    
    static ieee_float_soft bit_cast_from_ieee_uint32(uint32_t bits)
    {
        return ieee_float_soft(sixit::lwa::bit_cast<ieee_float_soft::soft_float_t>(bits));
    }

    static bool get_sign(ieee_float_soft val)
    {
        static constexpr uint32_t sign_bit = 0x8000'0000;
        return (bit_cast_to_ieee_uint32(val) & sign_bit) != 0;
    }

    static bool equal_to_zero(ieee_float_soft val)
    {
        static constexpr uint32_t sign_bit = 0x8000'0000;
        return (bit_cast_to_ieee_uint32(val) & ~sign_bit) == 0;
    }

    static auto to_fallback(ieee_float_soft val) { return val; }
};

} // namespace sixit::dmath

template <>
struct sixit::units::dimensional_scalar_rw_alias_helper<sixit::dmath::ieee_float_soft>
    : sixit::dmath::ieee_float_soft::rw_alias
{
};

template <>
struct sixit::rw::member_type_alias<sixit::dmath::ieee_float_soft> : sixit::dmath::ieee_float_soft::rw_alias
{
};

#endif // sixit_dmath_gamefloat_ieee_float_soft_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Victor Istomin

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
