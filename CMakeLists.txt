cmake_minimum_required( VERSION 3.12 )

if( TARGET UnitConvert )
  return()
endif( TARGET UnitConvert )

project( UnitConvert )

#
# Modules
#

find_package( CMakeCommonModules 3.0 REQUIRED )
list(APPEND CMAKE_MODULE_PATH "${CMakeCommonModules_DIR}" )
list(REMOVE_DUPLICATES CMAKE_MODULE_PATH )
include( CMakeCommonModules-macros )

GIT_VERSION()

#
# Options
#

set_PREFIX_CACHE()               # Set CACHE for CMAKE_PREFIX_PATH, and set CMAKE_INSTALL_PREFIX.
set_PROJECT_LIBRARY_TYPE_CACHE() # Set the default for the project's library type.
set_PROJECT_STANDALONE()         # Set ${PROJECT_NAME}_STANDALONE to indicate whether we are being called from another project 

OPTION( BUILD_UNIT_TESTS "Build unit tests for the library" ON )
OPTION( BUILD_BINDINGS "Build bindings for other languages using SWIG" OFF )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
set(INSTALL_PATH ${INSTALL_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/externals/install/")

include(function-git_version)
GIT_VERSION(UnitConvert)
message( STATUS "UnitConvert version: ${UnitConvert_VERSION}")


# need to run configure_file twice
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}/version.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/version.h")
configure_file( "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/version.h"
                "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/version.h")

#
# Dependencies
#

find_package( Boost REQUIRED )

#
# Build
#

add_library( ${PROJECT_NAME} INTERFACE )
add_library( AFRL-RHD::${PROJECT_NAME} ALIAS ${PROJECT_NAME} )

target_sources( ${PROJECT_NAME}
INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/UnitRegistry.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/Unit.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/Unit.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/Quantity.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/BoostUnitRegistry.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/Dimension.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/UnitRegistry.cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/Dimension.hpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UnitConvert/detail/functions.hpp>
)

target_include_directories( ${PROJECT_NAME} 
INTERFACE
  $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${${PROJECT_NAME}_BINARY_DIR}>
  $<INSTALL_INTERFACE:${${PROJECT_NAME}_INSTALL_DIR}/include>
)

target_compile_features( ${PROJECT_NAME} 
INTERFACE
	cxx_std_11 
	)  # let clients know we need c++11

target_link_libraries( ${PROJECT_NAME}
INTERFACE
	Boost::boost
	)

#set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "-d" )

configure_file("${CMAKE_SOURCE_DIR}/data/unit_definitions.txt" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY )

#
# Ancillaries
#

#set( ${PROJECT_NAME}_DO_SWIG OFF)
set(PYTHON_BINDINGS OFF)
enable_testing()
add_ancillaries_RHDO()

#
# Installation
#

install_RHDO( HEADERS_DIRECTORY src/ )

#
# Packaging
#

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



