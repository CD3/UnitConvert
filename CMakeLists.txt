cmake_minimum_required( VERSION 3.13 )

if( TARGET unit_convert)
  return()
endif( TARGET unit_convert )

OPTION( BUILD_UNIT_TESTS "Build unit tests for the library" ON )
OPTION( BUILD_SWIG_BINDINGS "Build bindings for other languages using SWIG" OFF )
OPTION( BUILD_PYBIND11_BINDINGS "Build bindings python using pybind11" OFF )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
set(INSTALL_PATH ${INSTALL_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/externals/install/")

include(function-git_version)
GIT_VERSION(unit_convert)
message( STATUS "unit_convert version: ${unit_convert_VERSION}")
project(unit_convert VERSION ${unit_convert_VERSION})
# need to run configure_file twice
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}/version.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/version.h")
configure_file( "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/version.h"
                "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/version.h")
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/version.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/include/unit_convert/version.h")
configure_file( "${CMAKE_CURRENT_BINARY_DIR}/include/unit_convert/version.h"
                "${CMAKE_CURRENT_BINARY_DIR}/include/unit_convert/version.h")


if( NOT TARGET CONAN_PKG::boost ) # protect against being included
if( EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake" )
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS)
endif()
endif()

if( NOT TARGET CONAN_PKG::boost )
find_package( Boost REQUIRED )
endif()


add_library( unit_convert )
add_library( unit_convert::unit_convert ALIAS unit_convert )
target_sources( unit_convert
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/io.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/units/basic_unit.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/units/si_unit.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/quantities/basic_quantity.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/quantities/si_quantity.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/quantities/add_registry.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/dimensions/si_dimension.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/dimensions/basic_dimension.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/detail/functions.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/unit_registries/basic_unit_registry.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/unit_registries/si_unit_registry.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/unit_registries/si_expression_unit_registry.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/unit_registries/parsers.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/unit_registries/global_unit_registry.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/unit_registries/global_unit_registry.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/unit_registries/unit_registry_base.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/unit_convert/unit_registries/expression_unit_registry.hpp>
  )

target_include_directories( unit_convert  INTERFACE
  $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${${PROJECT_NAME}_BINARY_DIR}>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(unit_convert
                        PUBLIC
                        $<$<TARGET_EXISTS:CONAN_PKG::boost>:CONAN_PKG::boost>
                        $<$<NOT:$<TARGET_EXISTS:CONAN_PKG::boost>>:Boost::boost>
                        $<$<NOT:$<TARGET_EXISTS:CONAN_PKG::boost>>:Boost::disable_autolinking>
                     )
target_compile_features( unit_convert  PUBLIC cxx_std_17 )
# set_target_properties( unit_convert PROPERTIES DEBUG_POSTFIX "-d" )

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/data/unit_definitions.txt" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY )




# LEGACY LIB
add_library( UnitConvert )
add_library( UnitConvert::UnitConvert ALIAS UnitConvert )
target_sources(
  UnitConvert
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/Unit.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/UnitRegistry.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/Unit.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/GlobalUnitRegistry.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/Dimension.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/Dimension.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/GlobalUnitRegistry.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/Quantity.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/UnitRegistry.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/BoostUnitRegistry.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/detail/functions.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/HighLevelFunctions.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/HighLevelFunctions.cpp>
)
target_include_directories( UnitConvert INTERFACE
  $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${${PROJECT_NAME}_BINARY_DIR}>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(UnitConvert
                        PUBLIC
                        $<$<TARGET_EXISTS:CONAN_PKG::boost>:CONAN_PKG::boost>
                        $<$<NOT:$<TARGET_EXISTS:CONAN_PKG::boost>>:Boost::boost>
                        $<$<NOT:$<TARGET_EXISTS:CONAN_PKG::boost>>:Boost::disable_autolinking>
                     )
target_compile_features( UnitConvert PUBLIC cxx_std_17 )
set_target_properties( UnitConvert PROPERTIES DEBUG_POSTFIX "-d" )


if( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR} AND BUILD_UNIT_TESTS)
enable_testing()
add_subdirectory( testing )
endif()

if( BUILD_SWIG_BINDINGS )
  add_subdirectory(swig)
endif()

if( BUILD_PYBIND11_BINDINGS )
  find_package(pybind11 REQUIRED)
  pybind11_add_module( pyUnitConvert $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/pybind11/pyUnitConvert.cpp> )
  target_link_libraries( pyUnitConvert PUBLIC UnitConvert )

  find_package( Python3 REQUIRED COMPONENTS Interpreter)
  add_test (NAME python-tests
    COMMAND ${Python3_EXECUTABLE} -m pytest -s ${PROJECT_SOURCE_DIR}/testing/PythonTests
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )
endif()


# Install and Packaging
install( TARGETS unit_convert EXPORT unit_convertTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
install(DIRECTORY src/
  DESTINATION include
  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
  DESTINATION include
  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)
install(EXPORT unit_convertTargets
  FILE unit_convertTargets.cmake
  NAMESPACE unit_convert::
  DESTINATION cmake
)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/unit_convertConfig.cmake
"include(CMakeFindDependencyMacro)
find_dependency(Boost)
include(\${CMAKE_CURRENT_LIST_DIR}/unit_convertTargets.cmake)
"
  )
include(CMakePackageConfigHelpers)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/unit_convertConfigVersion.cmake
  VERSION ${unit_convert_VERSION}
  COMPATIBILITY SameMajorVersion
  )
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/unit_convertConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/unit_convertConfigVersion.cmake
  DESTINATION cmake
  )

# LEGACY LIBRARY
install( TARGETS UnitConvert EXPORT UnitConvertTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
install(DIRECTORY src/
  DESTINATION include
  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
  DESTINATION include
  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)
install(EXPORT UnitConvertTargets
  FILE UnitConvertTargets.cmake
  NAMESPACE UnitConvert::
  DESTINATION cmake
)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/UnitConvertConfig.cmake
"include(CMakeFindDependencyMacro)
find_dependency(Boost)
include(\${CMAKE_CURRENT_LIST_DIR}/UnitConvertTargets.cmake)
"
  )
include(CMakePackageConfigHelpers)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/UnitConvertConfigVersion.cmake
  VERSION 0.14.1
  COMPATIBILITY SameMajorVersion
  )
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/UnitConvertConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/UnitConvertConfigVersion.cmake
  DESTINATION cmake
  )




INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "")
SET(CPACK_PACKAGE_VENDOR "")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md")
if( ${PROJECT_NAME}_VERSION_MAJOR)
  SET(CPACK_PACKAGE_VERSION_MAJOR ${${PROJECT_NAME}_VERSION_MAJOR})
endif()
if( ${PROJECT_NAME}_VERSION_MINOR)
  SET(CPACK_PACKAGE_VERSION_MINOR ${${PROJECT_NAME}_VERSION_MINOR})
endif()
if( ${PROJECT_NAME}_VERSION_PATCH)
  SET(CPACK_PACKAGE_VERSION_PATCH ${${PROJECT_NAME}_VERSION_PATCH})
endif()
INCLUDE(CPack)



