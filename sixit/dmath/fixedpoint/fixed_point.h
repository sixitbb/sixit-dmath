/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Mykhailo Borovyk
*/

#ifndef SIXIT_FIXED_POINT_H
#define SIXIT_FIXED_POINT_H

#include <bit>
#include <stdint.h>
#include <type_traits>

#include <sixit/dmath/traits.h>
#include <sixit/core/lwa.h>
#include <sixit/core/guidelines.h>
#include <sixit/rw/rw.h>


namespace sixit::dmath {

    // default value
    constexpr uint8_t FX_BASE_NBITS = 31;
    constexpr uint8_t FX_BASE_NORMALIZED_BITS = 30;

    // fowrward declarations
    template<uint8_t NBITS, uint8_t NORMALIZED_BITS, class fallback_type>
    class fixed_point;

    template<uint8_t NBITS, uint8_t NORMALIZED_BITS, class fallback_type>
    struct fp_traits<fixed_point<NBITS, NORMALIZED_BITS, fallback_type>>;

    class rational
    {
        template<uint8_t MBITS, uint8_t NORMALIZED_BITS, class fallback_type>
        friend class fixed_point;

    public:

        //friend float operator*(const float& lhs, const rational& rhs) 
        //{
        //    float result = lhs * float(rhs.numerator) / float(rhs.denominator);
        //    return result;
        //}

        int32_t numerator = 0;
        int32_t denominator = 0;

    private:
        inline rational(int32_t _numerator, int32_t _denominator) : numerator(_numerator), denominator(_denominator) {}
        inline rational(int64_t _numerator, int64_t _denominator)
        {
            int32_t n_width = std::bit_width(sixit::lwa::bit_cast<uint64_t>(_numerator >= 0 ? _numerator : -_numerator));
            int32_t d_width = std::bit_width(sixit::lwa::bit_cast<uint64_t>(_denominator >= 0 ? _denominator : -_denominator));

            int32_t shift = std::max(n_width, d_width) - 31;
            if (shift > 0)
            {
                auto dd = 1 << shift;
                numerator = sixit::guidelines::narrow_cast<int32_t>(_numerator / dd);
                denominator = sixit::guidelines::narrow_cast<int32_t>(_denominator / dd);
            }
            else
            {
                numerator = sixit::guidelines::narrow_cast<int32_t>(_numerator);
                denominator = sixit::guidelines::narrow_cast<int32_t>(_denominator);
            }
        }

    public:
        inline rational(const rational& other) = default;
        inline rational& operator=(const rational& other) = default;
    };


    template<uint8_t NBITS, uint8_t NORMALIZED_BITS, class fallback_type>
    class fixed_point
    {
        template<uint8_t NBITS2, uint8_t NORMALIZED_BITS2, class fallback_type2>
        friend class fixed_point;

        friend struct fp_traits<fixed_point<NBITS,NORMALIZED_BITS,fallback_type>>;

        static_assert(NBITS >= 16 && NBITS <= 64);
        static_assert(NORMALIZED_BITS <= NBITS);
        static constexpr bool use32 = NBITS <= 32;
        using underlying_type = typename std::conditional<use32, int32_t, int64_t>::type;
        using usigned_underlying_type = typename std::conditional<use32, uint32_t, uint64_t>::type;

        struct rw_alias
        {
            using value_type = fixed_point<FX_BASE_NBITS, FX_BASE_NORMALIZED_BITS, float>;
            using alias_type = float;

            static alias_type value2alias(const value_type& value)
            {
                return value.to_float();
            }

            static value_type alias2value(alias_type value)
            {
                return { value };
            }
        };

        friend struct sixit::units::dimensional_scalar_rw_alias_helper<sixit::dmath::fixed_point<FX_BASE_NBITS, FX_BASE_NORMALIZED_BITS, float>>;

        inline operator fallback_type() const
        {
            constexpr fallback_type one = fallback_type(underlying_type(1) << (NORMALIZED_BITS - 1));
            return fallback_type(data) / one;
        }

        static inline bool is_int_data_valid(const underlying_type& v)
        {
            return std::bit_width(v < 0 ? usigned_underlying_type(-v) : usigned_underlying_type(v)) < NBITS;
        }

        static inline bool is_fallback_data_valid(usigned_underlying_type umantissa, int32_t mantissa_shift)
        {
            int32_t mantissa_width = int32_t(std::bit_width(umantissa)) + mantissa_shift;
            return mantissa_width < 0 || mantissa_width < NBITS;
        }

    public:

        underlying_type data = 0;

        float to_float() const
        {
            // simple version
            constexpr float one = float(underlying_type(1) << (NORMALIZED_BITS - 1));
            return float(data) / one;

            // bit-precise version
            //if (data == 0)
            //    return 0.0f;
            //constexpr  int32_t shift_right = NORMALIZED_BITS - 24; // FLOAT_NORMALIZED_BITS;
            //bool sign = data < 0;
            //usigned_underlying_type umantissa = usigned_underlying_type(sign ? -data : data);
            //int32_t shift_left = NORMALIZED_BITS - std::bit_width(umantissa);
            //int32_t shift = shift_left - shift_right;
            //usigned_underlying_type umantissa_normalized = shift < 0 ? umantissa >> (-shift) : umantissa << shift;
            //uint32_t umantissa32 = uint32_t(umantissa_normalized) & 0x7fffff;
            //int32_t exp = -shift_left;
            //assert(exp >= -127 && exp < 128);

            //// construct float from mantissa & exp
            //uint32_t res_exp = uint32_t(exp + 127) & 0xff;
            //uint32_t udata = umantissa32 |= (res_exp << 23) | (sign << 31);
            //float result = sixit::lwa::bit_cast<float>(udata);
            //return result;
        }
        
        constexpr fixed_point() : data(0) {};
        inline fixed_point(const fixed_point<NBITS, NORMALIZED_BITS, fallback_type>& other) = default;
        inline fixed_point& operator=(const fixed_point<NBITS, NORMALIZED_BITS, fallback_type>& other) = default;

        // constructors from the other types
        // from fallback type
        inline fixed_point(const fallback_type& fp)
        {
            assert(fp_traits<fallback_type>::isfinite(fp));

            constexpr fallback_type one = fallback_type(underlying_type(1) << (NORMALIZED_BITS - 1));
            fallback_type normalized_value = fp * one;
            underlying_type int_data = sixit::guidelines::round_cast<underlying_type>(float(normalized_value));
            assert(is_int_data_valid(int_data));
            data = int_data;
        }

        // from integer
        inline fixed_point(const underlying_type& _data)
        {
            assert(is_int_data_valid(_data));
            data = _data;
        }

        // from float
        //inline from_float(const float& fp)
        //{
        //    assert(fp_traits<float>::isfinite(fp));

        //    constexpr float one = float(underlying_type(1) << (NORMALIZED_BITS - 1));
        //    float normalized_value = fp * one;
        //    underlying_type int_data = sixit::guidelines::round_cast<underlying_type>(normalized_value);
        //    assert(is_int_data_valid(int_data));
        //    data = int_data;

        //    //// bit-precise version
        //    //int32_t mantissa = fp_traits<fallback_type>::get_mantissa(fp);
        //    //int32_t exp = fp_traits<fallback_type>::get_exp(fp);
        //    //bool sign = mantissa < 0;
        //    //int32_t mantissa_shift = exp + (NORMALIZED_BITS - 24);  // 24 - FLOAT_NORMALIZED_BITS;
        //    //uint32_t umantissa = sign ? uint32_t(-mantissa) : uint32_t(mantissa);
        //    //assert(is_fallback_data_valid(umantissa, mantissa_shift));
        //    //umantissa = mantissa_shift > 0 ? umantissa << mantissa_shift : umantissa >> (-mantissa_shift);
        //    //data = sign ? -underlying_type(umantissa) : underlying_type(umantissa);
        //}

        // from other fixed_point with another NBITS2 and NORMALIZED_BITS2
        template<uint8_t NBITS_OTHER, uint8_t NORMALIZED_BITS_OTHER, class fallback_type_other>
        inline fixed_point(const fixed_point<NBITS_OTHER, NORMALIZED_BITS_OTHER, fallback_type_other>& other)
        {
            // at the moment only conversion with the same NORMALIZED_BITS is allowed
            // technically it's possible to convert from anonther NORMALIZED_BITS
            static_assert(NORMALIZED_BITS_OTHER == NORMALIZED_BITS);

            if constexpr (NBITS_OTHER <= NBITS)
            {
                // conversion to wider type without check
                data = other.data;
            }
            else
            {
                //check from wider type
                usigned_underlying_type umantissa = other.data < 0 ? usigned_underlying_type(-other.data) : usigned_underlying_type(other.data);
                auto mantissa_width = std::bit_width(umantissa);
                assert(mantissa_width < NBITS);

                data = other.data;
            }
        }

        // unary minus
        inline auto operator-() const { return fixed_point<NBITS, NORMALIZED_BITS, fallback_type>(-data); }

        // addition-subtruction available between any NBITS but the same  NORMALIZED_BITS
        template<uint8_t NBITS_OTHER, uint8_t NORMALIZED_BITS_OTHER, class fallback_type_other>
        inline auto operator + (const fixed_point<NBITS_OTHER, NORMALIZED_BITS_OTHER, fallback_type_other>& other) const
        {
            static_assert(NORMALIZED_BITS_OTHER == NORMALIZED_BITS);
            constexpr uint8_t OUT_NBITS = std::max(NBITS, NBITS_OTHER) + 1;

            if constexpr (OUT_NBITS <= 64)
            {
                using result_underlying_type = typename std::conditional<OUT_NBITS <= 32, int32_t, int64_t>::type;
                return fixed_point<OUT_NBITS, NORMALIZED_BITS, fallback_type>(result_underlying_type(data) + result_underlying_type(other.data));
            }
            else
            {
                fallback_type a = fallback_type(*this);
                fallback_type b = fallback_type(other);
                return fallback_type(a + b);
            }
        }
        template<uint8_t NBITS_OTHER, uint8_t NORMALIZED_BITS_OTHER, class fallback_type_other>
        inline auto operator - (const fixed_point<NBITS_OTHER, NORMALIZED_BITS_OTHER, fallback_type_other>& other) const
        {
            static_assert(NORMALIZED_BITS_OTHER == NORMALIZED_BITS);
            constexpr uint8_t OUT_NBITS = std::max(NBITS, NBITS_OTHER) + 1;

            if constexpr (OUT_NBITS <= 64)
            {
                using result_underlying_type = typename std::conditional<OUT_NBITS <= 32, int32_t, int64_t>::type;
                return fixed_point<OUT_NBITS, NORMALIZED_BITS, fallback_type>(result_underlying_type(data) - result_underlying_type(other.data));
            }
            else
            {
                fallback_type a = fallback_type(*this);
                fallback_type b = fallback_type(other);
                return fallback_type(a - b);
            }
        }

        //// multiplication-division
        template<uint8_t NBITS_OTHER, uint8_t NORMALIZED_BITS_OTHER, class fallback_type_other>
        inline auto operator * (const fixed_point<NBITS_OTHER, NORMALIZED_BITS_OTHER, fallback_type_other>& other) const
        {
            static_assert(NORMALIZED_BITS_OTHER == NORMALIZED_BITS);
            constexpr uint8_t OUT_NBITS = NBITS + NBITS_OTHER - 1;
            constexpr uint8_t OUT_NORMALIZED_BITS = NORMALIZED_BITS + NORMALIZED_BITS - 1;

            if constexpr (OUT_NORMALIZED_BITS <= 64 && OUT_NBITS <= 64)
            {
                using result_underlying_type = typename std::conditional<OUT_NBITS <= 32, int32_t, int64_t>::type;
                return fixed_point<OUT_NBITS, OUT_NORMALIZED_BITS, fallback_type>(result_underlying_type(data) * result_underlying_type(other.data));
            }
            else
            {
                fallback_type a = fallback_type(*this);
                fallback_type b = fallback_type(other);
                return fallback_type(a * b);
            }
        }

        template<uint8_t NBITS_OTHER, uint8_t NORMALIZED_BITS_OTHER, class fallback_type_other>
        inline auto operator / (const fixed_point<NBITS_OTHER, NORMALIZED_BITS_OTHER, fallback_type_other>& other) const
        {
            static_assert(NORMALIZED_BITS_OTHER == NORMALIZED_BITS);
            return rational(data, other.data);
        }

        // multiplication-division with rational
        inline auto operator * (const rational& r) const
        {
            fallback_type result = (fallback_type(*this) * fallback_type(r.numerator)) / fallback_type(r.denominator);
            return result;
        }
        inline fallback_type operator / (const rational& r) const
        {
            fallback_type result = (fallback_type(*this) * fallback_type(r.denominator)) / fallback_type(r.numerator);
            return result;
        }

        // comparison: we can compare with ANY other fixed_point_xx, but the same NORMALIZED_BITS
        template<uint8_t NBITS_OTHER, uint8_t NORMALIZED_BITS_OTHER, class fallback_type_other>
        inline bool operator<(const fixed_point<NBITS_OTHER, NORMALIZED_BITS_OTHER, fallback_type_other>& other) const
        {
            static_assert(NORMALIZED_BITS_OTHER == NORMALIZED_BITS);
            return data < other.data; 
        }
        template<uint8_t NBITS_OTHER, uint8_t NORMALIZED_BITS_OTHER, class fallback_type_other>
        inline bool operator>(const fixed_point<NBITS_OTHER, NORMALIZED_BITS_OTHER, fallback_type_other>& other) const
        { 
            static_assert(NORMALIZED_BITS_OTHER == NORMALIZED_BITS);
            return data > other.data; 
        }
        template<uint8_t NBITS_OTHER, uint8_t NORMALIZED_BITS_OTHER, class fallback_type_other>
        inline bool operator<=(const fixed_point<NBITS_OTHER, NORMALIZED_BITS_OTHER, fallback_type_other>& other) const
        { 
            static_assert(NORMALIZED_BITS_OTHER == NORMALIZED_BITS);
            return data <= other.data;
        };
        template<uint8_t NBITS_OTHER, uint8_t NORMALIZED_BITS_OTHER, class fallback_type_other>
        inline bool operator>=(const fixed_point<NBITS_OTHER, NORMALIZED_BITS_OTHER, fallback_type_other>& other) const
        { 
            static_assert(NORMALIZED_BITS_OTHER == NORMALIZED_BITS);
            return data >= other.data;
        };
        template<uint8_t NBITS_OTHER, uint8_t NORMALIZED_BITS_OTHER, class fallback_type_other>
        inline bool operator==(const fixed_point<NBITS_OTHER, NORMALIZED_BITS_OTHER, fallback_type_other>& other) const
        { 
            static_assert(NORMALIZED_BITS_OTHER == NORMALIZED_BITS);
            return data == other.data; 
        }
        template<uint8_t NBITS_OTHER, uint8_t NORMALIZED_BITS_OTHER, class fallback_type_other>
        inline bool operator!=(const fixed_point<NBITS_OTHER, NORMALIZED_BITS_OTHER, fallback_type_other>& other) const
        { 
            static_assert(NORMALIZED_BITS_OTHER == NORMALIZED_BITS);
            return data != other.data;
        }
    };

    template<uint8_t NBITS, uint8_t NORMALIZED_BITS, class fallback_type>
    struct fp_traits<fixed_point<NBITS, NORMALIZED_BITS, fallback_type>>
    {
        static constexpr bool is_valid_fp = true;
        static constexpr bool is_deterministic = true;
        static constexpr bool is_fixed_point = true;
        using fixed_point_type = fixed_point<NBITS, NORMALIZED_BITS, fallback_type>;

        static constexpr bool isnan(const auto& ) { return false; }
        static constexpr float floor(const auto&){ return 0.0f;  }
        static constexpr bool isinf(const auto& ) { return false; }
        static constexpr bool isfinite(const auto& ) { return true; }

        static bool get_sign(const fixed_point_type& val)
        {
            return val.data < 0;
        }

        static auto to_fallback(const fixed_point_type& val) 
        { 
            return fallback_type(val);
        }

        static auto multiply_by_coefficient(const fixed_point_type& fp, const rational& r)
        {
            assert(r.denominator != 0);

            int64_t tmp_data = int64_t(fp.data) * int64_t(r.numerator) * 2;
            tmp_data /= r.denominator;
            bool is_last_bit = tmp_data & 1;
            int64_t last_bit_offset = is_last_bit ? (tmp_data > 0 ? 1 : -1) : 0;
            tmp_data = tmp_data / 2 + last_bit_offset;

            // realtime narrow cast
            int32_t data32 = sixit::guidelines::narrow_cast<int32_t>(tmp_data);
            return fixed_point_type(data32);
        }

        static auto divide_by_coefficient(const fixed_point_type& fp, const rational& r)
        {
            assert(r.numerator != 0);

            int64_t tmp_data = int64_t(fp.data) * int64_t(r.denominator) * 2;
            tmp_data /= r.numerator;
            bool is_last_bit = tmp_data & 1;
            int64_t last_bit_offset = is_last_bit ? (tmp_data > 0 ? 1 : -1) : 0;
            tmp_data = tmp_data / 2 + last_bit_offset;

            // realtime narrow cast
            int32_t data32 = sixit::guidelines::narrow_cast<int32_t>(tmp_data);
            return fixed_point_type(data32);
        }
    };

    // default type with float fallback
    using fx32_float = fixed_point<FX_BASE_NBITS, FX_BASE_NORMALIZED_BITS,float>;

}

namespace sixit::units {

    template<>
    struct dimensional_scalar_rw_alias_helper<sixit::dmath::fixed_point<sixit::dmath::FX_BASE_NBITS, sixit::dmath::FX_BASE_NORMALIZED_BITS, float>>
    {
        using value_type = sixit::dmath::fixed_point<sixit::dmath::FX_BASE_NBITS, sixit::dmath::FX_BASE_NORMALIZED_BITS, float>;
        using alias_type = float;

        static alias_type value2alias(const value_type& value)
        {
            return value.to_float();
        }

        static value_type alias2value(alias_type value)
        {
            return { value };
        }
    };

} // namespace sixit::units


#endif // SIXIT_FIXED_POINT_H

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Mykhailo Borovyk

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