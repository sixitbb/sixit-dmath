/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin, Victor Istomin, Anna Kuznetsova
*/

#ifndef sixit_dmath_fp_approximate_eq_h_included
#define sixit_dmath_fp_approximate_eq_h_included


#include "sixit/core/lwa.h"
#include "sixit/dmath/traits.h"

namespace sixit::dmath::test_helpers {

    template<class T>
    struct inexact_eq_counter_holder
    {
        static int& inexact_eq_ctr()
        {
            static thread_local int _inexact_eq_ctr = 0;
            return _inexact_eq_ctr;
        }
    };

    using inexact_eq_counter = inexact_eq_counter_holder<void>;


    template <typename fp, typename fp2>
    inline bool approximate_eq(fp left, fp2 right2, size_t n = 1)
    {
        if constexpr (!sixit::dmath::fp_traits<fp>::is_deterministic)
        {
            const fp epsilon = n * geometry::low_level::mathf::epsilon;
            fp right(right2);

            // exact comparison for infinity
            if (left == right)
                return true;

            inexact_eq_counter::inexact_eq_ctr() += 1;

            // absolute comparison
            if (geometry::low_level::mathf::abs(left - right) <= epsilon)
                return true;
   
            // relative comparison
            if (geometry::low_level::mathf::abs(left - right) / (geometry::low_level::mathf::max(geometry::low_level::mathf::abs(left), geometry::low_level::mathf::abs(right)) + fp(geometry::low_level::mathf::epsilon)) <= epsilon)
                return true;

            return false;
        }
        else
        {
            return left == right2;
        }
    }

    template <typename fp, sixit::units::physical_dimension dim>
    inline bool approximate_eq(sixit::units::dimensional_scalar<fp, dim> left, sixit::units::dimensional_scalar<fp, dim> right, [[maybe_unused]] size_t n)
    {
        if constexpr (!sixit::dmath::fp_traits<fp>::is_deterministic)
        {
            const fp epsilon = n * std::numeric_limits<float>::epsilon();

            // exact comparison for infinity
            if (left == right)
                return true;

            inexact_eq_counter::inexact_eq_ctr() += 1;

            // absolute comparison
            if (geometry::low_level::mathf::abs(left - right) <= sixit::units::dimensional_scalar<fp, dim>(epsilon, sixit::units::internal_constructor_of_dimensional_scalar_from_fp()))
                return true;

            // relative comparison
            if (geometry::low_level::mathf::abs(left - right) / (geometry::low_level::mathf::max(geometry::low_level::mathf::abs(left), geometry::low_level::mathf::abs(right)) +
                sixit::units::dimensional_scalar<fp, dim>(std::numeric_limits<float>::epsilon(), sixit::units::internal_constructor_of_dimensional_scalar_from_fp())) <=
                sixit::units::dimensional_scalar<fp, sixit::units::simple_scalar::dim>(epsilon, sixit::units::internal_constructor_of_dimensional_scalar_from_fp()))
                return true;

            return false;
        }
        else
        {
            return left == right;
        }
    }

} // namespace sixit::dmath::test_helpers


#endif //sixit_dmath_fp_approximate_eq_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin, Victor Istomin, Anna Kuznetsova

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
