
find_package( SWIG REQUIRED )
include( ${SWIG_USE_FILE} )
set_source_files_properties( UnitConvert.i PROPERTIES CPLUSPLUS ON )

OPTION(PYTHON_BINDINGS "Generate Python bindings" ON)
SET(PYTHON_VERSION "2" CACHE STRING "Python version to generate bindings for.")


if( PYTHON_BINDINGS )
  find_package( Python${PYTHON_VERSION} REQUIRED COMPONENTS Development)
  swig_add_library( UnitConvert LANGUAGE python SOURCES UnitConvert.i )
  swig_link_libraries( UnitConvert AFRL-RHD::UnitConvert Python${PYTHON_VERSION}::Python )
endif()
