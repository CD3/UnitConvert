#define UNITCONVERT_NO_BACKWARD_COMPATIBLE_NAMESPACE
#include <UnitConvert.hpp>
#include <UnitConvert/GlobalUnitRegistry.hpp>
#include <string>
#include <emscripten/bind.h>



double GetMagnitudeInUnit(std::string input, std::string new_unit)
{
  static UnitConvert::UnitRegistry& ureg = UnitConvert::getGlobalUnitRegistry();
  auto quant = ureg.makeQuantity<double>(input);
  double new_val = quant.to(new_unit).value();
  return new_val;
}

std::string UnitConvertString(std::string input, std::string new_unit)
{
  return std::to_string(GetMagnitudeInUnit(input,new_unit)) + " " + new_unit;
}



EMSCRIPTEN_BINDINGS(WasmUnitConver)
{
  emscripten::function("UnitConvertString",&UnitConvertString);
  emscripten::function("GetMagnitudeInUnit",&GetMagnitudeInUnit);
}

