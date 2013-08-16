cmake_minimum_required(VERSION 2.8.8)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CMakeDependentOption)

project(vpn)

add_subdirectory(doc)
add_subdirectory(lib)
add_subdirectory(src)
