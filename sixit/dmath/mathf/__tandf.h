/*
 * ====================================================
 * Copyright 2004 Sun Microsystems, Inc.  All Rights Reserved.
 *
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

/* |tan(x)/x - t(x)| < 2**-25.5 (~[-2e-08, 2e-08]). */
#ifndef sixit_dmath_math_operations___tandf_h_included
#define sixit_dmath_math_operations___tandf_h_included

namespace sixit::dmath::mathf
{
    template <typename fp>
    fp __tandf(fp /*double must_be_double*/ x, int odd)
    {
        const fp T[] = {                 // must_be_double
            fp(float(0x15554d3418c99f.0p-54)), /* 0.333331395030791399758 */
            fp(float(0x1112fd38999f72.0p-55)), /* 0.133392002712976742718 */
            fp(float(0x1b54c91d865afe.0p-57)), /* 0.0533812378445670393523 */
            fp(float(0x191df3908c33ce.0p-58)), /* 0.0245283181166547278873 */
            fp(float(0x185dadfcecf44e.0p-61)), /* 0.00297435743359967304927 */
            fp(float(0x1362b9bf971bcd.0p-59)), /* 0.00946564784943673166728 */
        };

        fp z,r,w,s,t,u;   // must_be_double

        z = x*x;
        /*
        * Split up the polynomial into small independent terms to give
        * opportunities for parallel evaluation.  The chosen splitting is
        * micro-optimized for Athlons (XP, X64).  It costs 2 multiplications
        * relative to Horner's method on sequential machines.
        *
        * We add the small terms from lowest degree up for efficiency on
        * non-sequential machines (the lowest degree terms tend to be ready
        * earlier).  Apart from this, we don't care about order of
        * operations, and don't need to to care since we have precision to
        * spare.  However, the chosen splitting is good for accuracy too,
        * and would give results as accurate as Horner's method if the
        * small terms were added from highest degree down.
        */
        r = T[4] + z * T[5];
        t = T[2] + z * T[3];
        w = z*z;
        s = z*x;
        u = T[0] + z * T[1];
        r = (x + s*u) + (s*w)*(t + w*r);
        return odd ? fp(-1.f) / r : r;
    }
} //  sixit::dmath::mathf

#endif // sixit_dmath_math_operations___tandf_h_included
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