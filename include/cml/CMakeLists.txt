set(SUBPROJ_NAME "cml")
set(SUBPROJ_NAMESPACE "mech")

set(${SUBPROJ_NAME}_MAJOR_VERSION 0)
set(${SUBPROJ_NAME}_MINOR_VERSION 1)
set(${SUBPROJ_NAME}_PATCH_VERSION 0)
set(${SUBPROJ_NAME}_VERSION
    ${${SUBPROJ_NAME}_MAJOR_VERSION}.${${SUBPROJ_NAME}_MINOR_VERSION}.${${SUBPROJ_NAME}_PATCH_VERSION})

set(
    ${SUBPROJ_NAME}_HEADERS
    "cml.hh"
    "MilRabPrimeGenerator.hh"
    "PrimeGenerator.hh"
    "IsPrimeGenerator.hh"
    "Math.hh"
    "DefaultRandomGenerator.hh"
    "RandomGenerator.hh"
    "IsRandomGenerator.hh"
    "IsPowerOf2.hh")

# No source file in header-only library 
#set(
#    ${SUBPROJ_NAME}_SOURCES
#    "")
#

set(
    ${SUBPROJ_NAME}_ALL_SRCS
    ${${SUBPROJ_NAME}_HEADERS})

add_library(
    ${SUBPROJ_NAME}
    INTERFACE)

# Enable C++17 on this project
set_target_properties(
    ${SUBPROJ_NAME} PROPERTIES 
    INTERFACE_CXX_STANDARD 17
    INTERFACE_CXX_EXTENSIONS OFF
    INTERFACE_CXX_STANDARD_REQUIRED YES)

target_include_directories(
    ${SUBPROJ_NAME}
    INTERFACE $<BUILD_INTERFACE:${PROJECT_ROOT_DIR}/include>
              $<INSTALL_INTERFACE:include>)
    
# ############################################################### #
# Installing #################################################### #
# ############################################################### #

# Create export targets
install(
    TARGETS ${SUBPROJ_NAME}
    EXPORT  ${SUBPROJ_NAME}-targets)

# Install headers
install(
    FILES       ${${SUBPROJ_NAME}_HEADERS}
    DESTINATION ${${SUBPROJ_NAME}_INSTALL_INCLUDE_PREFIX})

set(SUBPROJ_TARGETS_FILE "${SUBPROJ_NAME}-targets.cmake")

# Create config-targets cmake file
install(
    EXPORT      ${SUBPROJ_NAME}-targets
    FILE        ${SUBPROJ_TARGETS_FILE}
    NAMESPACE   ${SUBPROJ_NAMESPACE}::
    DESTINATION ${${SUBPROJ_NAME}_INSTALL_CMAKE_PREFIX})

# Create config files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/${SUBPROJ_NAME}-config-version.cmake"
    VERSION ${cmake-test-headeronly_VERSION}
    COMPATIBILITY AnyNewerVersion)

configure_package_config_file(
    "${PROJECT_ROOT_DIR}/cmake/${SUBPROJ_NAME}-config.cmake.in"
    "${PROJECT_BINARY_DIR}/${SUBPROJ_NAME}-config.cmake"
    INSTALL_DESTINATION ${${SUBPROJ_NAME}_INSTALL_CMAKE_PREFIX})

# Install config files
install(
    FILES
        "${PROJECT_BINARY_DIR}/${SUBPROJ_NAME}-config.cmake"
        "${PROJECT_BINARY_DIR}/${SUBPROJ_NAME}-config-version.cmake"
    DESTINATION ${${SUBPROJ_NAME}_INSTALL_CMAKE_PREFIX})