/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Victor Istomin
*/

#ifndef sixit_dmath_gamefloat_ieee_float_inline_asm_h_included
#define sixit_dmath_gamefloat_ieee_float_inline_asm_h_included

#include "sixit/core/cpual/ieeefloat_inline_asm.h"
#include "sixit/core/guidelines.h"
#include "sixit/core/lwa.h"
#include "sixit/dmath/traits.h"
#include <cstdint>
#include <limits>
#include <type_traits>

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

// several architectures support ieee_float_inline_asm
#if defined(SIXIT_CPU_X64) || defined(SIXIT_CPU_X86) || defined(SIXIT_CPU_RISCV64) || defined(SIXIT_CPU_RISCV32) ||    \
    (defined(SIXIT_COMPILER_ANY_GCC_OR_CLANG) && defined(SIXIT_CPU_ARM64))

/**
 * @brief SIMD-based deterministic float implementation
 */
class ieee_float_inline_asm
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

    template <typename T>
    static constexpr bool is_simd_type = std::is_same_v<T, sixit::cpual::asm_float_t> && !std::is_same_v<T, float>;

    template <typename SimdFloat, typename = std::enable_if_t<is_simd_type<SimdFloat>>>
    ieee_float_inline_asm(SimdFloat xmm) : data(xmm)
    {
    }

  public:
    float to_float() const
    {
        return sixit::cpual::ieee_asm_to_float(data);
    }

    constexpr ieee_float_inline_asm() noexcept = default;
    ieee_float_inline_asm(const ieee_float_inline_asm& other) noexcept = default;
    ieee_float_inline_asm(ieee_float_inline_asm&& other) noexcept = default;
    ieee_float_inline_asm& operator=(const ieee_float_inline_asm& other) noexcept = default;

    constexpr ieee_float_inline_asm(float f) : data(sixit::cpual::ieee_asm_from_float(f)){};

    ieee_float_inline_asm operator+(ieee_float_inline_asm other) const
    {
        return sixit::cpual::ieee_add_float(data, other.data);
    }

    ieee_float_inline_asm operator-(ieee_float_inline_asm other) const
    {
        return sixit::cpual::ieee_subtract_float(data, other.data);
    }

    ieee_float_inline_asm operator*(ieee_float_inline_asm other) const
    {
        return sixit::cpual::ieee_multiply_float(data, other.data);
    }

    ieee_float_inline_asm operator/(ieee_float_inline_asm other) const
    {
        return sixit::cpual::ieee_divide_float(data, other.data);
    }

    ieee_float_inline_asm& operator+=(ieee_float_inline_asm other)
    {
        return *this = sixit::cpual::ieee_add_float(data, other.data);
    }

    ieee_float_inline_asm& operator-=(ieee_float_inline_asm other)
    {
        return *this = sixit::cpual::ieee_subtract_float(data, other.data);
    }

    ieee_float_inline_asm& operator*=(ieee_float_inline_asm other)
    {
        return *this = sixit::cpual::ieee_multiply_float(data, other.data);
    }

    ieee_float_inline_asm& operator/=(ieee_float_inline_asm other)
    {
        return *this = sixit::cpual::ieee_divide_float(data, other.data);
    }

    bool operator<(ieee_float_inline_asm other) const
    {
        return sixit::cpual::ieee_cmp_lt_float(data, other.data);
    }

    bool operator<=(ieee_float_inline_asm other) const
    {
        return sixit::cpual::ieee_cmp_le_float(data, other.data);
    }

    bool operator==(ieee_float_inline_asm other) const
    {
        return sixit::cpual::ieee_cmp_eq_float(data, other.data);
    }

    bool operator>(ieee_float_inline_asm other) const
    {
        return other < *this;
    }

    bool operator>=(ieee_float_inline_asm other) const
    {
        return other <= *this;
    }

    bool operator!=(ieee_float_inline_asm other) const
    {
        return !(*this == other);
    }

    ieee_float_inline_asm operator-() const
    {
        return ieee_float_inline_asm(sixit::cpual::ieee_neg_float(data));
    }

  private:
    sixit::cpual::asm_float_t data = {};

    template <typename fp>
    friend struct sixit::dmath::fp_traits;

    struct rw_alias
    {
        using value_type = ieee_float_inline_asm;
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

    friend struct sixit::units::dimensional_scalar_rw_alias_helper<sixit::dmath::ieee_float_inline_asm>;
    friend struct sixit::rw::member_type_alias<sixit::dmath::ieee_float_inline_asm>;
};

template <>
struct fp_traits<ieee_float_inline_asm>
{
    static constexpr bool is_valid_fp = true;
    static constexpr bool is_deterministic = true;
    static constexpr bool is_fixed_point = false;
    static constexpr bool is_supported = true;

    static constexpr auto display_name = sixit::lwa::string_literal_helper("ieee_float_inline_asm");

    using intermediate_type = float;
    using fixed_point_type = void*;

    static bool isnan(ieee_float_inline_asm val)
    {
        static constexpr uint32_t all_but_sign = 0x7fff'ffff;
        static constexpr uint32_t exponent_mask = 0x7f80'0000;

        // `exp == all_ones && mantissa > 0`
        return (bit_cast_to_ieee_uint32(val) & all_but_sign) > exponent_mask;
    }

    static bool isinf(ieee_float_inline_asm val)
    {
        static constexpr uint32_t all_but_sign = 0x7fff'ffff;
        static constexpr uint32_t exponent_mask = 0x7f80'0000;

        // `exp == all_ones && mantissa == 0`
        return (bit_cast_to_ieee_uint32(val) & all_but_sign) == exponent_mask;
    }

    static bool isfinite(ieee_float_inline_asm val)
    {
        static constexpr uint32_t all_but_sign = 0x7fff'ffff;
        static constexpr uint32_t exponent_mask = 0x7f80'0000;

        // `exp < all_ones`
        return (bit_cast_to_ieee_uint32(val) & all_but_sign) < exponent_mask;
    }

    static int32_t get_exp(ieee_float_inline_asm val)
    {
        auto bits = bit_cast_to_ieee_uint32(val);
        return ieee_float_inline_asm::f32bits_to_exponent(bits);
    }

    static int32_t get_mantissa(ieee_float_inline_asm val)
    {
        auto bits = bit_cast_to_ieee_uint32(val);
        return ieee_float_inline_asm::f32bits_to_mantissa(bits);
    }

    static bool set_exp(ieee_float_inline_asm& val, int exp)
    {
        SIXIT_ASSUMERT(exp >= -127 && exp <= 127);
        if (exp < -127 || exp > 127)
            return false;

        auto bits = bit_cast_to_ieee_uint32(val);
        bits = ieee_float_inline_asm::f32bits_set_exponent(bits, exp);
        val = bit_cast_from_ieee_uint32(bits);
        return true;
    }

    static int64_t fp2int64(ieee_float_inline_asm val)
    {
        return static_cast<int64_t>(val.to_float());
    }

    static uint32_t bit_cast_to_ieee_uint32(ieee_float_inline_asm val)
    {
        return sixit::lwa::bit_cast<uint32_t>(val.to_float());
    }

    static bool get_sign(ieee_float_inline_asm val)
    {
        static constexpr uint32_t sign_bit = 0x8000'0000;
        return (bit_cast_to_ieee_uint32(val) & sign_bit) != 0;
    }

    static bool equal_to_zero(ieee_float_inline_asm val)
    {
        static constexpr uint32_t sign_bit = 0x8000'0000;
        return (bit_cast_to_ieee_uint32(val) & ~sign_bit) == 0;
    }

    static ieee_float_inline_asm bit_cast_from_ieee_uint32(uint32_t val)
    {
        return sixit::lwa::bit_cast<float>(val);
    }

    static auto to_fallback(ieee_float_inline_asm val)
    {
        return val;
    }
};

#else // unsupported platform

// a stub as the ieee_float_inline_asm is not supported on this platform
class ieee_float_inline_asm
{
  public:
    struct rw_alias
    {
    };
};

template <>
struct fp_traits<ieee_float_inline_asm>
{
    // not implemented for this platform
    static constexpr bool is_supported = false;
};

#endif

} // namespace sixit::dmath

template <>
struct sixit::units::dimensional_scalar_rw_alias_helper<sixit::dmath::ieee_float_inline_asm>
    : sixit::dmath::ieee_float_inline_asm::rw_alias
{
};

template <>
struct sixit::rw::member_type_alias<sixit::dmath::ieee_float_inline_asm> : sixit::dmath::ieee_float_inline_asm::rw_alias
{
};

#endif // sixit_dmath_gamefloat_ieee_float_inline_asm_h_included

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
