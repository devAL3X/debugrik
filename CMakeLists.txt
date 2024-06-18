cmake_minimum_required(VERSION 3.15)
project(debugrik CXX)

# Add packages from conan
find_package(capstone REQUIRED)
find_package(libdwarf REQUIRED)


add_executable(${PROJECT_NAME}
    src/main.cpp
    src/debugger.cpp
    src/utils.cpp
    src/cfg.cpp
    src/dwarfinfo.cpp
)

target_link_libraries(${PROJECT_NAME} capstone::capstone)
target_link_libraries(${PROJECT_NAME} libdwarf::libdwarf)