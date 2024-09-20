/****************************************************************

The author of this software is David M. Gay.

Copyright (C) 1998, 1999 by Lucent Technologies
All Rights Reserved

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the name of Lucent or any of its entities
not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior
permission.

LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.

****************************************************************/

/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Serhii Iliukhin
*/

#ifndef sixit_dmath_bsd_strtod_classic_base_h_included
#define sixit_dmath_bsd_strtod_classic_base_h_included

#include "gdtoaimp.h"

// #include "sixit/profiler/profiler.h"
SIXIT_FORCEINLINE
bool _strtod_try_fast(const uint64_t &decimal_fraction_y, int sign, int e, int nd, double &ret_d)
{
	if (nd <= DBL_DIG
		&& Flt_Rounds == 1
		&& e >= -Ten_pmax && e <= Ten_pmax) 
	{
		
		if (e >= 0)
			ret_d = decimal_fraction_y * tens[e];
		else
			ret_d = decimal_fraction_y / tens[-e];

		if (sign)
			ret_d = -ret_d;
			
		return true;
	}
		
	return false;
}

SIXIT_FORCEINLINE
bool _strtod_big_digit(const int& sign, const int& e,  const uint64_t& value, double& ret_d)
{
	// using namespace sixit::profile;
	// probe<"sixit: slow_path", 4, usage::profiling> probe;
	BigInt res_b2;
	pow5mult(value, e, res_b2);
	// probe.template end_named_stage<"multiplication by power of five">();
	b2d(res_b2.get_high(), res_b2.get_pow(), ret_d, sign);
	// probe.template end_named_stage<"convertation to double">();
	// probe.endStage();
	return true;
}

#endif //sixit_dmath_bsd_strtod_classic_base_h_included
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

