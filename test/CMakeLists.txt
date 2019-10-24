set(SUBPROJ_NAME "cml_test")

set(
    ${SUBPROJ_NAME}_HEADERS
    "")

set(
    ${SUBPROJ_NAME}_SOURCES
    "test.cpp")

set(
    ${SUBPROJ_NAME}_ALL_SRCS
    ${${SUBPROJ_NAME}_HEADERS}
    ${${SUBPROJ_NAME}_SOURCES})

add_executable(${SUBPROJ_NAME} ${${SUBPROJ_NAME}_ALL_SRCS})

# Enable C++17 on this project
set_target_properties(
    ${SUBPROJ_NAME} PROPERTIES 
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED YES)
    
# Set specific properties
set_target_properties(
    ${SUBPROJ_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
    OUTPUT_NAME              "${SUBPROJ_NAME}$<$<CONFIG:Debug>:d>")
    
# Set include directories for this project and if it will use as installing
target_include_directories(
    ${SUBPROJ_NAME}
    PRIVATE   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
              $<INSTALL_INTERFACE:include>)

find_package(fbi CONFIG REQUIRED)

# Set prime_generator as dependency
target_link_libraries(
    ${SUBPROJ_NAME} 
    cml
    mech::fbi)
    
# ############################################################### #
# Installing #################################################### #
# ############################################################### #

# Create export targets
install(
    TARGETS ${SUBPROJ_NAME}
    EXPORT  ${SUBPROJ_NAME}-targets)

# Set out paths
install(
    TARGETS ${SUBPROJ_NAME}
    RUNTIME DESTINATION  ${${SUBPROJ_NAME}_INSTALL_BIN_PREFIX}
    ARCHIVE DESTINATION  ${${SUBPROJ_NAME}_INSTALL_LIB_PREFIX}
    LIBRARY DESTINATION  ${${SUBPROJ_NAME}_INSTALL_LIB_PREFIX})