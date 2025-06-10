/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Victor Istomin, Dmytro Ivanchykhin
*/
#ifndef sixit_dmath_gamefloat_ieee_float_if_semicolon_prohibits_reordering_h_included
#define sixit_dmath_gamefloat_ieee_float_if_semicolon_prohibits_reordering_h_included
#include "sixit/core/lwa.h"
#include "sixit/dmath/traits.h"

#include <cstdint>

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
 * @brief Whether the semicolon prevents reordering of floating point expression
 *
 * The following define can be manually defined via compiler flag if and only if the semicolon preventing reordering is
 * enforced by the compiler settings.
 *
 * For example:
 *    * `cl -ffp-contract=on -DSIXIT_DMATH_SUPPORT_IEEE_FLOAT_IF_SEMICOLON_PREVENTS_REORDERING my_gcc_src.cpp`
 */
#ifdef SIXIT_DMATH_SUPPORT_IEEE_FLOAT_IF_SEMICOLON_PREVENTS_REORDERING

/**
 * A header-only class which ensure that each math operation is a separate expression which ends with semicolon;
 * Will provide deterministic result only in case of compiler does not reorder ariphmetic across semicolon and
 * strict math options are enabled: -fno-fast-math, -fno-associative-math, and -ffp-contract=off
 */
class ieee_float_if_semicolon_prohibits_reordering
{
  public:
    float to_float() const
    {
        return data;
    }

    constexpr ieee_float_if_semicolon_prohibits_reordering() noexcept = default;
    ieee_float_if_semicolon_prohibits_reordering(const ieee_float_if_semicolon_prohibits_reordering& other) noexcept = default;
    ieee_float_if_semicolon_prohibits_reordering(ieee_float_if_semicolon_prohibits_reordering&& other) noexcept = default;
    constexpr ieee_float_if_semicolon_prohibits_reordering(const float& other) : data(other){};

    ieee_float_if_semicolon_prohibits_reordering operator+(ieee_float_if_semicolon_prohibits_reordering other) const
    {
        float ret = data + other.data;
        // !!! temporary variable and the semicolon is substantional here for invoking sequencing rule. Do not even think about merging in a single line !!!
        return ret;
    }

    ieee_float_if_semicolon_prohibits_reordering operator-(ieee_float_if_semicolon_prohibits_reordering other) const
    {
        float ret = data - other.data;
        // !!! temporary variable and the semicolon is substantional here for invoking sequencing rule. Do not even think about merging in a single line !!!
        return ret;
    }

    ieee_float_if_semicolon_prohibits_reordering operator*(ieee_float_if_semicolon_prohibits_reordering other) const
    {
        float ret = data * other.data;
        // !!! temporary variable and the semicolon is substantional here for invoking sequencing rule. Do not even think about merging in a single line !!!
        return ret;
    }

    ieee_float_if_semicolon_prohibits_reordering operator/(ieee_float_if_semicolon_prohibits_reordering other) const
    {
        float ret = data / other.data;
        // !!! temporary variable and the semicolon is substantional here for invoking sequencing rule. Do not even think about merging in a single line !!!
        return ret;
    }

    ieee_float_if_semicolon_prohibits_reordering& operator+=(ieee_float_if_semicolon_prohibits_reordering other)
    {
        float ret = data + other.data;
        // !!! temporary variable and the semicolon is substantional here for invoking sequencing rule. Do not even think about merging in a single line !!!
        data = ret;
        return *this;
    }

    ieee_float_if_semicolon_prohibits_reordering& operator-=(ieee_float_if_semicolon_prohibits_reordering other)
    {
        float ret = data - other.data;
        // !!! temporary variable and the semicolon is substantional here for invoking sequencing rule. Do not even think about merging in a single line !!!
        data = ret;
        return *this;
    }

    ieee_float_if_semicolon_prohibits_reordering& operator*=(ieee_float_if_semicolon_prohibits_reordering other)
    {
        float ret = data * other.data;
        // !!! temporary variable and the semicolon is substantional here for invoking sequencing rule. Do not even think about merging in a single line !!!
        data = ret;
        return *this;
    }

    ieee_float_if_semicolon_prohibits_reordering& operator/=(ieee_float_if_semicolon_prohibits_reordering other)
    {
        float ret = data / other.data;
        // !!! temporary variable and the semicolon is substantional here for invoking sequencing rule. Do not even think about merging in a single line !!!
        data = ret;
        return *this;
    }

    ieee_float_if_semicolon_prohibits_reordering& operator=(const ieee_float_if_semicolon_prohibits_reordering& other) noexcept = default;
    ieee_float_if_semicolon_prohibits_reordering& operator=(ieee_float_if_semicolon_prohibits_reordering&& other) noexcept = default;

    bool operator<(const ieee_float_if_semicolon_prohibits_reordering& other) const
    {
        return data < other.data;
    }

    bool operator>(const ieee_float_if_semicolon_prohibits_reordering& other) const
    {
        return data > other.data;
    }

    bool operator<=(const ieee_float_if_semicolon_prohibits_reordering& other) const
    {
        return data <= other.data;
    }

    bool operator>=(const ieee_float_if_semicolon_prohibits_reordering& other) const
    {
        return data >= other.data;
    }

    bool operator==(const ieee_float_if_semicolon_prohibits_reordering& other) const = default;

    ieee_float_if_semicolon_prohibits_reordering operator-() const
    {
        return ieee_float_if_semicolon_prohibits_reordering(-data);
    }

  private:
    float data = {};

    struct rw_alias
    {
        using value_type = ieee_float_if_semicolon_prohibits_reordering;
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

    friend struct sixit::units::dimensional_scalar_rw_alias_helper<sixit::dmath::ieee_float_if_semicolon_prohibits_reordering>;
    friend struct sixit::rw::member_type_alias<sixit::dmath::ieee_float_if_semicolon_prohibits_reordering>;

    template <typename fp>
    friend struct sixit::dmath::fp_traits;
};

template <>
struct fp_traits<ieee_float_if_semicolon_prohibits_reordering>
{
    static constexpr bool is_valid_fp = true;
    static constexpr bool is_deterministic = true;
    static constexpr bool is_fixed_point = false;
    static constexpr bool is_supported = true;

    static constexpr auto display_name = sixit::lwa::string_literal_helper("ieee_float_if_semicolon_prohibits_reordering");

    using intermediate_type = float;
    using fixed_point_type = void*;

    static bool isnan(ieee_float_if_semicolon_prohibits_reordering val)
    {
        return fp_traits<float>::isnan(val.data);
    }
    static bool isinf(ieee_float_if_semicolon_prohibits_reordering val)
    {
        return fp_traits<float>::isinf(val.data);
    }
    static bool isfinite(ieee_float_if_semicolon_prohibits_reordering val)
    {
        return fp_traits<float>::isfinite(val.data);
    }
    static int32_t get_exp(ieee_float_if_semicolon_prohibits_reordering val)
    {
        return fp_traits<float>::get_exp(val.data);
    }
    static int32_t get_mantissa(ieee_float_if_semicolon_prohibits_reordering val)
    {
        return fp_traits<float>::get_mantissa(val.data);
    }
    static bool set_exp(ieee_float_if_semicolon_prohibits_reordering& val, int exp)
    {
        return fp_traits<float>::set_exp(val.data, exp);
    }
    static int64_t fp2int64(ieee_float_if_semicolon_prohibits_reordering val)
    {
        return fp_traits<float>::fp2int64(val.data);
    }
    static uint32_t bit_cast_to_ieee_uint32(const ieee_float_if_semicolon_prohibits_reordering& val)
    {
        return fp_traits<float>::bit_cast_to_ieee_uint32(val.data);
    }
    static bool get_sign(const ieee_float_if_semicolon_prohibits_reordering& val)
    {
        return fp_traits<float>::get_sign(val.data);
    }
    static bool equal_to_zero(const ieee_float_if_semicolon_prohibits_reordering& val)
    {
        return fp_traits<float>::equal_to_zero(val.data);
    }
    static ieee_float_if_semicolon_prohibits_reordering bit_cast_from_ieee_uint32(uint32_t val)
    {
        return fp_traits<float>::bit_cast_from_ieee_uint32(val);
    }

    static auto to_fallback(ieee_float_if_semicolon_prohibits_reordering val) { return val; }
};

#else

// a stub as the ieee_float_inline_asm is not supported on this platform
class ieee_float_if_semicolon_prohibits_reordering
{
  public:
    struct rw_alias
    {
    };
};

template <>
struct fp_traits<ieee_float_if_semicolon_prohibits_reordering>
{
    // not implemented for this platform
    static constexpr bool is_supported = false;
};

#endif

} // namespace sixit::dmath

template <>
struct sixit::units::dimensional_scalar_rw_alias_helper<sixit::dmath::ieee_float_if_semicolon_prohibits_reordering>
    : sixit::dmath::ieee_float_if_semicolon_prohibits_reordering::rw_alias
{
};

template <>
struct sixit::rw::member_type_alias<sixit::dmath::ieee_float_if_semicolon_prohibits_reordering>
    : sixit::dmath::ieee_float_if_semicolon_prohibits_reordering::rw_alias
{
};

#endif // sixit_dmath_gamefloat_ieee_float_if_semicolon_prohibits_reordering_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Victor Istomin, Dmytro Ivanchykhin

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
