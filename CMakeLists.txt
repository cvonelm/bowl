# SPDX-FileCopyrightText: 2026 Christian von Elm <christian.von_elm@tu-dresden.de>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.11)
project(bowl VERSION 0.0.1)


add_library(bowl INTERFACE)
target_include_directories(bowl INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>")

add_library(Bowl::bowl ALIAS bowl)
if(PROJECT_IS_TOP_LEVEL)
    find_package(Catch2 REQUIRED)
    include(CTest)
    include(Catch)

    add_executable(tests tests/test.cpp)
    target_link_libraries(tests PRIVATE Catch2::Catch2WithMain bowl)
    target_compile_options(tests PRIVATE --coverage)
    target_link_options(tests PRIVATE --coverage)

    catch_discover_tests(tests)

    add_executable(example example/example.cpp)
    target_link_libraries(example PRIVATE bowl)


endif()

include(GNUInstallDirs)
install(
    DIRECTORY "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/bowl>"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "*.hpp"
)
