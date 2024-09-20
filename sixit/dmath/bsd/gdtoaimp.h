/****************************************************************

The author of this software is David M. Gay.

Copyright (C) 1998-2000 by Lucent Technologies
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

/* This is a variation on dtoa.c that converts arbitary binary
   floating-point formats to and from decimal notation.  It uses
   double-precision arithmetic internally, so there are still
   various #ifdefs that adapt the calculations to the native
   double-precision arithmetic (any of IEEE, VAX D_floating,
   or IBM mainframe arithmetic).
 */

/* On a machine with IEEE extended-precision registers, it is
 * necessary to specify double-precision (53-bit) rounding precision
 * before invoking strtod or dtoa.  If the machine uses (the equivalent
 * of) Intel 80x87 arithmetic, the call
 *	_control87(PC_53, MCW_PC);
 * does this with many compilers.  Whether this or another call is
 * appropriate depends on the compiler; for this to work, it may be
 * necessary to #include "float.h" or another system-dependent header
 * file.
 */

/* strtod for IEEE-, VAX-, and IBM-arithmetic machines.
 *
 * This strtod returns a nearest machine number to the input decimal
 * string (or sets errno to ERANGE).  With IEEE arithmetic, ties are
 * broken by the IEEE round-even rule.  Otherwise ties are broken by
 * biased rounding (add half and chop).
 *
 * Inspired loosely by William D. Clinger's paper "How to Read Floating
 * Point Numbers Accurately" [Proc. ACM SIGPLAN '90, pp. 112-126].
 *
 * Modifications:
 *
 *	1. We only require IEEE, IBM, or VAX double-precision
 *		arithmetic (not IEEE double-extended).
 *	2. We get by with floating-point arithmetic in a case that
 *		Clinger missed -- when we're computing d * 10^n
 *		for a small integer d and the integer n is not too
 *		much larger than 22 (the maximum integer k for which
 *		we can represent 10^k exactly), we may be able to
 *		compute (d*10^k) * 10^(e-k) with just one roundoff.
 *	3. Rather than a bit-at-a-time adjustment of the binary
 *		result in the hard case, we use floating-point
 *		arithmetic to determine the adjustment to within
 *		one bit; only in really hard cases do we need to
 *		compute a second residual.
 *	4. Because of 3., we don't need a large table of powers of 10
 *		for ten-to-e (just some small tables, e.g. of 10^k
 *		for 0 <= k <= 22).
 */

/*
 * #define IEEE_8087 for IEEE-arithmetic machines where the least
 *	significant byte has the lowest address.
 * #define IEEE_MC68k for IEEE-arithmetic machines where the most
 *	significant byte has the lowest address.
 * #define Long int on machines with 32-bit ints and 64-bit longs.
 * #define Sudden_Underflow for IEEE-format machines without gradual
 *	underflow (i.e., that flush to zero on underflow).
 * #define IBM for IBM mainframe-style floating-point arithmetic.
 * #define VAX for VAX-style floating-point arithmetic (D_floating).
 * #define No_leftright to omit left-right logic in fast floating-point
 *	computation of dtoa.
 * #define Check_FLT_ROUNDS if FLT_ROUNDS can assume the values 2 or 3.
 * #define RND_PRODQUOT to use rnd_prod and rnd_quot (assembly routines
 *	that use extended-precision instructions to compute rounded
 *	products and quotients) with IBM.
 * #define ROUND_BIASED for IEEE-format with biased rounding.
 * #define Inaccurate_Divide for IEEE-format with correctly rounded
 *	products but inaccurate quotients, e.g., for Intel i860.
 * #define NO_LONG_LONG on machines that do not have a "long long"
 *	integer type (of >= 64 bits).  On such machines, you can
 *	#define Just_16 to store 16 bits per 32-bit Long when doing
 *	high-precision integer arithmetic.  Whether this speeds things
 *	up or slows things down depends on the machine and the number
 *	being converted.  If long long is available and the name is
 *	something other than "long long", #define Llong to be the name,
 *	and if "unsigned Llong" does not work as an unsigned version of
 *	Llong, #define #ULLong to be the corresponding unsigned type.
 * #define KR_headers for old-style C function headers.
 * #define Bad_float_h if your system lacks a float.h or if it does not
 *	define some or all of DBL_DIG, DBL_MAX_10_EXP, DBL_MAX_EXP,
 *	FLT_RADIX, FLT_ROUNDS, and DBL_MAX.
 * #define MALLOC your_malloc, where your_malloc(n) acts like malloc(n)
 *	if memory is available and otherwise does something you deem
 *	appropriate.  If MALLOC is undefined, malloc will be invoked
 *	directly -- and assumed always to succeed.
 * #define Omit_Private_Memory to omit logic (added Jan. 1998) for making
 *	memory allocations from a private pool of memory when possible.
 *	When used, the private pool is PRIVATE_MEM bytes long:  2304 bytes,
 *	unless #defined to be a different length.  This default length
 *	suffices to get rid of MALLOC calls except for unusual cases,
 *	such as decimal-to-binary conversion of a very long string of
 *	digits.  When converting IEEE double precision values, the
 *	longest string gdtoa can return is about 751 bytes long.  For
 *	conversions by strtod of strings of 800 digits and all gdtoa
 *	conversions of IEEE doubles in single-threaded executions with
 *	8-byte pointers, PRIVATE_MEM >= 7400 appears to suffice; with
 *	4-byte pointers, PRIVATE_MEM >= 7112 appears adequate.
 * #define INFNAN_CHECK on IEEE systems to cause strtod to check for
 *	Infinity and NaN (case insensitively).
 *	When INFNAN_CHECK is #defined and No_Hex_NaN is not #defined,
 *	strtodg also accepts (case insensitively) strings of the form
 *	NaN(x), where x is a string of hexadecimal digits (optionally
 *	preceded by 0x or 0X) and spaces; if there is only one string
 *	of hexadecimal digits, it is taken for the fraction bits of the
 *	resulting NaN; if there are two or more strings of hexadecimal
 *	digits, each string is assigned to the next available sequence
 *	of 32-bit words of fractions bits (starting with the most
 *	significant), right-aligned in each sequence.
 *	Unless GDTOA_NON_PEDANTIC_NANCHECK is #defined, input "NaN(...)"
 *	is consumed even when ... has the wrong form (in which case the
 *	"(...)" is consumed but ignored).
 * #define MULTIPLE_THREADS if the system offers preemptively scheduled
 *	multiple threads.  In this case, you must provide (or suitably
 *	#define) two locks, acquired by ACQUIRE_DTOA_LOCK(n) and freed
 *	by FREE_DTOA_LOCK(n) for n = 0 or 1.  (The second lock, accessed
 *	in pow5mult, ensures lazy evaluation of only one copy of high
 *	powers of 5; omitting this lock would introduce a small
 *	probability of wasting memory, but would otherwise be harmless.)
 *	You must also invoke freedtoa(s) to free the value s returned by
 *	dtoa.  You may do so whether or not MULTIPLE_THREADS is #defined.
 * #define IMPRECISE_INEXACT if you do not care about the setting of
 *	the STRTOG_Inexact bits in the special case of doing IEEE double
 *	precision conversions (which could also be done by the strtog in
 *	dtoa.c).
 * #define NO_HEX_FP to disable recognition of C9x's hexadecimal
 *	floating-point constants.
 * #define -DNO_ERRNO to suppress setting errno (in strtod.c and
 *	strtodg.c).
 * #define NO_STRING_H to use private versions of memcpy.
 *	On some K&R systems, it may also be necessary to
 *	#define DECLARE_SIZE_T in this case.
 * #define YES_ALIAS to permit aliasing certain double values with
 *	arrays of ULongs.  This leads to slightly better code with
 *	some compilers and was always used prior to 19990916, but it
 *	is not strictly legal and can cause trouble with aggressively
 *	optimizing compilers (e.g., gcc 2.95.1 under -O2).
 * #define USE_LOCALE to use the current locale's decimal_point value.
 */

#ifndef sixit_dmath_bsd_gdtoaimp_h_included
#define sixit_dmath_bsd_gdtoaimp_h_included

#define NO_HEX_FP

#include "gdtoa.h"
#include "../strtod/gd_qnan.h"
#include <bit>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <utility>

#include "sixit/core/core.h"
#include "sixit/core/cpual/integer_math.h"
// #include "sixit/profiler/profiler.h"

//#include "errno.h"
#ifndef DBL_MAX_EXP
#define DBL_MAX_EXP 1024
#endif

#ifdef Bad_float_h

#define DBL_DIG 15
#define DBL_MAX_10_EXP 308
#define FLT_RADIX 2
#define DBL_MAX 1.7976931348623157e+308

#ifndef LONG_MAX
#define LONG_MAX 2147483647
#endif

#else /* ifndef Bad_float_h */
//#include "float.h"
#endif /* Bad_float_h */

#define Scale_Bit 0x10

#ifndef __MATH_H__
//#include "math.h"
#endif

class BigInt {
public:
  BigInt(): value_(0, 0), pow_(0) {};

  BigInt(const uint64_t& l, const uint64_t& h, int p) :
    value_(l, h), pow_(p + 128)
  {
    if (!value_.high)
    {
      value_.high = value_.low;
      value_.low = 0;
      pow_ -= 64;
    }

    int leading_zeros(std::countl_zero(value_.high));
    
    if (leading_zeros) {
      value_.high = (value_.high << leading_zeros) | (value_.low >> (64 - leading_zeros));
      value_.low <<= leading_zeros;
      pow_ -= leading_zeros;
    }
  }

  SIXIT_FORCEINLINE
  void mult(uint64_t other_val, const int& other_pow, BigInt& rv) const
  {
    constexpr uint64_t mask = 0xffffffffffffffffULL >> 55;
    // sixit::profile::probe<"sixit: mult", 6, sixit::profile::usage::profiling> probe;    
    int leading_zeros(std::countl_zero(other_val));
    other_val <<= leading_zeros;

    // probe.template end_named_stage<"clz">();
    rv.value_ = sixit::core::cpual::umul64x64(value_.high, other_val);
    // probe.template end_named_stage<"first multiplication">();       
    rv.pow_ = pow_ + other_pow + 64 - leading_zeros;

    if ((rv.value_.high & mask) == mask) 
    {
      sixit::core::cpual::uint128_t tmp = sixit::core::cpual::umul64x64(value_.low, other_val);
      rv.value_.low += tmp.high;
      rv.value_.high += (rv.value_.low < tmp.high);
    }
    // probe.template end_named_stage<"second multiplication">();

    const uint32_t explicit_bit = !(rv.value_.high >> 63);
    rv.value_.high <<= explicit_bit;
    rv.pow_ -= explicit_bit;
  }

  int get_pow() const { return pow_; }

  const uint64_t& get_high() const { return value_.high; }

  const uint64_t& get_low() const { return value_.low; }

private:
    sixit::core::cpual::uint128_t value_;
    int pow_ = 0;
};

void pow5mult(const uint64_t& value, int e, BigInt& rv);



#ifdef __cplusplus
extern "C" {
#endif

/* The following definition of Storeinc is appropriate for MIPS processors.
 * An alternative that might be better on some machines is
 * #define Storeinc(a,b,c) (*a++ = b << 16 | c & 0xffff)
 */
#define Storeinc(a,b,c) (((unsigned short *)a)[1] = (unsigned short)b, \
((unsigned short *)a)[0] = (unsigned short)c, a++)

/* #define P DBL_MANT_DIG */
/* Ten_pmax = floor(P*log(2)/log(5)) */
/* Bletch = (highest power of 2 < DBL_MAX_10_EXP) / 16 */
/* Quick_max = floor((P-1)*log(FLT_RADIX)/log(10) - 1) */
/* Int_max = floor(P*log(FLT_RADIX)/log(10) - 1) */

#define Exp_shift  20
#define Exp_shift1 20
#define Exp_msk1    0x100000
#define Exp_msk11   0x100000
#define Exp_mask  0x7ff00000
#define P 53
#define Emin (-1022)
#define Exp_1  0x3ff00000
#define Exp_11 0x3ff00000
#define Ebits 11
#define Frac_mask  0xfffff
#define Frac_mask1 0xfffff
#define Ten_pmax 22
#define Bletch 0x10
#define Bndry_mask  0xfffff
#define Bndry_mask1 0xfffff
#define LSB 1
#define Sign_bit 0x80000000
#define Log2P 1
#define Tiny0 0
#define Tiny1 1
#define Quick_max 14
#define Int_max 14

#ifndef Flt_Rounds
#ifdef FLT_ROUNDS
#define Flt_Rounds FLT_ROUNDS
#else
#define Flt_Rounds 1
#endif
#endif /*Flt_Rounds*/

#define ULbits 32
#define kshift 5
#define kmask 31
#define ALL_ON 0xffffffff

#define Kmax 15

#define ULtoQ ULtoQ_D2A
#define ULtof ULtof_D2A
#define ULtod ULtod_D2A
#define ULtodd ULtodd_D2A
#define ULtox ULtox_D2A
#define ULtoxL ULtoxL_D2A
#define dtoa_result dtoa_result_D2A
#define g__fmt g__fmt_D2A
#define hexdig hexdig_D2A
#define hexnan hexnan_D2A
#define nrv_alloc nrv_alloc_D2A
#define rv_alloc rv_alloc_D2A
#define tens tens_D2A

 extern char *dtoa_result;
  extern CONST double tens[];
 extern unsigned char hexdig[];

 extern char *dtoa ANSI((double d, int mode, int ndigits,
			int *decpt, int *sign, char **rve));
 extern void hexdig_init_D2A(Void);
 extern char *nrv_alloc ANSI((char*, char **, int));
 
 extern char *rv_alloc ANSI((int));
 extern char *strcp ANSI((char*, const char*));
 extern double _strtod ANSI((const char *s00, char **se));

#ifdef __cplusplus
}
#endif
/*
 * NAN_WORD0 and NAN_WORD1 are only referenced in strtod.c.  Prior to
 * 20050115, they used to be hard-wired here (to 0x7ff80000 and 0,
 * respectively), but now are determined by compiling and running
 * qnan.c to generate gd_qnan.h, which specifies d_QNAN0 and d_QNAN1.
 * Formerly gdtoaimp.h recommended supplying suitable -DNAN_WORD0=...
 * and -DNAN_WORD1=...  values if necessary.  This should still work.
 * (On HP Series 700/800 machines, -DNAN_WORD0=0x7ff40000 works.)
 */
#define _0 1
constexpr int _1 = 0;
#ifndef NAN_WORD0
#define NAN_WORD0 d_QNAN1
#endif
#ifndef NAN_WORD1
#define NAN_WORD1 d_QNAN0
#endif

#undef SI
#define SI 0

SIXIT_FORCEINLINE constexpr
void b2d(uint64_t mantissa, int exp, double& rv, const int& sign)
{
	if (exp-- > DBL_MAX_EXP)
	{
		rv = sign ? -std::numeric_limits<double>::infinity() : -std::numeric_limits<double>::infinity();
	} else
	{
		const int shift = Ebits + (exp < Emin ? Emin - exp : 0);
		exp = exp < Emin ? Emin - 1 : exp;

		const uint64_t add = ((mantissa >> (shift - 1)) & 1);
		exp = Exp_1 + (exp << Exp_shift);

		mantissa = ((mantissa >> shift) & 0xfffffffffffffULL) | (uint64_t(exp) << 32);
    mantissa += add;

    mantissa |= uint64_t(sign) << 63;

		rv = sixit::guidelines::bit_cast<double>(mantissa);
	}
}

#endif //sixit_dmath_bsd_gdtoaimp_h_included

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

