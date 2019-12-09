- [Description](#description)
- [Dependencies](#dependencies)
- [Build &amp; install](#build-amp-install)
  - [Getting sources](#getting-sources)
  - [CMake configuration](#cmake-configuration)
  - [CMake options](#cmake-options)
  - [Compiling](#compiling)
  - [Installing](#installing)
  - [How to link](#how-to-link)
    - [Find package](#find-package)
    - [Link library](#link-library)

# Description
CML - Crypto Math Library

Student lab. Don't take it seriously.

# Dependencies
- [Googletest](https://github.com/google/googletest)
- [Boost](https://www.boost.org/)
  - Boost.Multiprecision
  - Boost.Random
- [PicoSHA256](https://github.com/okdshin/PicoSHA2) (Build-in cml)

# Build & install
## Getting sources
Download from github:
```bash
$ git clone https://github.com/LazyMechanic/cml
```

## CMake configuration
Create a temporary `build` folder and change your working directory to it:
```bash
$ mkdir build
$ cd build/
```

Make cml
```bash
$ cmake                                 \
    -DCMAKE_BUILD_TYPE=[Debug|Release]  \
    -G "MinGW Makefiles"                \
    ..
```

For build by `MSVC` you need use Visual Studio Prompt:
```bash
$ cmake                                \
    -DCMAKE_BUILD_TYPE=[Debug|Release] \ 
    -G "NMake Makefiles"               \
    ..
```

## CMake options
```bash
# Boost
-DBoost_DIR=path/to/boost/library

# Googletest
-DGTest_DIR=path/to/googletest/library

# Build library tests. OFF by default
-Dcml_BUILD_TESTS=[OFF|ON]

# Installation directory for CMake files. "lib/cmake/cml" by default
-Dcml_INSTALL_CMAKE_PREFIX=prefix/path

# Installation directory for CMake files. "include/cml" by default
-Dcml_INSTALL_INCLUDE_PREFIX=prefix/path
```

Generate specific project, for example `Visual Studio solution` generator:
```bash
-G "Visual Studio 16"
```

## Compiling
To compile (tests for example):
```bash
$ cmake --build .
```

## Installing
To install:
```bash
$ cmake --install . --prefix /path/to/install
```

## How to link
### Find package
Set `-Dcml_DIR` path to the `cml-config.cmake` file:
```bash
-Dcml_DIR=/path/to/directory/with/config
```

Find the package:
```cmake
find_package(cml CONFIG)

if (NOT ${cml_FOUND})
    message(FATAL_ERROR "cml couldn't be located")
endif()
```

### Link library
Link library to target:
```cmake
add_executable(SomeTarget) # Your target
target_link_libraries(
    SomeTarget 
    mech::cml              # cml target
```
Include directory will automatically be added to *Sometarget*