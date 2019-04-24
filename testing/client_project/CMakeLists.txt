cmake_minimum_required(VERSION 3.10)
project(UnitConvertPackageTest CXX)

find_package(UnitConvert REQUIRED)
add_executable(example example.cpp)
target_link_libraries(example UnitConvert::UnitConvert)
