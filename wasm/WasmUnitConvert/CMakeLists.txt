cmake_minimum_required( VERSION 3.1 )

project(UnitConvertWasm)

add_subdirectory( ../.. UnitConvert)
file(COPY ../../testing/JestTests/jest.config.js DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ../../testing/JestTests/WasmUnitConvert.test.js DESTINATION ${CMAKE_BINARY_DIR})

add_executable( WasmUnitConvert WasmUnitConvert.cpp )
# set_target_properties( WasmUnitConvert PROPERTIES
#   LINK_FLAGS
#     )
target_link_libraries( WasmUnitConvert UnitConvert::UnitConvert
    "--bind"
    "-s WASM=1"
    "-s MODULARIZE=1"
    "-s EXPORT_NAME='WasmUnitConvert'"
    "-s DISABLE_EXCEPTION_CATCHING=0"
    "-s ALLOW_MEMORY_GROWTH=0"

  )

