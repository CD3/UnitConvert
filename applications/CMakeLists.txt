cmake_minimum_required( VERSION 3.1 )

project(UnitConvert-Applications)

if( EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake" )
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS)
endif()

if( NOT TARGET CONAN_PKG::boost )
find_package( Boost REQUIRED )
endif()

add_subdirectory( .. UnitConvert)

add_executable( uc uc.cpp )
target_link_libraries( uc UnitConvert::UnitConvert
                        $<$<TARGET_EXISTS:CONAN_PKG::boost>:CONAN_PKG::boost>
                        $<$<NOT:$<TARGET_EXISTS:CONAN_PKG::boost>>:Boost::program_options>
  )

