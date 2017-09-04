project(Zcode)
cmake_minimum_required(VERSION 2.8)

list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

set(ZCODE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Zcode)

find_package(TBB REQUIRED)
include_directories(${TBB_INCLUDE_DIRS})

OPTION(BUILD_TESTS "zcode test suite" OFF)
OPTION(DOWNLOAD_GTEST "build gtest from downloaded sources" OFF)

if(DOWNLOAD_GTEST OR GTEST_SRC_DIR)
    set(BUILD_TESTS ON)
endif()

if(BUILD_TESTS)
    add_subdirectory(test)
endif()
