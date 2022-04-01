#pragma once
#include <string>

/**
 * @file HighLevelFunctions.hpp
 *
 * A collection of functions for working with quantities as strings.
 */

namespace UnitConvert {

auto GetMagnitudeInUnit(std::string input, std::string new_unit) -> double;
auto UnitConvertString(std::string input, std::string new_unit) -> std::string;
auto HaveSameDimensions(std::string u1, std::string u2) -> bool;
auto AddUnitDefinition(std::string def) -> bool;

}
