# Toolchain file for building OpenAce user-space applications on macOS.
# This ensures CMake uses the native Clang compiler provided by Xcode's
# Command Line Tools, rather than a cross-compiler like x86_64-elf-gcc.

set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Specify the compilers
set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

# Point to the macOS SDK for system headers and libraries
set(CMAKE_OSX_SYSROOT /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk)
set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum macOS version")