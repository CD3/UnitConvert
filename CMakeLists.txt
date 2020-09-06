cmake_minimum_required( VERSION 3.13 )

if( TARGET UnitConvert )
  return()
endif( TARGET UnitConvert )

OPTION( BUILD_UNIT_TESTS "Build unit tests for the library" ON )
OPTION( BUILD_SWIG_BINDINGS "Build bindings for other languages using SWIG" OFF )
OPTION( BUILD_PYBIND11_BINDINGS "Build bindings python using pybind11" ON )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
set(INSTALL_PATH ${INSTALL_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/externals/install/")

include(function-git_version)
GIT_VERSION(UnitConvert)
message( STATUS "UnitConvert version: ${UnitConvert_VERSION}")
project(UnitConvert VERSION ${UnitConvert_VERSION})
# need to run configure_file twice
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}/version.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/version.h")
configure_file( "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/version.h"
                "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/version.h")


if( EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake" )
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
endif()

find_package( Boost COMPONENTS filesystem REQUIRED )

string( REGEX REPLACE "^lib" "" LIB_NAME ${PROJECT_NAME} )
add_library( ${LIB_NAME} )
add_library( ${PROJECT_NAME}::${LIB_NAME} ALIAS ${LIB_NAME} )
target_sources(
  ${LIB_NAME}
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
)
target_include_directories( ${LIB_NAME} INTERFACE
  $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${${PROJECT_NAME}_BINARY_DIR}>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(${LIB_NAME}
                        PUBLIC
                        Boost::boost
                        Boost::filesystem
                        Boost::disable_autolinking
                     )
target_compile_features( ${LIB_NAME} PUBLIC cxx_std_11 )
set_target_properties( ${LIB_NAME} PROPERTIES DEBUG_POSTFIX "-d" )

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/data/unit_definitions.txt" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY )

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
install( TARGETS ${LIB_NAME} EXPORT ${PROJECT_NAME}Targets
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
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION cmake
)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
"include(CMakeFindDependencyMacro)
find_dependency(Boost COMPONENTS filesystem)
include(\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Targets.cmake)
"
  )
include(CMakePackageConfigHelpers)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${${PROJECT_NAME}_VERSION}
  COMPATIBILITY SameMajorVersion
  )
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
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



