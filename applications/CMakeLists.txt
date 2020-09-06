cmake_minimum_required( VERSION 3.1 )

project(UnitConvert-Applications)

if( EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake" )
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS)
endif()

find_package(Boost REQUIRED COMPONENTS program_options)

add_subdirectory( .. UnitConvert)

add_executable( uc uc.cpp )
target_link_libraries( uc UnitConvert::UnitConvert Boost::program_options)

