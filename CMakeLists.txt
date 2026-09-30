cmake_minimum_required(VERSION 3.24)
project(xgen_crc VERSION 0.1.0 DESCRIPTION "Independent bounded C11 CRC algorithms" LANGUAGES C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(XGCRC_BUILD_CRC8 "Build MAXIM CRC8" ON)
option(XGCRC_BUILD_CRC16 "Build MODBUS CRC16" ON)
option(XGCRC_BUILD_TESTS "Build host GoogleTest and package consumption tests" OFF)
option(XGCRC_ENABLE_COVERAGE "Instrument owned production objects for host coverage" OFF)

if(NOT XGCRC_BUILD_CRC8 AND NOT XGCRC_BUILD_CRC16)
    message(FATAL_ERROR "Enable at least one of XGCRC_BUILD_CRC8 or XGCRC_BUILD_CRC16")
endif()
if(XGCRC_BUILD_TESTS AND (NOT XGCRC_BUILD_CRC8 OR NOT XGCRC_BUILD_CRC16))
    message(FATAL_ERROR "XGCRC_BUILD_TESTS requires both CRC algorithms")
endif()
if(XGCRC_ENABLE_COVERAGE)
    if(CMAKE_CROSSCOMPILING OR NOT XGCRC_BUILD_TESTS)
        message(FATAL_ERROR "Coverage requires an explicit host test build")
    endif()
    if(NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$" OR
       CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        message(FATAL_ERROR "Coverage requires a GCC-compatible host compiler")
    endif()
endif()

configure_file(include/xgen/crc/version.h.in generated/xgen/crc/version.h @ONLY)

function(xgcrc_enable_warnings target)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow)
        target_compile_options(${target} PRIVATE
            $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>)
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    endif()
endfunction()

function(xgcrc_add_algorithm component source)
    add_library(xgcrc_${component} STATIC ${source})
    add_library(xgcrc::${component} ALIAS xgcrc_${component})
    set_target_properties(xgcrc_${component} PROPERTIES
        EXPORT_NAME ${component}
        C_STANDARD 11 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
        XGCRC_VERSION ${PROJECT_VERSION}
        XGCRC_ABI_VERSION 1
        EXPORT_PROPERTIES "XGCRC_VERSION;XGCRC_ABI_VERSION")
    target_compile_features(xgcrc_${component} PUBLIC c_std_11)
    target_include_directories(xgcrc_${component} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    xgcrc_enable_warnings(xgcrc_${component})
    if(XGCRC_ENABLE_COVERAGE)
        target_compile_options(xgcrc_${component} PRIVATE --coverage -O0 -g)
        target_link_options(xgcrc_${component} INTERFACE --coverage)
    endif()
    install(TARGETS xgcrc_${component} EXPORT xgenCrc${component}Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(EXPORT xgenCrc${component}Targets
        FILE xgenCrc-${component}-targets.cmake NAMESPACE xgcrc::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xgen_crc)
endfunction()

set(XGCRC_COMPONENTS "")
if(XGCRC_BUILD_CRC8)
    xgcrc_add_algorithm(crc8 src/crc8.c)
    list(APPEND XGCRC_COMPONENTS crc8)
endif()
if(XGCRC_BUILD_CRC16)
    xgcrc_add_algorithm(crc16 src/crc16.c)
    list(APPEND XGCRC_COMPONENTS crc16)
endif()

install(FILES include/xgen/crc/crc.h
    "${CMAKE_CURRENT_BINARY_DIR}/generated/xgen/crc/version.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xgen/crc)
configure_package_config_file(cmake/xgen_crcConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/xgen_crcConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xgen_crc)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/xgen_crcConfigVersion.cmake"
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMinorVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/xgen_crcConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/xgen_crcConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xgen_crc)

if(XGCRC_BUILD_TESTS)
    if(CMAKE_CROSSCOMPILING)
        message(FATAL_ERROR "Build host tests separately from MCU targets")
    endif()
    enable_language(CXX)
    find_package(GTest 1.16.0 EXACT CONFIG REQUIRED)
    enable_testing()
    add_subdirectory(tests)
endif()
