/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Serhii Iliukhin
*/

#ifndef sixit_dmath_bigint_bigint_h_included
#define sixit_dmath_bigint_bigint_h_included

#include <vector>
#include <cstdint>

#include "sixit/core/cpual/integer_math.h"

namespace sixit 
{
    class bigint 
    {
    public:
        bigint(const uint64_t& value);

        bigint(const std::vector<uint64_t> digit);

        bigint& operator=(const bigint& other) = default;

        bool operator<(const bigint& other) const;
        bool operator==(const bigint& other) const;

        bigint operator+(const bigint& other) const;
        bigint operator*(const bigint& other) const;

    private:
        bigint() = default;
        void remove_leading_zeros();
        
    private:
        std::vector<uint64_t> data;
    };
};

namespace sixit 
{
    bigint::bigint(const uint64_t& value)
    {
        data.push_back(value);
    }

    bigint::bigint(const std::vector<uint64_t> digit) : data(digit)
    {
        remove_leading_zeros();
    }

    void bigint::remove_leading_zeros()
    {
        while (data.size() > 1 && !data.back())
            data.pop_back();
    }

    bool bigint::operator<(const bigint& other) const 
    {
        if (data.size() != other.data.size()) return data.size() < other.data.size();
        
        int counter = int(data.size());
        const uint64_t *left = data.data() + counter - 1;
        const uint64_t *right = other.data.data() + counter - 1;

        while (counter--) 
        {
            if (*left != *right) return *left < *right;
            left--; right--;
        }

        return false;
    }

    bool bigint::operator==(const bigint& other) const 
    {
        if (data.size() != other.data.size()) return false;
        
        int counter = int(data.size());
        const uint64_t *left = data.data() + counter - 1;
        const uint64_t *right = other.data.data() + counter - 1;

        while (counter--) 
        {
            if (*left != *right) return false;
            left--; right--;
        }

        return true;
    }

    bigint bigint::operator+(const bigint& other) const
    {
        bigint result;
        result.data.assign(std::max(result.data.size(), data.size()) + 1, 0);

        const uint64_t *a = data.data();
        const uint64_t *a_end = data.data() + data.size();

        const uint64_t *b = other.data.data();
        const uint64_t *b_end = other.data.data() + other.data.size();

        uint64_t *r = result.data.data();

        while (a < a_end && b < b_end)
        {
            *r += *a + *b;
            *(r + 1) += *r < *a;
            ++a; ++b; ++r;
        }

        if (a < a_end)
        {
            *r += *a;
            *(r + 1) += *r < *a;
            ++a; ++r;   
        }
        
        if (b < b_end)
        {
            *r += *b;
            *(r + 1) += *r < *b;
            ++b; ++r;   
        }

        result.remove_leading_zeros();
        return result;
    }

    bigint bigint::operator*(const bigint& other) const 
    {
        bigint result;
        result.data.assign(data.size() + other.data.size(), 0);

        uint64_t carry = 0;

        const uint64_t *a;
        const uint64_t *a_end;

        const uint64_t *b;
        const uint64_t *b_end;

        if (data.size() < other.data.size())
        {
            a = data.data();
            a_end = data.data() + data.size();
            b = other.data.data();
            b_end = other.data.data() + other.data.size();
        } else 
        {
            a = other.data.data();
            a_end = other.data.data() + other.data.size();
            b = data.data();
            b_end = data.data() + data.size();
        }
        
        uint64_t *r = result.data.data();
        [[maybe_unused]] uint64_t *r_end = result.data.data() + result.data.size();

        for (; a < a_end; ++a, ++r)
        {
            uint64_t *r1 = r;
            const uint64_t *b1 = b;
            for (; b1 < b_end; ++b1, ++r1)
            {
                
                auto mval = sixit::core::cpual::umul64x64(*a, *b1);
                *r1 += mval.low;
                carry = mval.high + (*r1 < mval.low);
                
                uint64_t *r2 = r1 + 1;
                while (carry)
                {
                    *r2 += carry;
                    carry = (*r2++ < carry);
                }
            }
        }
        
        result.remove_leading_zeros();
        return result;        
    }
}

#endif //sixit_dmath_bigint_bigint_h_included

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