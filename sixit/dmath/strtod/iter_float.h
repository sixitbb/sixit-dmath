/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors:
*/
#ifndef sixit_dmath_strtod_iter_float_h_included
#define sixit_dmath_strtod_iter_float_h_included

//float min subnormal
//EXP 0
// 
//fraction 1 = 0.00000000000000000000001b = 2^(-23)
// 
// 
//SINGLE PREC
//subnormal (-1)^sign * 2^(-126) * 0.fraction
//normal (-1)^sign * 2^(EXP-127) * 1.fraction
//minimal positive subnormal = 2^(-126) * 2^(-23) = 2^(-149) ~= 1.4012984643248170709237295832899e-45
//minimal positive normal = 2^(-126) * (1) ~= 1.1754943508222875079687365372222e-38
//next positive normal = 2^(-126) * (1 + 2^(-23)) = 2^(-126) + 2^(-149) ~= 1.1754944909521339404504436295952e-38
// 
//DOUBLE PREC
//subnormal (-1)^sign * 2^(-1022) * 0.fraction
//normal (-1)^sign * 2^(EXP-1023) * 1.fraction
//minimal positive subnormal = 2^(-1022) * 2^(-52) = 2^(-1074) ~= 4.9406564584124654417656879286822e-324
//minimal positive normal = 2^(-1022) * (1) ~= 2.2250738585072013830902327173324e-308

#include <cstdint>
#include <cmath>
#include <cstdlib>

union iterate_float
{
    iterate_float(){}

    void set(float _f) { f = _f; }
    void set(int32_t _i) { i = _i; }

    float next() { float ret = f;  i += 1;  return ret; }

    bool positive() const { return (i & 0x80000000) == 0; }
  //  int32_t significand() const { return i & ((1 < > 23) & 0xFF; }

    bool is_normal() { return std::isnormal(f); }
    bool is_nan() { return std::isnan(f); }
    bool is_inf() { return std::isinf(f); }

    int32_t i;
    float f;

    struct
    {
        uint32_t mantissa : 23;
        uint32_t exponent : 8;
        uint32_t sign : 1;
    } parts;
};

union iterate_double
{
    iterate_double() {}

    void set_min_normal() { i = 0x10'0000'0000'0000; }//bit # 52

    void set(double _f) { f = _f; }
    void set(int64_t _i) { i = _i; }

    double rand() 
    { 
        i = int64_t(std::rand());
        i <<= 32;
        i |= std::rand(); 
        return f;
    }

    double next() { double ret = f;  i += 1;  return ret; }

    bool positive() const { return (i & 0x8000000000000000) == 0; }

    bool is_normal() { return std::isnormal(f); }
    bool is_nan() { return std::isnan(f); }
    bool is_inf() { return std::isinf(f); }

    int64_t i;
    double f;

    struct
    {
        uint64_t mantissa : 52;
        uint64_t exponent : 11;
        uint64_t sign : 1;
    } parts;
};

#endif //sixit_dmath_strtod_iter_float_h_included
/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors:

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