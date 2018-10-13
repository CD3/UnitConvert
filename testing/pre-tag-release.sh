#! /bin/bash

set -e
root=$(git rev-parse --show-toplevel)

cd $root

bindir="$PWD/build-and-test"
function cleanup()
{
  rm -r $bindir
}
trap cleanup EXIT

function copy_bindir()
{
  cp -r $bindir $bindir.error
}
trap copy_bindir ERR

mkdir $bindir
cd $bindir
cmake .. -DCMAKE_INSTALL_PREFIX=$bindir/install
cmake --build .
cmake --build . --target test



# test install

cmake --build . --target install


echo
echo "Building test app"
mkdir app
cd app

cat << EOF > main.cpp
#include <iostream>
#include <UnitConvert.hpp>
#include <UnitConvert/version.h>

int main()
{
  std::cout << "VERSION: " << UnitConvert_VERSION << std::endl;
  std::cout << "FULL VERSION: " << UnitConvert_VERSION_FULL << std::endl;

  UnitRegistry ureg;
  ureg.addBaseUnit<Dimension::Name::Length>("m");

  auto q = ureg.Quantity<double>(100,"m");

  std::cout << "100 m = " << q.to("km").value() << " km" << std::endl;


  return 0;
}
EOF

cat << EOF > CMakeLists.txt
cmake_minimum_required(VERSION 3.1)
add_executable( main main.cpp )
find_package( UnitConvert REQUIRED PATHS ${bindir}/install )
target_link_libraries(main UnitConvert::UnitConvert )
set_target_properties(main PROPERTIES CXX_STANDARD 14)
EOF

mkdir build1
cd build1
cmake .. -DUnitConvert_DIR=${bindir}/install/UnitConvert-*/cmake/
cmake --build .
./main

cd ..

mkdir build2
cd build2
cmake ..
cmake --build .
./main






echo "PASSED"

exit 0
