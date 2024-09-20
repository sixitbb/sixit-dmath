/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Mykhailo Borovyk
*/
#ifndef sixit_dmath_strtod_parse_json_double_h_included
#define sixit_dmath_strtod_parse_json_double_h_included

#include <assert.h>
#include "../bsd/strtod_classic_base.h"

#include <cstdint>

#include "sixit/profiler/profiler.h"
#include "sixit/core/cpual/simd/simd_byte_buffer64.h"
#include "sixit/core/guidelines.h"

struct DoubleData {
    uint64_t decimal_fraction_y = 0;//IEEE 'fraction'
    int16_t decimal_exp = 0;
    bool minus = false;

    int nd = 0;
    int nd0 = -1;//probably temp
};
 
bool constexpr is_digit(const char32_t& c)
{
    return !(c < '0' || c > '9');
}

template<class Char32Stream>
SIXIT_FORCEINLINE bool _exp_ddata(Char32Stream& in, DoubleData& ddata)
{
    char32_t c = in.readChar();
    bool expMinus = false;
    int exp = 0;
    if (c == '-') {
        expMinus = true;
        c = in.readChar();
    }
    else if (c == '+')
        c = in.readChar();

    while (is_digit(c)) {
        exp = exp * 10 + (c - '0');
        c = in.readChar();
    }
    ddata.decimal_exp += static_cast<int16_t>(expMinus ? -exp : exp);
    return true;
}

template<class Char32Stream>
SIXIT_FORCEINLINE bool _parse_ddata_old(Char32Stream& in, DoubleData &ddata)
{
    sixit::profile::probe<"_parse_data", 5, sixit::profile::usage::profiling> probe;
    char32_t c = in.readChar();

    if (c == '-') {
        ddata.minus = true;
        c = in.readChar();
    } else if (c == '+') 
        c = in.readChar();

    if (c == 'I' || c == 'i')
    {
        c = in.readChar();
        if (c != 'n')
            return false;
        c = in.readChar();
        if (c != 'f')
            return false;
        ddata.decimal_fraction_y = 0x7ff0000000000000;
        return true;
    } else 
    if (c == 'N' || c == 'n')
    {
        c = in.readChar();
        if (c != 'A' && c != 'a')
            return false;
        c = in.readChar();
        if (c != 'N' && c != 'n')
            return false;
        ddata.decimal_fraction_y = 0x7ff8000000000000;
        return true;
    }

    while (c == '0')
        c = in.readChar();

    int dig_count = 19;
    while (dig_count && is_digit(c))

    {
        --dig_count;
        ddata.decimal_fraction_y = ddata.decimal_fraction_y * 10 + c - '0';
        c = in.readChar();
    }

    int zeros = 0;
    if (c == '.')
    {
        ddata.nd0 = 19 - dig_count;
        c = in.readChar();

        if (!ddata.decimal_fraction_y)//zeros after the point
        {
            while (c == '0')
            {
                c = in.readChar();
                ++zeros;
            }
        }

        while (dig_count && is_digit(c))
        {
            --dig_count;
            ddata.decimal_fraction_y = ddata.decimal_fraction_y * 10 + c - '0';
            c = in.readChar();
        }   
    }

    if (c == '.')
    {
        if (ddata.nd0 >= 0)//we already have the point
            return false;
        ddata.nd0 = 19 - dig_count;
        c = in.readChar();

        while (is_digit(c))
        {
            c = in.readChar();
            --dig_count;
        }
    }

    while (is_digit(c)) {
        c = in.readChar();
        dig_count -= ddata.nd0 < 0;
    }
    ddata.nd = 19 - dig_count;
    
    if (ddata.nd0 > -1)
        ddata.decimal_exp = static_cast<int16_t>(ddata.nd0 - ddata.nd - zeros);
     probe.template end_named_stage<"Parse mantissa">();

    if (c == 'e' || c == 'E')
        return _exp_ddata(in, ddata);
     probe.template end_named_stage<"Parse exponent">();
    return true;
}


constexpr static uint64_t tensULL[] = {
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000
};

template<class Char32Stream> 
bool _parse_ddata_new(Char32Stream& instream, DoubleData &ddata)
{
    simd_buffer64 buffer;
    int nn = 7;
    int dig_count = 19;
    
    sixit::profile::probe<"ParseIntoDouble", 5, sixit::profile::usage::profiling> probe;
    if (*instream.p == '-' || *instream.p == '+') {
        ddata.minus = *instream.p == '-';
        instream.p++;
    }

    while (*instream.p == '0')
        ++instream.p;
    
    int zeros = 0;
    if (*instream.p == '.' && !ddata.decimal_fraction_y)
    {
        ddata.nd0 = 19 - dig_count;
        ++instream.p;
        while (*instream.p == '0')
        {
            ++instream.p;
            ++zeros;
        }
    }

    for (;dig_count && (is_digit(*instream.p) || *instream.p == '.'); instream.p += nn, dig_count -=nn)
    {
        buffer.fill_from(*(uint64_t*)(instream.p), 7); // TODO replace it

        int e = buffer.equal_to<'.'>().countl_zero();
        if (e != 7) {
            ddata.nd0 = 19 - dig_count + e;
            buffer.erase_and_shift_left(e);
            instream.p++;
        }

        auto mask = buffer.lt_than<'9' + 1>();
        nn = std::min((mask | buffer.sub<'0'>()).countl_zero(), dig_count);
        ddata.decimal_fraction_y = ddata.decimal_fraction_y * tensULL[nn] + buffer.atoi(nn); 
        buffer.consume(buffer.n_left());
    }

    while (dig_count && is_digit(*instream.p))
    {
        --dig_count;
        ddata.decimal_fraction_y = ddata.decimal_fraction_y * 10 + *instream.p - '0';
        ++instream.p;
    } 

    while (is_digit(*instream.p)) {
        ++instream.p;
        dig_count -= ddata.nd0 < 0;
    }

    ddata.nd = 19 - dig_count;
    if (ddata.nd0 > -1)
        ddata.decimal_exp = static_cast<int16_t>(ddata.nd0 - ddata.nd - zeros);

    probe.template end_named_stage<"Parse mantissa">();

    if (*instream.p == 'e') {
        int16_t e = 0;
        ++instream.p;
        bool eminus = false;
        if (*instream.p == '-' || *instream.p == '+')
        {
            eminus = *instream.p == '-';
            ++instream.p;
        }            

        for (;is_digit(*instream.p); instream.p += nn)
        {
            buffer.fill_from(*(uint64_t*)(instream.p), 7);     // TODO replace it
            nn = buffer.lt_than<'9' + 1>().countl_zero();
            nn = std::min(nn, buffer.sub<'0'>().countl_zero());
            e = e * sixit::guidelines::narrow_cast<int16_t, uint64_t>(tensULL[nn]) + sixit::guidelines::narrow_cast<int16_t, uint64_t>(buffer.atoi(nn)); 
            buffer.consume(buffer.n_left());
        }
        if (eminus)
            ddata.decimal_exp -= e;
        else
            ddata.decimal_exp += e;
    }
    probe.template end_named_stage<"Parse exponent">();
    return true;
}

template<class Char32Stream>
inline double parseJSONNumAsDouble(Char32Stream& in)
{
    // using namespace sixit::profile;
    // probe<"sixit", 6, usage::profiling> probe;

	DoubleData ddata;
    ddata.decimal_exp = 0;
    ddata.decimal_fraction_y = 0;
    ddata.minus = 0;

    _parse_ddata_new(in, ddata);
    // probe.template end_named_stage<"new_parsing ">();
    ddata.decimal_exp = 0;
    ddata.decimal_fraction_y = 0;
    ddata.minus = 0;
    in.resetPtr();
    _parse_ddata_old(in, ddata);
    // probe.template end_named_stage<"old_parsing ">();

    if (ddata.decimal_fraction_y == 0x7ff0000000000000)
        return std::numeric_limits<double>::infinity();
    if (ddata.decimal_fraction_y == 0x7ff8000000000000)
        return std::numeric_limits<double>::quiet_NaN();
    
    double d2;

	if (_strtod_try_fast(ddata.decimal_fraction_y, ddata.minus, ddata.decimal_exp, ddata.nd, d2))
		return d2;

    // probe.template end_named_stage<"fast_path">();
    
    _strtod_big_digit(ddata.minus, ddata.decimal_exp + std::max(0, ddata.nd - 19), ddata.decimal_fraction_y, d2);

    // probe.template end_named_stage<"slow_path">();
    // probe.template end_named_stage<"empty stage">();
	return d2;
}

#endif //sixit_dmath_strtod_parse_json_double_h_included
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