/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Victor Istomin
*/
#ifndef sixit_dmath_gamefloat_ieee_float_static_lib_h_included
#define sixit_dmath_gamefloat_ieee_float_static_lib_h_included

#include <cstdint>

#include "sixit/core/core.h"
#include "sixit/dmath/traits.h"

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

class ieee_float_static_lib;

/**
 * All-private internal implementations that should never be called directly.
 * Please use `ieee_float_static_lib` instead
 *
 * @see ieee_float_static_lib
 */
class ieee_float_static_lib_detail
{
    // all-private
    static float add(float lhs, float rhs);
    static float subtract(float lhs, float rhs);
    static float multiply(float lhs, float rhs);
    static float divide(float lhs, float rhs);
    static bool compare_lt(float lhs, float rhs);
    static bool compare_gt(float lhs, float rhs);
    static bool compare_le(float lhs, float rhs);
    static bool compare_ge(float lhs, float rhs);
    static bool compare_eq(float lhs, float rhs);
    static float negate(float value);

    static int64_t fp2int64(float val);
    static bool set_exp(float& val, int exp);
    static int32_t get_mantissa(float val);
    static int32_t get_exp(float val);
    static bool isfinite(float val);
    static bool isinf(float val);
    static bool isnan(float val);
    static uint32_t bit_cast_to_ieee_uint32(float val);
    static bool get_sign(float val);
    static bool equal_to_zero(float val);
    static float bit_cast_from_ieee_uint32(uint32_t val);

    friend sixit::dmath::ieee_float_static_lib;
    template <typename fp>
    friend struct sixit::dmath::fp_traits;
};

/**
 * @brief User's confirmation about LTO/LTCG absense is needed to use `ieee_float_static_lib`
 *
 * Once LTO flags aren't enabled in the compiler options, user may confirm this by defining
 * SIXIT_DMATH_SUPPORT_IEEE_FLOAT_STATIC_LIB in the compiler options.
 *
 * For example:
 *    `cl ... -DSIXIT_DMATH_SUPPORT_IEEE_FLOAT_STATIC_LIB my_src.cpp`
 */
#ifdef SIXIT_DMATH_SUPPORT_IEEE_FLOAT_STATIC_LIB

// todo: Detect LTO/LTCG!

/**
 * Static library based deterministic float.
 *
 * The point of having operations hidden inside a different translation unit is to
 * prevent compiler from inlining and optimizing its methods.
 *
 * @warning non-deterministic with LTO/LTCG (gcc `-flto` or msvc `/GL /LTCG`) or unity builds.
 */
class ieee_float_static_lib
{
  public:
    float to_float() const
    {
        return static_cast<float>(*this);
    }

    constexpr ieee_float_static_lib() noexcept = default;
    ieee_float_static_lib(const ieee_float_static_lib& other) noexcept = default;
    ieee_float_static_lib(ieee_float_static_lib&& other) noexcept = default;
    constexpr ieee_float_static_lib(const float& other) : data(other){};

    ieee_float_static_lib& operator=(const ieee_float_static_lib& other) = default;

    ieee_float_static_lib operator+(ieee_float_static_lib other) const
    {
        return ieee_float_static_lib_detail::add(data, other.data);
    }

    ieee_float_static_lib operator-(ieee_float_static_lib other) const
    {
        return ieee_float_static_lib_detail::subtract(data, other.data);
    }

    ieee_float_static_lib operator*(ieee_float_static_lib other) const
    {
        return ieee_float_static_lib_detail::multiply(data, other.data);
    }

    ieee_float_static_lib operator/(ieee_float_static_lib other) const
    {
        return ieee_float_static_lib_detail::divide(data, other.data);
    }

    ieee_float_static_lib& operator+=(ieee_float_static_lib other)
    {
        return *this = ieee_float_static_lib_detail::add(data, other.data);
    }

    ieee_float_static_lib& operator-=(ieee_float_static_lib other)
    {
        return *this = ieee_float_static_lib_detail::subtract(data, other.data);
    }

    ieee_float_static_lib& operator*=(ieee_float_static_lib other)
    {
        return *this = ieee_float_static_lib_detail::multiply(data, other.data);
    }

    ieee_float_static_lib& operator/=(ieee_float_static_lib other)
    {
        return *this = ieee_float_static_lib_detail::divide(data, other.data);
    }

    bool operator<(const ieee_float_static_lib& other) const
    {
        return ieee_float_static_lib_detail::compare_lt(data, other.data);
    }

    bool operator>(const ieee_float_static_lib& other) const
    {
        return ieee_float_static_lib_detail::compare_gt(data, other.data);
    }

    bool operator<=(const ieee_float_static_lib& other) const
    {
        return ieee_float_static_lib_detail::compare_le(data, other.data);
    }

    bool operator>=(const ieee_float_static_lib& other) const
    {
        return ieee_float_static_lib_detail::compare_ge(data, other.data);
    }

    bool operator==(const ieee_float_static_lib& other) const
    {
        return ieee_float_static_lib_detail::compare_eq(data, other.data);
    }

    ieee_float_static_lib operator-() const
    {
        return ieee_float_static_lib_detail::negate(data);
    }

  private:
    float data = {};

    operator float() const
    {
        return data;
    }

    struct rw_alias
    {
        using value_type = ieee_float_static_lib;
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

    friend struct sixit::units::dimensional_scalar_rw_alias_helper<sixit::dmath::ieee_float_static_lib>;
    friend struct sixit::rw::member_type_alias<ieee_float_static_lib>;

    template <typename fp>
    friend struct sixit::dmath::fp_traits;
};

template <>
struct fp_traits<ieee_float_static_lib>
{
    static constexpr bool is_valid_fp = true;
    static constexpr bool is_deterministic = true;
    static constexpr bool is_fixed_point = false;
    static constexpr bool is_supported = true;

    static constexpr auto display_name = sixit::lwa::string_literal_helper("ieee_float_static_lib");

    using intermediate_type = float;
    using fixed_point_type = void*;

    static int64_t fp2int64(ieee_float_static_lib val)
    {
        return ieee_float_static_lib_detail::fp2int64(val.data);
    }

    static bool set_exp(ieee_float_static_lib& val, int exp)
    {
        return ieee_float_static_lib_detail::set_exp(val.data, exp);
    }

    static int32_t get_mantissa(ieee_float_static_lib val)
    {
        return ieee_float_static_lib_detail::get_mantissa(val.data);
    }

    static int32_t get_exp(ieee_float_static_lib val)
    {
        return ieee_float_static_lib_detail::get_exp(val.data);
    }

    static bool isfinite(ieee_float_static_lib val)
    {
        return ieee_float_static_lib_detail::isfinite(val.data);
    }

    static bool isinf(ieee_float_static_lib val)
    {
        return ieee_float_static_lib_detail::isinf(val.data);
    }

    static bool isnan(ieee_float_static_lib val)
    {
        return ieee_float_static_lib_detail::isnan(val.data);
    }

    static uint32_t bit_cast_to_ieee_uint32(ieee_float_static_lib val)
    {
        return ieee_float_static_lib_detail::bit_cast_to_ieee_uint32(val.data);
    }

    static bool get_sign(ieee_float_static_lib val)
    {
        return ieee_float_static_lib_detail::get_sign(val.data);
    }

    static bool equal_to_zero(ieee_float_static_lib val)
    {
        return ieee_float_static_lib_detail::equal_to_zero(val.data);
    }

    static ieee_float_static_lib bit_cast_from_ieee_uint32(uint32_t val)
    {
        return ieee_float_static_lib(ieee_float_static_lib_detail::bit_cast_from_ieee_uint32(val));
    }
    static auto to_fallback(ieee_float_static_lib val) { return val; }
};

#else  // LTO/LTCG absence is not confirmed, ieee_float_static_lib disabled
// a stub as the ieee_float_inline_asm is not supported on this platform
class ieee_float_static_lib
{
  public:
    struct rw_alias
    {
    };
};

template <>
struct fp_traits<ieee_float_static_lib>
{
    // not implemented for this platform
    static constexpr bool is_supported = false;
};
#endif // SIXIT_DMATH_SUPPORT_IEEE_FLOAT_STATIC_LIB

} // namespace sixit::dmath

template <>
struct sixit::units::dimensional_scalar_rw_alias_helper<sixit::dmath::ieee_float_static_lib>
    : sixit::dmath::ieee_float_static_lib::rw_alias
{
};

template <>
struct sixit::rw::member_type_alias<sixit::dmath::ieee_float_static_lib> : sixit::dmath::ieee_float_static_lib::rw_alias
{
};

#endif // sixit_dmath_gamefloat_ieee_float_static_lib_h_included

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
