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
\file{../testing/CatchTests/Usage.cpp}
```
## Features

- Small and simple library that only depends on `boost` (or is that a limitation?)
- Can perform arbitrary linear unit conversions, including conversions between offset units.
- Dimensions of unit conversions are checked. An exception is thrown if the dimensions don't match.
- Units can be given as strings, so they can easily be specified at runtime.

## Limitation

- Calculations involving `Quantity` objects are not supported. This library is intended for unit conversion, not quantity calculations.
  `Boost.Units` is a great unit library that supports calculations with quantities. If you need to do quantity calculations with
  input from a user, use `UnitConvert` to convert the user's input to the unit you want to use internally and create a `Boost.Units` `quantity`. `UnitConvert` supports creating `Boost.Units` `quantity` instances from `Quantity` objects.
- Parsing of unit strings is rudimentary, and has not been optimized. It currently uses `Boost.Tokenize`.
- Only units with a linear scale and offset are supported. It's not possible to represent wire gauge for example.
