include(GoogleTest)

add_executable(xgcrc_tests unit/test_crc.cpp)
set_target_properties(xgcrc_tests PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_features(xgcrc_tests PRIVATE cxx_std_17)
target_link_libraries(xgcrc_tests PRIVATE xgcrc::crc8 xgcrc::crc16 GTest::gtest_main)
xgcrc_enable_warnings(xgcrc_tests)
gtest_discover_tests(xgcrc_tests DISCOVERY_MODE POST_BUILD
    PROPERTIES LABELS "unit" TIMEOUT 30)

add_library(xgcrc_header_checks OBJECT
    integration/include_crc.c integration/include_version.c
    integration/include_crc.cpp integration/include_version.cpp)
set_target_properties(xgcrc_header_checks PROPERTIES
    C_STANDARD 11 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_link_libraries(xgcrc_header_checks PRIVATE xgcrc::crc8)
xgcrc_enable_warnings(xgcrc_header_checks)

foreach(test_case source_crc8 source_crc16 source_both
        installed_crc8 installed_crc16 installed_both installed_optional
        installed_unknown installed_wrong_version installed_missing)
    add_test(NAME xgcrc_${test_case}
        COMMAND "${CMAKE_COMMAND}"
            "-DXGCRC_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
            "-DXGCRC_TEST_ROOT=${PROJECT_BINARY_DIR}/integration"
            "-DXGCRC_TEST_CASE=${test_case}"
            "-DXGCRC_C_COMPILER=${CMAKE_C_COMPILER}"
            "-DXGCRC_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DXGCRC_GENERATOR=${CMAKE_GENERATOR}"
            "-DXGCRC_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DXGCRC_GENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            "-DXGCRC_CONFIG=$<CONFIG>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/integration/check_consumer.cmake")
    set_tests_properties(xgcrc_${test_case} PROPERTIES LABELS "integration" TIMEOUT 120)
endforeach()
