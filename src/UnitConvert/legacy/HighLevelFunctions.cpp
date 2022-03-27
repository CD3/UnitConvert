#include "./UnitRegistry.hpp"
#include "./Quantity.hpp"
#include "./GlobalUnitRegistry.hpp"
#include "./HighLevelFunctions.hpp"


namespace UnitConvert {

auto GetMagnitudeInUnit(std::string input, std::string new_unit) -> double
{
  static UnitConvert::UnitRegistry& ureg = UnitConvert::getGlobalUnitRegistry();
  auto quant = ureg.makeQuantity<double>(input);
  double new_val = quant.to(new_unit).value();
  return new_val;
}

auto UnitConvertString(std::string input, std::string new_unit) -> std::string
{
  return std::to_string(GetMagnitudeInUnit(input,new_unit)) + " " + new_unit;
}

auto HaveSameDimensions(std::string u1, std::string u2) -> bool
{
  static UnitConvert::UnitRegistry& ureg = UnitConvert::getGlobalUnitRegistry();
  auto q1 = ureg.makeQuantity<double>(u1);
  auto q2 = ureg.makeQuantity<double>(u2);
  return q1.unit().dimension() == q2.unit().dimension();
}

auto AddUnitDefinition(std::string def) -> bool
{
  static UnitConvert::UnitRegistry& ureg = UnitConvert::getGlobalUnitRegistry();
  ureg.addUnit(def);
  return true;
}

}
