# UnitConvert

A small C++ library, inspired by [`pint`](https://pint.readthedocs.io/en/latest/), for doing runtime unit conversions that interoperates with the `Boost.Units` library.

## Description

`UnitConvert` provides **runtime** unit conversions. This is necessary when, for example, you want to perform a unit conversion based on user input. The library
provides a `UnitRegistry` class (similar to [`pint`](https://pint.readthedocs.io/en/latest/)) that can parse unit strings.

## Getting Started

To use `UnitConvert`, include the header `UnitConvert.hpp`. Then create a unit registry, and add
some units to it. You can then use the registry to create `Quantity` instances, which can be
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
ureg.addBaseUnit<Dimension::Name::Length>("m");
ureg.addBaseUnit<Dimension::Name::Mass>("kg");
ureg.addBaseUnit<Dimension::Name::Time>("s");
ureg.addBaseUnit<Dimension::Name::Temperature>("K");
ureg.addBaseUnit<Dimension::Name::Amount>("mol");

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


There is also a `BoostUnitRegistry`, which adds more boost.units support.
When you create the unit registry, pass it a boost.units system, and it will automatically
populate the registry with the base units of the system. All prefixed versions are also added.
You can add new units as before, or by passing in an instance of a boost unit. If you use a boost
unit, the string representation of the unit will be added to the registry.

```
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
ureg.addUnit( "W = J s" );

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
- Only units with a linear scale and offset are supported. It's not possible to represent wire gauge for example.
