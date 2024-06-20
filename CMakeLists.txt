cmake_minimum_required(VERSION 3.15)
project(debugrik CXX)

# Add packages from conan
find_package(capstone REQUIRED)
find_package(libdwarf REQUIRED)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)


add_executable(${PROJECT_NAME}
    src/main.cpp
    src/debugger.cpp
    src/utils.cpp
    src/cfg.cpp
    src/dwarfinfo.cpp
    src/disassm.cpp
)

target_link_libraries(${PROJECT_NAME} capstone::capstone)
target_link_libraries(${PROJECT_NAME} libdwarf::libdwarf)


# TESTS

enable_testing()

# Utils
add_executable(debugger_utils_tests
    src/tests.cpp
    src/utils.cpp
)
target_link_libraries(debugger_utils_tests
    gtest_main gmock_main)

add_test(NAME UtilsTestsSuite COMMAND debugger_utils_tests)

# Config
add_executable(debugger_cfg_tests
    src/cfg.cpp
    src/cfg_tests.cpp
    src/utils.cpp
)

target_link_libraries(debugger_cfg_tests
    gtest_main gmock_main)

add_test(NAME CfgTestsSuite COMMAND debugger_cfg_tests)

# Disassm
add_executable(debugger_disassm_tests
    src/disassm.cpp
    src/test_disassm.cpp
)

target_link_libraries(debugger_disassm_tests
    gtest_main gmock_main capstone::capstone)

add_test(NAME DisassmTestsSuite COMMAND debugger_disassm_tests)

