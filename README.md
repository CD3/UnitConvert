# UnitConvert

A small C++ library, inspired by [`pint`](https://pint.readthedocs.io/en/latest/), for doing runtime unit conversions interoperates with the `Boost.Units` library.

## Description

`UnitConvert` provides **runtime** unit conversions. This is necessary when, for example, you want to perform a unit conversion based on user input. The library
provides a `UnitRegistry` class (similar to [`pint`](https://pint.readthedocs.io/en/latest/)) that can parse unit strings 

## Getting Started

## Features

- Small and simple library that only depends on `boost` (or is that a limitation?)
- Can perform arbitrary unit conversion, including conversions between offset units.
- Dimensions of unit conversions are checked. An exception is thrown if the dimensions don't match.

## Limitation

- Calculations involving `Quantity` objects are not supported. This library is intended for unit conversion, not quantity calculations.
  `Boost.Units` is a great unit library that supports calculations with quantities. If you need to do quantity calculations with
  input from a user, use `UnitConvert` to convert the user's input to the unit you want to use internally and create a `Boost.Units` quantity.
- String parsing is rudimentary, and has not been optimized. It currently used `Boost.Tokenize`.

