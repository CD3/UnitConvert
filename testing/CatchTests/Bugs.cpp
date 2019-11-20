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


TEST_CASE("Fahrenheit conversions incorrect", "[bugs]")
{
    UnitRegistry ureg;

    // this seems is probably an issue with offset units
    // in general, but if temperature is the most common case
    ureg.addUnit("K = [THETA]");

    ureg.addUnit("C = K - 273.15");
    ureg.addUnit("delta_C = K");

    ureg.addUnit("900 R = 500 K");

    ureg.addUnit("F = R - 459.67");
    ureg.addUnit("delta_F = R");

    auto K = ureg.getUnit("K");
    auto C = ureg.getUnit("C");
    auto R = ureg.getUnit("R");
    auto F = ureg.getUnit("F");

    std::cout << "K: " << K << std::endl;
    std::cout << "C: " << C << std::endl;
    std::cout << "R: " << R << std::endl;
    std::cout << "F: " << F << std::endl;

    // going from absolute to absolute and absolute to offset units
    CHECK(ureg.makeQuantity<double>("0 K").to("C").value() == Approx(-273.15));
    CHECK(ureg.makeQuantity<double>("0 K").to("R").value() == Approx(0));
    CHECK(ureg.makeQuantity<double>("0 K").to("F").value() == Approx(-459.67));

    CHECK(ureg.makeQuantity<double>("0 R").to("C").value() == Approx(-273.15));
    CHECK(ureg.makeQuantity<double>("0 R").to("K").value() == Approx(0));
    CHECK(ureg.makeQuantity<double>("0 R").to("F").value() == Approx(-459.67));

    CHECK(ureg.makeQuantity<double>("100 K").to("C").value() == Approx(-173.15));
    CHECK(ureg.makeQuantity<double>("100 K").to("R").value() == Approx(180));
    CHECK(ureg.makeQuantity<double>("100 K").to("F").value() == Approx(180 - 459.67));

    CHECK(ureg.makeQuantity<double>("100 R").to("C").value() == Approx(55.555 - 273.15));
    CHECK(ureg.makeQuantity<double>("100 R").to("K").value() == Approx(55.555));
    CHECK(ureg.makeQuantity<double>("100 R").to("F").value() == Approx(100 - 459.67));

    CHECK(ureg.makeQuantity<double>("491.67 R").to("C").value() + 1 == Approx(1));
    CHECK(ureg.makeQuantity<double>("491.67 R").to("K").value() == Approx(273.15));
    CHECK(ureg.makeQuantity<double>("491.67 R").to("F").value() == Approx(32));



    // going from offset units to absolute within the same scale
    CHECK(ureg.makeQuantity<double>("0 C").to("K").value() == Approx(273.15));
    CHECK(ureg.makeQuantity<double>("0 F").to("R").value() == Approx(459.67));
    CHECK(ureg.makeQuantity<double>("100 C").to("K").value() == Approx(373.15));
    CHECK(ureg.makeQuantity<double>("100 F").to("R").value() == Approx(559.67));

    // going from offset to absolute in different scale
    CHECK(ureg.makeQuantity<double>("0 C").to("R").value() == Approx(491.67));

}
