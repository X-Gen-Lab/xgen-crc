# Exercise source and installed consumption in a separate, fresh build tree.
foreach(required XGCRC_SOURCE_DIR XGCRC_TEST_ROOT XGCRC_TEST_CASE
        XGCRC_C_COMPILER XGCRC_CXX_COMPILER XGCRC_GENERATOR)
    if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
        message(FATAL_ERROR "${required} is required")
    endif()
endforeach()
if(NOT DEFINED XGCRC_CONFIG OR XGCRC_CONFIG STREQUAL "")
    set(XGCRC_CONFIG Debug)
endif()

string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef run_id)
set(work "${XGCRC_TEST_ROOT}/${XGCRC_TEST_CASE}/${run_id}")
file(MAKE_DIRECTORY "${work}")

function(run_checked step)
    execute_process(COMMAND ${ARGN} RESULT_VARIABLE result
        OUTPUT_VARIABLE output ERROR_VARIABLE errors)
    file(WRITE "${work}/${step}.log" "${output}\n${errors}")
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "${step} failed (${result}); log: ${work}/${step}.log\n${output}\n${errors}")
    endif()
endfunction()

set(generator_args -G "${XGCRC_GENERATOR}")
if(DEFINED XGCRC_GENERATOR_PLATFORM AND NOT XGCRC_GENERATOR_PLATFORM STREQUAL "")
    list(APPEND generator_args -A "${XGCRC_GENERATOR_PLATFORM}")
endif()
if(DEFINED XGCRC_GENERATOR_TOOLSET AND NOT XGCRC_GENERATOR_TOOLSET STREQUAL "")
    list(APPEND generator_args -T "${XGCRC_GENERATOR_TOOLSET}")
endif()
set(compiler_args "-DCMAKE_C_COMPILER=${XGCRC_C_COMPILER}"
    "-DCMAKE_BUILD_TYPE=${XGCRC_CONFIG}")
set(consumer_args -S "${XGCRC_SOURCE_DIR}/tests/integration/consumer"
    -B "${work}/consumer" ${generator_args} ${compiler_args}
    "-DCMAKE_CXX_COMPILER=${XGCRC_CXX_COMPILER}"
    -DXGCRC_CONSUMER_CHECK_CXX=ON
    -DCMAKE_FIND_USE_PACKAGE_REGISTRY=OFF
    -DCMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY=OFF)

if(XGCRC_TEST_CASE MATCHES "^source_(crc8|crc16|both)$")
    set(mode "${CMAKE_MATCH_1}")
    list(APPEND consumer_args "-DXGCRC_CONSUMER_SOURCE_DIR=${XGCRC_SOURCE_DIR}")
elseif(XGCRC_TEST_CASE MATCHES "^installed_(crc8|crc16|both|optional|unknown|wrong_version|missing)$")
    set(mode "${CMAKE_MATCH_1}")
    set(build_crc16 ON)
    if(mode STREQUAL "missing")
        set(build_crc16 OFF)
        set(mode crc16)
    endif()
    run_checked(producer_configure "${CMAKE_COMMAND}" -S "${XGCRC_SOURCE_DIR}"
        -B "${work}/producer" ${generator_args} ${compiler_args}
        -DXGCRC_BUILD_TESTS=OFF -DXGCRC_ENABLE_COVERAGE=OFF
        -DXGCRC_BUILD_CRC8=ON "-DXGCRC_BUILD_CRC16=${build_crc16}"
        -DCMAKE_INSTALL_LIBDIR=lib)
    run_checked(producer_build "${CMAKE_COMMAND}" --build "${work}/producer"
        --config "${XGCRC_CONFIG}")
    run_checked(producer_install "${CMAKE_COMMAND}" --install "${work}/producer"
        --config "${XGCRC_CONFIG}" --prefix "${work}/prefix")
    list(APPEND consumer_args "-Dxgen_crc_DIR=${work}/prefix/lib/cmake/xgen_crc")
else()
    message(FATAL_ERROR "Unknown consumer test case '${XGCRC_TEST_CASE}'")
endif()
list(APPEND consumer_args "-DXGCRC_CONSUMER_MODE=${mode}")

if(XGCRC_TEST_CASE MATCHES "^installed_(unknown|wrong_version|missing)$")
    execute_process(COMMAND "${CMAKE_COMMAND}" ${consumer_args}
        RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE errors)
    file(WRITE "${work}/expected_failure.log" "${output}\n${errors}")
    if(result EQUAL 0)
        message(FATAL_ERROR "Invalid package request was accepted: ${XGCRC_TEST_CASE}")
    endif()
    if(XGCRC_TEST_CASE STREQUAL "installed_wrong_version")
        if(NOT "${output}\n${errors}" MATCHES "0\\.2\\.0")
            message(FATAL_ERROR "Failure did not diagnose the requested version: ${errors}")
        endif()
    elseif(NOT "${output}\n${errors}" MATCHES "is not installed; available:")
        message(FATAL_ERROR "Failure did not diagnose the missing component: ${errors}")
    endif()
    message(STATUS "Rejected ${XGCRC_TEST_CASE}; evidence: ${work}")
    return()
endif()

run_checked(consumer_configure "${CMAKE_COMMAND}" ${consumer_args})
run_checked(consumer_build "${CMAKE_COMMAND}" --build "${work}/consumer"
    --config "${XGCRC_CONFIG}")
run_checked(consumer_test "${CMAKE_CTEST_COMMAND}" --test-dir "${work}/consumer"
    -C "${XGCRC_CONFIG}" --output-on-failure --no-tests=error)
message(STATUS "Verified ${XGCRC_TEST_CASE}; evidence: ${work}")
