# About
This code is an adaptation of [Berkley Soft-Float-3e](https://github.com/ucb-bar/berkeley-softfloat-3/) to be a header-only C++ library.

# Usage 
The single entry point header `softfloat_inline.h` contains public function declarations and includes ported SoftFloat types and method implementations into `sixit::dmath::softfloat` namespace.

# Headers
* `softfloat_inline.h`: public entry point. Contains public function declarations;
* `platform.h`: SoftFloat-compatible platform configuration. Usually it does not need to change. Should be considered as a dependency of any other header in this folder;
* `softfloat_defines.h`, `softfloat_types.h`: platform-independent types and macro with no dependencies except `platform.h`;
* `softfloat_specialize.h`, `softfloat_specialize_*.h`: private, platform-specific code entry point and implementations;
* `softfloat_fwd.h`: forward declarations of all *private* functions which allow to place implementations in the arbitrary order;
* `softfloat_functions.h`: both private and public function implementations;

# Scope
* only several f32 functions are ported at the time of initial implementation: `+ - * /`, `< <= ==`, `f32_isSignalingNaN`, `f32_roundToInt`, `f32_to_i64`;

# Changes made:
* all the code resides in the `sixit::dmath::softfloat` namespace;
* library is header-only;
* no logic changes were made;
* several warnings fixed (leveraging `bit_cast` and `static_cast` only);