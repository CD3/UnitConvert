cmake_minimum_required( VERSION 3.1 )

set( binDir ${CMAKE_BINARY_DIR}/testBin )
set( buildDir ${CMAKE_BINARY_DIR} )


add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/catch)

# Find all sources.
file( GLOB_RECURSE unitTest_SOURCES
      RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
      "./CatchTests/*.cpp" )
message(STATUS "Detected Catch-based Unit Test Sources:")
foreach(testSrc ${unitTest_SOURCES})
  message(STATUS "  ${testSrc}" )
endforeach()

set( testName ${PROJECT_NAME}_CatchTests )
add_executable(${testName} ${unitTest_SOURCES})
target_compile_definitions( ${testName} PRIVATE -DTESTING )
target_link_libraries(${testName} Catch)
target_link_libraries(${testName} UnitConvert::UnitConvert)
target_include_directories( ${testName} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include" )
target_compile_definitions( ${testName} PUBLIC UNITCONVERT_NO_BACKWARD_COMPATIBLE_NAMESPACE)
target_compile_options( ${testName} PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Werror -Wall -Wno-unknown-pragmas>
     $<$<CXX_COMPILER_ID:MSVC>: /W4>)
set_target_properties(${testName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${binDir})
set_target_properties(${testName} PROPERTIES CXX_STANDARD 17)

add_test(NAME ${testName} 
         WORKING_DIRECTORY ${buildDir}
         COMMAND ${binDir}/${testName} )

file(COPY PythonTests DESTINATION ${binDir})

