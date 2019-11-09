#include "catch.hpp"

#include <UnitConvert.hpp>


TEST_CASE("UnitRegistry Unit Equation Parser", "[bugs]")
{
  UnitRegistry ureg;

  ureg.addBaseUnit<Dimension::Name::Length>("m");
  ureg.addBaseUnit<Dimension::Name::Mass>("kg");
  ureg.addBaseUnit<Dimension::Name::Time>("s");
  ureg.addBaseUnit<Dimension::Name::Temperature>("K");
  ureg.addBaseUnit<Dimension::Name::Amount>("mol");


  SECTION("Division by numerical values")
  {
    ureg.addUnit("C", ureg.getUnit("K")-273.15);
    ureg.addUnit("F", ureg.getUnit("C")*(5./9) + 32);

    auto q = ureg.makeQuantity<double>(37, "C");
    CHECK( q.value() == Approx(37) );
    // the Quantity class can convert to a unit string
    // if it was created by a UnitRegistry
    CHECK( q.to("F").value() == Approx(98.6) );
    CHECK_THROWS( q.to("m") );
  }



}


TEST_CASE("UnitRegistry.makeQuantity(std::string) does not parse derived units correctly", "[bugs]")
{
  UnitRegistry ureg;

  ureg.addBaseUnit<Dimension::Name::Length>("m");
  ureg.addBaseUnit<Dimension::Name::Mass>("g");
  ureg.addBaseUnit<Dimension::Name::Mass>("s");

  CHECK( ureg.makeQuantity<double>(10, "kg m").to("g m").value() == Approx(10000) );
  CHECK( ureg.makeQuantity<double>("10 kg m").to("g m").value() == Approx(10000) );
  CHECK( ureg.makeQuantity<double>("10 kg m / s").to("g m / ms").value() == Approx(10) );
}
