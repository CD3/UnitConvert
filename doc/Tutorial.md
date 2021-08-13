# `UnitConvert` Tutorial

## Basics

`UnitConvert` provides **runtime** unit conversions, and is capable of parsing unit strings. It was inspired
by [`pint`](https://pint.readthedocs.io/en/0.9/), the wonderful unit conversion library for Python. Unlike `pint`,
`UnitConvert` does not provide any quantity *calculations*. You should use `Boost.Units` for that.

To get started, include the `UnitConvert.hpp` header.
```cpp
#include <UnitConvert.hpp>
```
This will include all of the pieces of the library that you need.

In order to parse unit strings, `UnitConvert` must know about unit symbols. This is done by creating
a unit registry. A unit registry associates a unit (dimension, scale factor, and offset) with a text string.
To perform unit conversions using unit strings, you must first create a unit registry.
```cpp
ureg = UnitRegistry();
```
Next, we need to add some base units to the registry. We do this by calling the `.addUnit` method with a string
that defines the unit in terms of its dimension(s), which are specified with square brackets.
```cpp
ureg.addUnit("m = [L]");
ureg.addUnit("g = [M]");
ureg.addUnit("s = [T]");
```
The registry now contains units for length, mass, and time. A symbol for each of the [7 SI base dimensions](https://en.wikipedia.org/wiki/SI_base_unit), plus a symbol for dimensionless, is defined in the unit registry:
  - `L` : Length
  - `M` : Mass
  - `T` : Time
  - `N` : Amount
  - `THETA` : Temperature
  - `I` : Electrical Current
  - `J` : Luminous Intensity
  - `1` : Dimensionless

We can now perform conversions between arbitrary combinations of
these base units. We need to create a `Quantity` (a value with a unit). The `UnitRegistry` class provides a `makeQuantity` method
for this. It takes a template parameter for the type used to store the units numerical value.
```cpp
Quantity<double> L = ureg.makeQuantity<double>(200, "m");
```
Here we have created a quantity for length, it is 200 meters. The `UnitRegistry`
class knows about [all 20 SI prefixes](https://physics.nist.gov/cuu/Units/prefixes.html), so we can perform
unit conversions between different unit prefixes. Unit conversions are performed with the `.to` method.
```cpp
Quantity<double> l = L.to("cm");
```
Quantities have a value, and a unit. The value can be retrieved with the `.value` method.
```
std::cout << L.value() << "\n"; // prints 200
std::cout << l.value() << "\n"; // prints 20000
```
Converting units for one dimension between the different SI prefixes isn't very interesting. However, the `UnitRegistry` knows
how to parse unit strings (strings defining combinations of units, i.e. derived units), so we can do more complicated conversions too.
```cpp
Quantity<double> F = ureg.makeQuantity<double>(10, "kg m / s^2"); // 10 Newton
Quantity<double> f = F.to("g cm / ms^2");
std::cout << F.value() << "\n"; // prints 10
std::cout << f.value() << "\n"; // prints 0.1
```
The `UnitRegistry` can parse arbitrary (reasonable) unit strings, and perform unit conversions between them. If we make a mistake
and try to convert to a unit with different dimensions, we will get an exception.
```cpp
std::cout << F.to("kg cm / s") << "\n"; // throws an exception, dimensions don't match.
```
To perform unit conversions with named derived units (i.e. Newton) or units from other systems (i.e. inch), we need to add them
to the `UnitRegistry`. This is done by calling the `.addUnit` method. The
`UnitRegistry` can parse unit *equations* as well as unit strings, so it is
possible to define new units in a natural way by just writing their definition.
```cpp
ureg.addUnit("in = 2.54 cm");
ureg.addUnit("ft  = 12 in");
ureg.addUnit("yd  = 3 ft");
ureg.addUnit("mile  = 5280 ft");
ureg.addUnit("N = kg m / s^2");
ureg.addUnit("lbf = 4.4482 N");
ureg.addUnit("J = N m");
ureg.addUnit("W = J / s");
```
As soon as a unit is added to the registry, it can be used to define new units. Converting units that contain named derived units
is the same.
```
Quantity<double> E = ureg.makeQuantity<double>(10, "J");
std::cout << E.to("mJ").value() << "\n"; // prints 10000
std::cout << E.to("g m cm / s^2").value() << "\n"; // prints 1e6
std::cout << E.to("g in^2 / s^2").value() << "\n"; // prints 1.55e+7
```
We can also ask for a unit expressed in the base units of the `UnitRegistry`.
```
std::cout << ureg.makeQuantity<double>(10, "lbf").to_base_units().value << "\n"; // prints 44.48
```
## Boost.Units Integration
[`Boost.Units`](https://www.boost.org/doc/libs/1_71_0/doc/html/boost_units.html) is a boost library for doing unit conversions and dimensional
analysis. All conversions and dimension checking are done at *compile time*, so there is very little (the authors claim zero) runtime overhead.
With `Boost.Units`, you can ensure that calculations involving a dimension error will result in a compiler error. It is a great library, and
I recommend using it.

However, since `Boost.Units` does not perform runtime unit conversions, it is not possible to read a string from a user (from a configuration file
for example), and unit convert it to some internal representation used by your code. This was the primary motivation for `UnitConvert`, which
is why `UnitConvert` does not provide any dimensional analysis for calculations involving quantities, `Boost.Units` already does this.
The role of `UnitCovert` is to provide support for runtime unit conversions and unit string parsing so that an application can read a quantity
from a configuration file, unit convert it to the unit used internally by the
application, and then use `Boost.Units` for the internal calculations.

The `Quantity` class provides an overload of the `.to` method that takes a template argument. You pass a `Boost.Units` unit type to the method,
and it will return a `Boost.Units` `quantity`.
```
// Boost.Units headers
#include<boost/units/systems/si.hpp>
#include<boost/units/quantity.hpp>

...
Quantity<double> DistanceFromUser = ureg.makeQuantity<double>("2 mile");

boost::units::quantity<boost::units::si::length> L = DistanceFromUser.to<boost::units::si::length>();

std::cout << L << "\n"; // prints '3218.69 m'
```
This makes it very easy to parse a unit string into a `Boost.Units` `quantity`, so the application can allow users to specify configuration
quantities in whatever unit is convenient.





