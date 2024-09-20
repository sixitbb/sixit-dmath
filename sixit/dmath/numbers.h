/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin, Marcos Bracco
*/

#ifndef sixit_dmath_numbers_h_included
#define sixit_dmath_numbers_h_included

#include "sixit/dmath/traits.h"

namespace sixit::dmath::numbers {

    template<class T>
    struct fp_checker
    {
        static_assert(std::is_floating_point_v<T> || fp_traits<T>::is_valid_fp, "T is not a floating point type");
    };

    template<class T>
    struct e_t : public fp_checker<T>
    {
        static constexpr T value = 2.71828182845904523536028747135267L;
    };
    template <typename FPT>
    inline constexpr FPT e_v = e_t<FPT>::value;

    template<class T>
    struct log2e_t : public fp_checker<T>
    {
        static constexpr T value = 1.44269504088896340735992468100189L;
    };
    template <typename FPT>
    inline constexpr FPT log2e_v = log2e_t<FPT>::value;

    template<class T>
    struct log10e_t : public fp_checker<T>
    {
        static constexpr T value = 0.43429448190325182765112891891661L;
    };
    template <typename FPT>
    inline constexpr FPT log10e_v = log10e_t<FPT>::value;

    template<class T>
    struct pi_t : public fp_checker<T>
    {
        static constexpr T value = 3.14159265358979323846264338327950f;
    };
    template <typename FPT>
    inline constexpr FPT pi_v = pi_t<FPT>::value;

    template<class T>
    struct inv_pi_t : public fp_checker<T>
    {
        static constexpr T value = 0.31830988618379067153776752674503L;
    };
    template <typename FPT>
    inline constexpr FPT inv_pi_v = inv_pi_t<FPT>::value;

    template<class T>
    struct inv_sqrtpi_t : public fp_checker<T>
    {
        static constexpr T value = 0.56418958354775628694807945156077L;
    };
    template <typename FPT>
    inline constexpr FPT inv_sqrtpi_v = inv_sqrtpi_t<FPT>::value;

    template<class T>
    struct ln2_t : public fp_checker<T>
    {
        static constexpr T value = 0.69314718055994530941723212145818L;
    };
    template <typename FPT>
    inline constexpr FPT ln2_v = ln2_t<FPT>::value;

    template<class T>
    struct ln10_t : public fp_checker<T>
    {
        static constexpr T value = 2.30258509299404568401799145468436L;
    };
    template <typename FPT>
    inline constexpr FPT ln10_v = ln10_t<FPT>::value;

    template<class T>
    struct sqrt2_t : public fp_checker<T>
    {
        static constexpr T value = 1.41421356237309504880168872420970L;
    };
    template <typename FPT>
    inline constexpr FPT sqrt2_v = sqrt2_t<FPT>::value;

    template<class T>
    struct sqrt3_t : public fp_checker<T>
    {
        static constexpr T value = 1.73205080756887729352744634150587L;
    };
    template <typename FPT>
    inline constexpr FPT sqrt3_v = sqrt3_t<FPT>::value;

    template<class T>
    struct inv_sqrt3_t : public fp_checker<T>
    {
        static constexpr T value = 0.57735026918962576450914878050196L;
    };
    template <typename FPT>
    inline constexpr FPT inv_sqrt3_v = inv_sqrt3_t<FPT>::value;

    template<class T>
    struct egamma_t : public fp_checker<T>
    {
        static constexpr T value = 0.57721566490153286060651209008240L;
    };
    template <typename FPT>
    inline constexpr FPT egamma_v = egamma_t<FPT>::value;

    template<class T>
    struct phi_t : public fp_checker<T>
    {
        static constexpr T value = 1.61803398874989484820458683436564L;
    };
    template <typename FPT>
    inline constexpr FPT phi_v = phi_t<FPT>::value;

} // namespace sixit::dmath::numbers

/*
* if using std:: constants is desirable, they could be introduces as follows:
# include <numbers>
namespace sixit::dmath::numbers {
template<class FPT>
inline constexpr FPT e_v = std::numbers::e_v<FPT>;
template<class FPT>
inline constexpr FPT log2e_v = std::numbers::log2e_v<FPT>;
template<class FPT>
inline constexpr FPT log10e_v = std::numbers::log10e_v<FPT>;
template<class FPT>
//inline constexpr FPT pi_v = std::numbers::pi_v<FPT>;
template<class FPT>
inline constexpr FPT inv_pi_v = std::numbers::inv_pi_v<FPT>;
template<class FPT>
inline constexpr FPT inv_sqrtpi_v = std::numbers::inv_sqrtpi_v<FPT>;
template<class FPT>
inline constexpr FPT ln2_v = std::numbers::ln2_v<FPT>;
template<class FPT>
inline constexpr FPT ln10_v = std::numbers::ln10_v<FPT>;
template<class FPT>
inline constexpr FPT sqrt2_v = std::numbers::sqrt2_v<FPT>;
template<class FPT>
inline constexpr FPT sqrt3_v = std::numbers::sqrt3_v<FPT>;
template<class FPT>
inline constexpr FPT inv_sqrt3_v = std::numbers::inv_sqrt3_v<FPT>;
template<class FPT>
inline constexpr FPT egamma_v = std::numbers::egamma_v<FPT>;
template<class FPT>
inline constexpr FPT phi_v = std::numbers::phi_v<FPT>;
} // sixit::dmath::numbers
*/






#endif //sixit_dmath_numbers_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin, Marcos Bracco

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
