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


add_executable(debugger_tests
    src/tests.cpp
    src/utils.cpp
)
target_link_libraries(debugger_tests
    gtest_main gmock_main
)

add_test(NAME UtilsTestsSuite COMMAND debugger_tests)