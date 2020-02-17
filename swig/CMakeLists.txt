find_package( SWIG REQUIRED )
include( ${SWIG_USE_FILE} )
set_source_files_properties( UnitConvert.i PROPERTIES CPLUSPLUS ON )

OPTION(PYTHON_BINDINGS "Generate Python bindings" ON)
SET(PYTHON_VERSION "3" CACHE STRING "Python version to generate bindings for.")


if( PYTHON_BINDINGS )
  find_package( Python${PYTHON_VERSION} REQUIRED COMPONENTS Development Interpreter)
  set(SWIG_MODULE_NAME pyUnitConvert)
  set(CMAKE_SWIG_OUTDIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  set(CMAKE_SWIG_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  swig_add_library( pyUnitConvert LANGUAGE python SOURCES UnitConvert.i )
  swig_link_libraries( pyUnitConvert UnitConvert::UnitConvert Python${PYTHON_VERSION}::Python )

  add_test (NAME python-tests
    COMMAND ${Python3_EXECUTABLE} -m pytest -s ${PROJECT_SOURCE_DIR}/testing/PythonTests
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )
endif()
