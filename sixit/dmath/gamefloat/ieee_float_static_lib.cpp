/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Victor Istomin
*/

#include "sixit/core/lwa.h"
#include "sixit/dmath/gamefloat/ieee_float_static_lib.h"

#include <cmath>

using sixit::dmath::ieee_float_static_lib_detail;

float ieee_float_static_lib_detail::add(float lhs, float rhs)
{
    return lhs + rhs;
}

float ieee_float_static_lib_detail::subtract(float lhs, float rhs)
{
    return lhs - rhs;
}

float ieee_float_static_lib_detail::multiply(float lhs, float rhs)
{
    return lhs * rhs;
}

float ieee_float_static_lib_detail::divide(float lhs, float rhs)
{
    return lhs / rhs;
}

bool ieee_float_static_lib_detail::compare_lt(float lhs, float rhs)
{
    return lhs < rhs;
}

bool ieee_float_static_lib_detail::compare_gt(float lhs, float rhs)
{
    return lhs > rhs;
}

bool ieee_float_static_lib_detail::compare_le(float lhs, float rhs)
{
    return lhs <= rhs;
}

bool ieee_float_static_lib_detail::compare_ge(float lhs, float rhs)
{
    return lhs >= rhs;
}

bool ieee_float_static_lib_detail::compare_eq(float lhs, float rhs)
{
    return lhs == rhs;
}

float ieee_float_static_lib_detail::negate(float value)
{
    return -value;
}

int64_t ieee_float_static_lib_detail::fp2int64(float val)
{
    return static_cast<int64_t>(val);
}

bool ieee_float_static_lib_detail::set_exp(float& val, int exp)
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

int32_t ieee_float_static_lib_detail::get_mantissa(float val)
{
    int32_t bits = sixit::lwa::bit_cast<int32_t>(val);
    int32_t rv = bits & INT32_C(0x7fffff);
    bool has_implicit_bit = get_exp(val) > -127;
    rv |= uint32_t(has_implicit_bit) << 23;

    bool is_negative = bits & INT32_C(0x8000'0000);
    return is_negative ? -rv : rv;
}

int32_t ieee_float_static_lib_detail::get_exp(float val)
{
    int32_t rv = (sixit::lwa::bit_cast<uint32_t>(val) >> 23) & 0xff;
    rv -= 127;
    return rv;
}

bool ieee_float_static_lib_detail::isfinite(float val)
{
    static constexpr uint32_t all_but_sign = 0x7fff'ffff;
    static constexpr uint32_t exponent_mask = 0x7f80'0000;

    // `exp < all_ones`
    return (bit_cast_to_ieee_uint32(val) & all_but_sign) < exponent_mask;
}

bool ieee_float_static_lib_detail::isinf(float val)
{
    static constexpr uint32_t all_but_sign = 0x7fff'ffff;
    static constexpr uint32_t exponent_mask = 0x7f80'0000;

    // `exp == all_ones && mantissa == 0`
    return (bit_cast_to_ieee_uint32(val) & all_but_sign) == exponent_mask;
}

bool ieee_float_static_lib_detail::isnan(float val)
{
    static constexpr uint32_t all_but_sign = 0x7fff'ffff;
    static constexpr uint32_t exponent_mask = 0x7f80'0000;

    // `exp == all_ones && mantissa > 0`
    return (bit_cast_to_ieee_uint32(val) & all_but_sign) > exponent_mask;
}

uint32_t ieee_float_static_lib_detail::bit_cast_to_ieee_uint32(float val)
{
    return sixit::lwa::bit_cast<uint32_t>(val);
}

bool ieee_float_static_lib_detail::get_sign(float val)
{
    return sixit::lwa::bit_cast<uint32_t>(val) >> 31;
}

bool ieee_float_static_lib_detail::equal_to_zero(float val)
{
    return !(sixit::lwa::bit_cast<uint32_t>(val) << 1);
}

float ieee_float_static_lib_detail::bit_cast_from_ieee_uint32(uint32_t val)
{
    return sixit::lwa::bit_cast<float>(val);
}

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