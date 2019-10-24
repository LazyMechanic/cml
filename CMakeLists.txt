cmake_minimum_required(VERSION 3.13)

set(_PROJECT_MAJOR_VERSION 0)
set(_PROJECT_MINOR_VERSION 1)
set(_PROJECT_PATCH_VERSION 0)
set(_PROJECT_VERSION
  ${_PROJECT_MAJOR_VERSION}.${_PROJECT_MINOR_VERSION}.${_PROJECT_PATCH_VERSION})

project(cml LANGUAGES CXX VERSION "${_PROJECT_VERSION}")
    
# ############################################################### #
# cml options ################################################### #
# ############################################################### #
option(cml_BUILD_TEST "Build library tests" OFF)
set(
    cml_INSTALL_CMAKE_PREFIX
    "lib/cmake/cml"
    CACHE PATH "Installation directory for CMake files")
set(
    cml_INSTALL_INCLUDE_PREFIX
    "include/cml"
    CACHE PATH "Installation directory for header files")

# Cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/modules")
set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(include/cml)
if (${cml_BUILD_TEST})
    add_subdirectory(test)
endif()
