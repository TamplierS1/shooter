cmake_minimum_required(VERSION 3.17.0)
project(shooter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_BUILD_TYPE Debug)

add_executable(shooter "")
add_subdirectory(src)
add_subdirectory(third_party/rlextras)

set(CXX_COMPILE_FLAGS)
set(CXX_LINK_FLAGS)
list(APPEND CXX_COMPILE_FLAGS -std=c++20 -Wall -Wextra -pedantic -Wno-narrowing -Wno-sign-compare)
list(APPEND CXX_LINK_FLAGS -fuse-ld=gold)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    list(APPEND CXX_COMPILE_FLAGS -fno-omit-frame-pointer
            -fno-optimize-sibling-calls)
    list(APPEND CXX_LINK_FLAGS
            -fno-omit-frame-pointer -fno-optimize-sibling-calls)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    list(APPEND CXX_COMPILE_FLAGS -O3)
endif ()

set(CXX_INCLUDE_DIRS)
set(CXX_LINK_LIBS)
list(APPEND CXX_INCLUDE_DIRS
        "third_party/fmt/include"
        "third_party/raylib/src"
        "third_party/rlextras"
        "third_party/json/single_include"
        "include")

find_package(fmt
        PATHS "third_party")
find_package(raylib
        PATHS "third_party")
list(APPEND CXX_LINK_LIBS raylib fmt::fmt)

target_compile_options(shooter PRIVATE ${CXX_COMPILE_FLAGS})
target_link_options(shooter PRIVATE ${CXX_LINK_FLAGS})
target_include_directories(shooter PRIVATE SYSTEM
        ${CXX_INCLUDE_DIRS})
target_link_libraries(shooter ${CXX_LINK_LIBS})
