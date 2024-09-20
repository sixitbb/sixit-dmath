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
#ifndef sixit_dmath_math_operations___cosdf_h_included
#define sixit_dmath_math_operations___cosdf_h_included

#include "__utils.h"

namespace sixit::dmath::mathf
{
    template <typename fp>
    fp __cosdf(fp x /*must_be_double*/)
    {
        fp C0  = fp(float(-0x1ffffffd0c5e81.0p-54)); /* -0.499999997251031003120 */ // must_be_double
        fp C1  = fp(float( 0x155553e1053a42.0p-57)); /*  0.0416666233237390631894 */ // must_be_double
        fp C2  = fp(float(-0x16c087e80f1e27.0p-62)); /* -0.00138867637746099294692 */ // must_be_double
        fp C3  = fp(float( 0x199342e0ee5069.0p-68)); /*  0.0000243904487962774090654 */ // must_be_double

        fp r, w, z; // must_be_double

        /* Try to optimize for parallel evaluation as in __tandf.c. */
        z = x * x;
        w = z * z;
        r = C2 + z * C3;
        return ((fp(1.0) + z * C0) + w * C1) + (w * z) * r;
    }
} //  sixit::dmath::mathf

#endif // sixit_dmath_math_operations___cosdf_h_included
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