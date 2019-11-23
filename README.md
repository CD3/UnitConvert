# UnitConvert

A small C++ library, inspired by [`pint`](https://pint.readthedocs.io/en/latest/), for doing runtime unit conversions that interoperates with the `Boost.Units` library.

A few examples are given below. You can also go to the [tutorial](doc/Tutorial.md) for a more detailed description of how to use the library.

## Description

`UnitConvert` provides **runtime** unit conversions. This is necessary when, for example, you want to perform a unit conversion based on user input. The library
provides a `UnitRegistry` class (similar to [`pint`](https://pint.readthedocs.io/en/latest/)) that can parse unit strings.


## Getting Started

### Installing (Manual)

`UnitConvert` needs to be compiled and installed in a place that your project can find it. Builds are managed by CMake, and you need to have boost installed.

```bash
$ cd /path/to/UnitConvert/source
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release
$ cmake --build .
$ sudo cmake --build . --target install
```

This will compile and install the library (`libUnitConver.a`) and headers. It
will also install a CMake `*Config.cmake` file that can be used to find
`UnitConvert` in your own CMake project. By default, CMake will install these
under `/usr/local`. You can change the default install location by passing the
`-DCMAKE_INSTALL_PREFIX` command line option to CMake:
```bash
$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/path/to/install/dir
```

### Conan Install

`UnitConvert` releases are also provided as [Conan](https://conan.io) packages. To install via Conan, you need to add a remote
```
$ conan remote add cd3 https://api.bintray.com/conan/cd3/conan-devel
```
Use `conan search` to find the latest release. For example
```
$ conan search UnitConvert -r cd3
Existing package recipes:

UnitConvert/0.5.1@cd3/devel
UnitConvert/0.5.2@cd3/devel
```
The Conan packages set the `UnitConvert_DIR` environment variable to point at the directory containing the CMake `*Config.cmake` file,
so you do not need to make any changes to your CMakeLists.txt file. Just use the `virtualenv` generator and `find_package` will work.

### Using

To use `UnitConvert`, you need to link against the library `libUnitConvert.a`. If you use CMake, add these lines to your `CMakeLists.txt`:
```cmake
find_pacakge(UnitConvert)
...
# define your targets and link against UnitConvert
target_link_libraries(MyTarget UnitConvert::UnitConvert )
```
If CMake complains that it cannot find the `UnitCovnert` package, specify the directory that contains the `UnitConvertConfig.cmake`
explicitly when running CMake for the first time (`/usr/local/cmake/` by default).
```bash
$ cmake .. -DUnitConvert_DIR=/usr/local/cmake
```
If you get an error when running CMake that says
```
  New Boost version may have incorrect or missing dependencies and imported targets
```
Then you need to upgrade your CMake version.

If you're using conan, use the `virtualenv` generator and run CMake inside the
virtual environment. This will point CMake at the correct configuration file.
For example, create a file named conanfile.txt next to your CMakeLists.txt with the following
```
[requires]
UnitConvert/0.5.2@cd3/devel
[generators]
virtualenv
```
Then run (from the build directory)
```bash
$ conan install ..
$ source ./activate.sh
$ cmake ..
```

In your code, include the header `UnitConvert.hpp`.
Then create a unit registry, and add
some units to it. Units can be defined in terms of their dimension, or in terms of other units already defined.
So you will need to define a set of base units in terms of their dimensions first, and then you can define all other units
in terms of the base units (or other derived units that have been defined).
You can then use the registry to create `Quantity` instances, which can be
converted to any other unit defined in the registry, as well as any derived units based on the units in the
registry.

```cpp
#include <catch.hpp>
#include <UnitConvert.hpp>

#include <boost/units/systems/si.hpp>

TEST_CASE("UnitRegistry Example")
{

UnitRegistry ureg;

// add base units to the registry
ureg.addUnit("m = [L]");
ureg.addUnit("kg = [M]");
ureg.addUnit("s = [T]");
ureg.addUnit("K = [THETA]");
ureg.addUnit("mol = [N]");

// add some derived units to the registry
ureg.addUnit("100 cm = 1 m");
ureg.addUnit("1 in = 2.54 cm");
ureg.addUnit("1 ft = 12 in");
ureg.addUnit("1 J = 1 kg*m^2*s^-2");
ureg.addUnit("1 W = 1 J/s");
ureg.addUnit("1 cal = 4.184 J");

Quantity<double> q = ureg.makeQuantity<double>(200, "cm");

// q.value() returns the numerical value of the quantity
CHECK(q.value() == Approx(200));
CHECK(q.to("m").value() == Approx(2));
// millimeter was not defined, but we can use SI-prefixed versions of any unit in the registry
CHECK(q.to("mm").value() == Approx(2000));
// converting to a unit with different dimensions will throw an error
CHECK_THROWS(q.to(ureg.getUnit("s")));




// quantities  can be assigned to new dimensions
q = ureg.makeQuantity<double>(0.25, "s");
CHECK(q.value() == Approx(0.25));
CHECK(q.to("ms").value() == Approx(250));
// conversions have to match the quantities current dimensions.
CHECK_THROWS(q.to(ureg.getUnit("m")));



q = ureg.makeQuantity<double>(100, "cal");
CHECK(q.value() == Approx(100));
CHECK(q.to("J").value() == Approx(100 * 4.184));
// again, we can use SI prefixes
CHECK(q.to("mJ").value() == Approx(100 * 4.184*1000));
// we can convert to arbitrary combinations of defined dimensions
CHECK(q.to("kg*m^2*s^-2").value() == Approx(100 * 4.184));
CHECK(q.to("kg*m^2*ms^-2").value() == Approx(100 * 4.184 / 1000 / 1000));
CHECK(q.to("W*s").value() == Approx(100 * 4.184));
CHECK(q.to("kW*s").value() == Approx(100 * 4.184/1000));

CHECK_THROWS(q.to("m"));



// The unit registry can create Boost.Units quantities.
// So we can easily convert to a unit in one of the Boost.Unit
// systems and then create a Boost.Unit quantity.
q = ureg.makeQuantity<double>(100, "ft");
boost::units::quantity<boost::units::si::length> L = q.to<boost::units::si::length>();
CHECK(boost::units::quantity_cast<double>(L) == Approx(30.48));

}

```


There is also a `BoostUnitRegistry`, which adds more `Boost.Units` support.
When you create the unit registry, pass it a `Boost.Units` system, and it will automatically
populate the registry with the base units of the system. All prefixed versions are also added.
You can add new units as before, or by passing in an instance of a boost unit
(i.e. `boost::unit::gcs::length`). If you use a boost unit, the string
representation of the unit will be added to the registry.
```cpp
#include <catch.hpp>
#include <UnitConvert.hpp>

#include <boost/units/systems/si.hpp>
#include <boost/units/systems/cgs.hpp>

TEST_CASE("BoostUnitRegisty Example")
{
BoostUnitRegistry<boost::units::si::system> ureg;

Quantity<double> q;

// all SI base units and their prefixed versions
// are automatically defined.

q = ureg.makeQuantity<double>( 24, "cm" );
CHECK( q.to("m").value() == Approx(0.24) );

// mass
q = ureg.makeQuantity<double>( 24, "kg" );
CHECK( q.to("kg").value() == Approx(24) );
//CHECK( q.to("g").value() == Approx(24000) ); // 'g' does not exist yet
CHECK( q.to("mkg").value() == Approx(24000) );

// time
q = ureg.makeQuantity<double>( 24, "ms" );
CHECK( q.to("ms").value() == Approx(24) );
CHECK( q.to("s").value() == Approx(0.024) );
CHECK( q.to_base_units().value() == Approx(0.024) );

// electrical current
q = ureg.makeQuantity<double>( 24, "A" );
CHECK( q.to("mA").value() == Approx(24000) );

// temperature
q = ureg.makeQuantity<double>( 24, "K" );
CHECK( q.to("mK").value() == Approx(24000) );

// amount
q = ureg.makeQuantity<double>( 24, "mol" );
CHECK( q.to("mmol").value() == Approx(24000) );

// luminous intensity
q = ureg.makeQuantity<double>( 24, "cd" );
CHECK( q.to("mcd").value() == Approx(24000) );



// now add some non-base units
// we can add boost unit directly.
// however, the unit will be added using whatever it string-ify's to
ureg.addUnit( boost::units::cgs::mass() );
// or we can define the units with a string
ureg.addUnit( "N = kg m / s^2" );
ureg.addUnit( "J = N m" );
ureg.addUnit( "W = J / s" );

q = ureg.makeQuantity<double>( 24, "kg" );
CHECK( q.to("kg").value() == Approx(24) );
CHECK( q.to("g").value() == Approx(24000) ); // 'g' exists now


q = ureg.makeQuantity<double>( 24, "kJ" );
CHECK( q.to("mJ").value() == Approx(24000000) );
CHECK( q.to_base_units().value() == Approx(24000) );

q = ureg.makeQuantity<double>( 24, "mW" );
CHECK( q.to("kW").value() == Approx(0.000024) );


}
```

## Features

- Small and simple library that only depends on `boost` (or is that a limitation?)
- Can perform arbitrary linear unit conversions, including conversions between offset units.
- Dimensions of unit conversions are checked. An exception is thrown if the dimensions don't match.
- Units can be given as strings, so they can easily be specified at runtime.
- Unit string parsing uses Boost.Spirit which is fast, flexible, and robust. 
- Quantity class can build a Boost.Unit quantity, so it's easy to construct a Boost.Unit quantity from user input with support for arbitrary unit conversions.

## Limitation

- Calculations involving `Quantity` objects are not supported. This library is intended for unit conversion, not quantity calculations.
  `Boost.Units` is a great unit library that supports calculations with quantities. If you need to do quantity calculations with
  input from a user, use `UnitConvert` to convert the user's input to the unit you want to use internally and create a `Boost.Units` `quantity`. `UnitConvert` supports creating `Boost.Units` `quantity` instances from `Quantity` objects.
- No output formatting. The `UnitRegistry` can parse unit strings, but it does not format. If you print a unit to the screen, it will show the saling factor and dimension powers.
- Only units with a linear scale and offset are supported. It's not possible to represent wire gauge for example.

