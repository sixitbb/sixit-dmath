# sixit-dmath
sixit/dmath is a cross-platform deterministic math lib (as ~~seen on TV~~ presented on CppCon24)

**[Slides from Sherry's talk on CppCon24 on Sep 20](https://github.com/sixitbb/.github/blob/main/profile/FP%20Determinism.pdf)**

## The Big Idea
The idea is to provide a library which (to the best of our knowledge) allows for fully<sup>(1)</sup> deterministic floating-point calculations across all our supported platforms (currently: 4 CPU architectures, 3 compilers, and 6 OS's). TBD: add link to CppCon presentation as soon as it is out. 

dmath library itself provides cross-platform implementations for major math functions (we're using a templatized version of well-known [MUSL library](https://www.musl-libc.org)). Currently, it takes one of the following classes as template parameter:
- `float` (yes, just usual C++ `float`) - in this case behavior is❗**NOT DETERMINISTIC**❗(in other words - with float as a template parameter, dmath becomes just a yet another math lib; it is still useful for libs such as sixit/geometry which are templatized on the same set of types).
- `sixit::dmath::ieee_float_inline_asm` - so far it seems our best bet. DOES NOT rely on compiler flags (as far as we know). Average Performance is roughly 0.75-0.90 of that of float.
   - As MSVC doesn't support inline asm, we're using intrinsics there instead; however, intrinsic-based implementation looks a bit more shaky (we had to resort to some trickery to prevent compiler from optimizing intrinsics and breaking determinism). 
- `sixit::dmath::ieee_float_if_semicolon_is_respected` - relies on "sequencing" clause in the C++ standard to ensure deterministic behavior. Average Performance is roughly 0.4-0.9 of that of float. ❗**to use it, you MUST disable non-conformant flags such as fast-math or contract=fast for YOUR WHOLE PROJECT** ❗(specific flags TBD)
- `sixit::dmath::ieee_float_if_strict` - relies on "strict" floating-point behavior Average Performance is roughly 0.4-0.9 of that of float. ❗**to use it, you MUST enforce "strict fp" compiler flags for YOUR WHOLE PROJECT** ❗(specific flags TBD)
- `sixit::dmath::ieee_float_static_lib` - using static lib (actually, standalone .cpp file) to ensure determinism. Average Performance is roughly 0.33-0.65 of that of float. ❗**to use it, you MUST disable LTO** ❗(specific flags TBD)
- `sixit::dmath::ieee_float_shared_lib` - using shared lib to ensure determinism. Average Performance is roughly 0.33-0.55 of that of float. NOT supported for WASM32. 
- `sixit::dmath::ieee_float_soft` - "soft float" implementation based on an excellent [Berkeley Soft Float](https://github.com/ucb-bar/berkeley-softfloat-3) lib . Unconditionally and unequvocally DETERMINISTIC, and works EVERYWHERE, but is pretty slow. Average Performance is roughly 0.17-0.25 of that of float.
   + _NB: we were forced to incorporate it, as we're planning to provide constexpr versions for the functions_

<sup>(1)</sup> - except maybe when dealing with NaNs

## WARNING: strict proofs are plain IMPOSSIBLE in this field
While ALL our implementations pass ALL our tests (if applicable, under restrictions listed above), it is next to impossible to provide any strict guarantees. 
- ❗**IF YOU RUN INTO A DISCREPANCY AND CAN PROVIDE A REPRO - PLEASE PLEASE PLEASE DO IT** :pray: :pray: :pray:❗

## WARNING: APIs are SUBJECT TO CHANGE WITHOUT NOTICE
We are currently at version 0.0.1, and we will NOT commit to APIs being stable at least until v0.1.0... 

## Supported Platforms
In our speak, "platform" = "CPU+Compiler+OS". Currently, we're supporting {x64|ARM64|RISC-V64|WASM32}, {Clang|MSVC|GCC}, and {Android|iOS|Linux|MacOS|Windows|WASM}. For details, please refer to [Supported platforms for sixit C++ Libs](https://github.com/sixitbb/.github/blob/main/profile/cpp-supported-platforms.md).

## Dependencies, Install and Building
Dependencies and Install:
- [sixit/core](https://github.com/sixitbb/sixit-core/tree/main)
Just put all sixit libs under the same sixit folder.

Building:
sixit/dmath is a HEADER-ONLY LIB, no build is really necessary. 

## Current Testing Data
### Correctness

|| float, max flags | float, no-fast-math flags | float, strict-fp flags | All ieee_float_* classes, sum across 3 different sets of optimization flags |
|:---|:---:|:---:|:---:|:---:| 
| GCC/x64 | 17/163 | 16/163 | 14/163 | 0/489 |
| MSVC/x64 | 17/163 | 17/163 | 14/163 | 0/489 |
| Apple Clang/ARM64 | 23/163 | 23/163 | 14/163 | 0/489 |
| GCC/RISC-V | 25/163 | 24/163 | 14/163 | 0/489 |

*Legend: Number of failed tests / number of total tests*

### Performance 
|| ieee_float _soft | ieee_float _shared_lib | ieee_float _static_lib | ieee_float _inline_asm |ieee_float _if_strict_fp | ieee_float _if_semicolon |
|:---|:---:|:---:|:---:|:---:|:---:|:---:| 
| GCC/x64 | 6.03x | 3.67x | 2.62x | **1.19x** | 1.44x | 1.40x |
| MSVC/x64 | 4.83x | 1.89x | 1.87x | 1.33x | 2.20x | **1.31x** |
| Apple-Clang/ARM64 | 5.54x | 2.43x | 3.09x | **1.32x** | 2.47x | 2.39x |
| GCC/RISC-V | 3.97x | 3.07x | 1.55x | 1.10x | **1.09x** | 1.10x |
| Process-wide Deoptimizations | -- | -- | No LTO | -- | No fast-math, no reordering, no FMA | No fast-math, no fp-contract:fast | 

*Legend: Execution times, normalized to built-in float on respective platform, geometric mean over tests*

## Plans for v0.0.2
- code cleanup, including:
   + applying sixit clang-format 
   + renaming gamefloat -> fp
   + consolidating everything strtod-related (bsd folder?) under strtod folder
- releasing working and tested fixed_point (also moving it under dmath/fp folder).
- providing `double` versions - and fixing technical debt non-deterministic issue for exp() function
- making `fp_traits<fp>::is_deterministic()` to return enum {guaranteed|assumed|tested-ok|to-be-tested|tested-failed|non-deterministic} and provide fp_traits::test_is_deterministic where applicable (at least LTO and contract flags _seem_ to be detectable).
- making working/usable deterministic strtod
- cleaning up softfloat: at least changing to C++-standard thread_local, moving under dmath/fp folder, removing pointers-to-functions-to-remove-inline, removing everything but 32- and 64-bits (in particular, removing everything 16-, 80-, and 128-bit), making everything constexpr, moving as much as possible to the namespace (incl. replacing #defines with constexpr int's).
- moving exhaustive-range-testing helpers from core
- MAYBE: adding support for WASM32 (in particular, for softfloat: sorting out current WASM32->x86 (?))

## WIP and Future plans
WIP:
- supporting `double` (C++ `double` +6 deterministic versions of sixit::dmath::ieee_double_*, similar to ieee_float_* ones).
- providing sophisticated fixed-point-with-fallback (very useful for geometry)
- providing simd_float, allowing for things such as simultaneous calculation of sin(x) for 32 different values of x in parallel.
   + this generalizes into running ANY algorithm (for example, ANY algorithm from sixit/geometry) using SIMD parallelism. 
   + we'll be utilizing whatever-is-found-on-current-CPU (all the way up to AVX-512-FP16) - in the future, with runtime auto-detection at exe level.
