#include "catch.hpp"

#include <UnitConvert.hpp>
#include <UnitConvert/GlobalUnitRegistry.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/units/systems/cgs.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/units/io.hpp>

namespace std
{
template <typename U, typename T>
istream& operator>>(istream& in, boost::units::quantity<U,T>& val)
{
  char unit_char_array[256];
  in.getline(unit_char_array,256);
  std::string unit_string(unit_char_array);

  val = UnitConvert::getGlobalUnitRegistry().makeQuantity<T>(unit_string).template to<boost::units::si::length>();

  return in;
}
}  // namespace std

TEST_CASE("Boost.PropertyTree Integration")
{
  using namespace boost::units;
  boost::property_tree::ptree tree;

  quantity<si::length> L(10 * si::meter);

  tree.put("Length",L);

  boost::property_tree::write_json(std::cout, tree);

  quantity<si::length,double> L2 = tree.get<quantity<si::length,double>>("Length");

  std::cout << L2;

  CHECK(L2.value() == Approx(10));
}
