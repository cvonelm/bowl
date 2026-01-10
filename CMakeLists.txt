# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.11)
project(bowl VERSION 0.0.1)


add_library(bowl INTERFACE)
target_include_directories(bowl INTERFACE include)

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


    include(GNUInstallDirs)

    set(BOWL_HEADERS
        include/bowl/error.hpp
        include/bowl/exception.hpp
        include/bowl/expected.hpp
        include/bowl/maybe_error.hpp
        include/bowl/unexpected.hpp)
    set_target_properties(bowl PROPERTIES PUBLIC_HEADER "${BOWL_HEADERS}")
    install(TARGETS bowl
        PUBLIC_HEADER
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bowl)
endif()
