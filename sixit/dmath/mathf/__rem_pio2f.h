/*
* ====================================================
* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
*
* Developed at SunPro, a Sun Microsystems, Inc. business.
* Permission to use, copy, modify, and distribute this
* software is freely granted, provided that this notice
* is preserved.
* ====================================================
*/
/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Serhii Iliukhin
*/

/* __rem_pio2f(x,y)
*
* return the remainder of x rem pi/2 in *y
* use double precision for everything except passing x
* use __rem_pio2_large() for large x
*/

/*
* invpio2:  53 bits of 2/pi
* pio2_1:   first 25 bits of pi/2
* pio2_1t:  pi/2 - pio2_1
*/
#ifndef sixit_dmath_math_operations___rem_pio2f_h_included
#define sixit_dmath_math_operations___rem_pio2f_h_included

#include "__utils.h"
#include "__rem_pio2_large.h"

namespace sixit::dmath::mathf
{
    template <typename fp>
    struct __rem_pio2f_data {
        static constexpr fp toint = fp(float(1.5 / std::numeric_limits<float /*must_be_double*/>::epsilon() ));                      // must_be_double
        static constexpr fp pio4  = fp(float(0x1.921fb6p-1));                // must_be_double
        static constexpr fp invpio2 = fp(float(6.36619772367581382433e-01)); /* 0x3FE45F30, 0x6DC9C883 */  // must_be_double
        static constexpr fp pio2_1  = fp(float(1.57079631090164184570e+00)); /* 0x3FF921FB, 0x50000000 */  // must_be_double
        static constexpr fp pio2_1t = fp(float(1.58932547735281966916e-08)); /* 0x3E5110b4, 0x611A6263 */  // must_be_double
    };

    template <typename fp>
    int __rem_pio2f(fp x, fp *y /*must_be_double*/)
    {
        uint32_t ui = sixit::dmath::fp_traits<fp>::bit_cast_to_ieee_uint32(x);
        fp tx[1],ty[1];  // must_be_double
        fp fn;   // must_be_double
        uint32_t ix;
        int n, sign, e0;

        ix = ui & 0x7fffffff;
        /* 25+53 bit pi is good enough for medium size */
        if (ix < 0x4dc90fdb) {  /* |x| ~< 2^28*(pi/2), medium size */
            /* Use a specialized rint() to get fn. */
            fn = /*(double_t) must_be_doubl*/x * __rem_pio2f_data<fp>::invpio2 + __rem_pio2f_data<fp>::toint - __rem_pio2f_data<fp>::toint;
            n  = int(sixit::dmath::fp_traits<fp>::fp2int64(fn));
            *y = x - fn * __rem_pio2f_data<fp>::pio2_1 - fn * __rem_pio2f_data<fp>::pio2_1t;
            // for float conversation
            ///////////////
            fn = *y * __rem_pio2f_data<fp>::invpio2 + __rem_pio2f_data<fp>::toint - __rem_pio2f_data<fp>::toint;
            n += int(sixit::dmath::fp_traits<fp>::fp2int64(fn));
            *y = *y - fn * __rem_pio2f_data<fp>::pio2_1 - fn * __rem_pio2f_data<fp>::pio2_1t;
            ///////////////
            /* Matters with directed rounding. */
            if (predict_false(*y < -__rem_pio2f_data<fp>::pio4)) {
                n--;
                // fn = fn - fp(1.f);  // TODO
                // *y = x - fn * __rem_pio2f_data<fp>::pio2_1 - fn * __rem_pio2f_data<fp>::pio2_1t;
                *y = *y + __rem_pio2f_data<fp>::pio2_1 + __rem_pio2f_data<fp>::pio2_1t;
            } else if (predict_false(*y > __rem_pio2f_data<fp>::pio4)) {
                n++;
                // fn = fn + fp(1.f);  // TODO
                // *y = x - fn * __rem_pio2f_data<fp>::pio2_1 - fn * __rem_pio2f_data<fp>::pio2_1t;
                *y = *y - __rem_pio2f_data<fp>::pio2_1 - __rem_pio2f_data<fp>::pio2_1t;
            }
            return n;
        }
        if(ix>=0x7f800000) {  /* x is inf or NaN */
            *y = x-x;
            return 0;
        }
        /* scale x into [2^23, 2^24-1] */
        sign = ui >> 31;
        e0 = (ix >> 23) - (0x7f+23);  /* e0 = ilogb(|x|)-23, positive */
        ui = ix - (e0 << 23);
        tx[0] = sixit::dmath::fp_traits<fp>::bit_cast_from_ieee_uint32(ui);
        n  =  __rem_pio2_large(tx, ty, e0, 1, 0);
        if (sign) {
            *y = -ty[0];
            return -n;
        }
        *y = ty[0];
        return n;
    }
} //  sixit::dmath::mathf

#endif // sixit_dmath_math_operations___rem_pio2f_h_included
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