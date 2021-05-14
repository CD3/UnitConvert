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

bool HaveSameDimensions(std::string u1, std::string u2)
{
  static UnitConvert::UnitRegistry& ureg = UnitConvert::getGlobalUnitRegistry();
  auto q1 = ureg.makeQuantity<double>(u1);
  auto q2 = ureg.makeQuantity<double>(u2);
  return q1.unit().dimension() == q2.unit().dimension();
}



EMSCRIPTEN_BINDINGS(WasmUnitConver)
{
  emscripten::function("UnitConvertString",&UnitConvertString);
  emscripten::function("GetMagnitudeInUnit",&GetMagnitudeInUnit);
  emscripten::function("HaveSameDimensions",&HaveSameDimensions);
}

