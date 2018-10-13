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

    //auto q = ureg.Quantity<double>(37, "C");
    //CHECK( q.value() == Approx(37) );
    //// the Quantity class can convert to a unit string
    //// if it was created by a UnitRegistry
    //CHECK( q.to("F").value() == Approx(98.6) );
    //CHECK_THROWS( q.to("m") );
  }



}


