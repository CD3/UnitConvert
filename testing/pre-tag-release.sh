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

function error_exit()
{
  cp -r $bindir $bindir.error
  echo "There was an error."
  echo "Tapped on '$BASH_COMMAND'"
}
trap error_exit ERR

mkdir $bindir
cd $bindir
cmake .. -DCMAKE_INSTALL_PREFIX=$bindir/install
cmake --build .
cmake --build . --target test



# test install

cmake --build . --target install


# TEST APP
cd $bindir
echo
echo "Building test app"
rm -rf app
mkdir app
cd app

cat << EOF > main.cpp
#include <iostream>
#define UNITCONVERT_NO_BACKWARD_COMPATIBLE_NAMESPACE
#include <UnitConvert.hpp>
#include <UnitConvert/GlobalUnitRegistry.hpp>

int main()
{
  UnitRegistry ureg;
  return 0;
}
EOF

cat << EOF > CMakeLists.txt
cmake_minimum_required(VERSION 3.1)
add_executable( main main.cpp )
find_package( UnitConvert REQUIRED )
target_link_libraries(main UnitConvert::UnitConvert )
set_target_properties(main PROPERTIES CXX_STANDARD 14)
EOF

mkdir build1
cd build1
cmake .. -DUnitConvert_DIR=${bindir}/install/cmake/
ret=$(cmake --build . > /dev/null 2>&1; echo $?)
echo $ret
test "$ret" -ne 0

cd ..
cat << EOF > main.cpp
#include <iostream>
#define UNITCONVERT_NO_BACKWARD_COMPATIBLE_NAMESPACE
#include <UnitConvert.hpp>
#include <UnitConvert/GlobalUnitRegistry.hpp>

int main()
{
  UnitConvert::UnitRegistry& ureg = UnitConvert::getGlobalUnitRegistry();
  return 0;
}
EOF

cd build1
cmake .. -DUnitConvert_DIR=${bindir}/install/cmake/
cmake --build .
./main




# BACKWARD COMPATIBILITY
cd $bindir
echo
echo "Building test app using old (no UnitConvert namespace) API"
rm -rf app
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

  auto q = ureg.makeQuantity<double>(100,"m");

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
cmake .. -DUnitConvert_DIR=${bindir}/install/kmake/
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
