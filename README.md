# sixit-dmath
sixit/dmath is a cross-platform deterministic math lib (as ~~seen on TV~~ presented on CppCon24)

## WARNING: APIs are SUBJECT TO CHANGE WITHOUT NOTICE
We are currently at version 0.0.1, and we will NOT commit to APIs being stable at least until v0.1.0... 

## Included files and libs
- TBD

## Supported Platforms
In our speak, "platform" = "CPU+Compiler+OS". Currently, we're supporting {x64|ARM64|RISC-V64|WASM32}, {Clang|MSVC|GCC}, and {Android|iOS|Linux|MacOS|Windows|WASM}. For details, please refer to [Supported platforms for sixit C++ Libs](https://github.com/sixitbb/.github/blob/main/profile/cpp-supported-platforms.md).

## Dependencies, Install and Building
Dependencies and Install:
- [sixit/core](https://github.com/sixitbb/sixit-core/tree/main)
Just put all sixit libs under the same sixit folder.

Building:
sixit/dmath is a HEADER-ONLY LIB, no build is really necessary. 

## Plans for v0.0.2
- code cleanup
- moving exhaustive-range-testing helpers from core
- MAYBE: adding support for WASM32 
