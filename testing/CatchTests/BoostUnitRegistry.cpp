#include <catch.hpp>

#include <boost/units/systems/cgs.hpp>
#include <boost/units/systems/si.hpp>

#include <UnitConvert.hpp>

TEST_CASE("BoostUnitRegisty")
{
  using namespace UnitConvert;

  BoostUnitRegistry<boost::units::si::system> ureg;
  

  SECTION("Conversions")
  {

  Quantity<double> q;

  // length
  q = ureg.makeQuantity<double>(24, "cm");
  CHECK(q.to("m").value() == Approx(0.24));
  CHECK(q.to("cm").value() == Approx(24));
  CHECK(q.to("mm").value() == Approx(240));
  CHECK(q.to_base_units().value() == Approx(0.24));

  // mass
  q = ureg.makeQuantity<double>(24, "kg");
  CHECK(q.to("kg").value() == Approx(24));
  // CHECK( q.to("g").value() == Approx(24000) ); // 'g' does not exist yet
  CHECK(q.to("mkg").value() == Approx(24000));
  CHECK(q.to_base_units().value() == Approx(24));

  // time
  q = ureg.makeQuantity<double>(24, "ms");
  CHECK(q.to("ms").value() == Approx(24));
  CHECK(q.to("cs").value() == Approx(2.4));
  CHECK(q.to("s").value() == Approx(0.024));
  CHECK(q.to("ks").value() == Approx(0.000024));
  CHECK(q.to_base_units().value() == Approx(0.024));

  // electrical current
  q = ureg.makeQuantity<double>(24, "A");
  CHECK(q.to("mA").value() == Approx(24000));
  CHECK(q.to("cA").value() == Approx(2400));
  CHECK(q.to("A").value() == Approx(24));
  CHECK(q.to("kA").value() == Approx(0.024));
  CHECK(q.to_base_units().value() == Approx(24));

  // temperature
  q = ureg.makeQuantity<double>(24, "K");
  CHECK(q.to("mK").value() == Approx(24000));
  CHECK(q.to("cK").value() == Approx(2400));
  CHECK(q.to("K").value() == Approx(24));
  CHECK(q.to("kK").value() == Approx(0.024));
  CHECK(q.to_base_units().value() == Approx(24));

  // amount
  q = ureg.makeQuantity<double>(24, "mol");
  CHECK(q.to("mmol").value() == Approx(24000));
  CHECK(q.to("cmol").value() == Approx(2400));
  CHECK(q.to("mol").value() == Approx(24));
  CHECK(q.to("kmol").value() == Approx(0.024));
  CHECK(q.to_base_units().value() == Approx(24));

  // luminous intensity
  q = ureg.makeQuantity<double>(24, "cd");
  CHECK(q.to("mcd").value() == Approx(24000));
  CHECK(q.to("ccd").value() == Approx(2400));
  CHECK(q.to("cd").value() == Approx(24));
  CHECK(q.to("kcd").value() == Approx(0.024));
  CHECK(q.to_base_units().value() == Approx(24));


  // now add some non-base units
  ureg.addUnit(boost::units::cgs::mass());
  ureg.addUnit("N = kg m / s^2");
  ureg.addUnit("J = N m");
  ureg.addUnit("W = J s");

  q = ureg.makeQuantity<double>(24, "kg");
  CHECK(q.to("kg").value() == Approx(24));
  CHECK(q.to("g").value() == Approx(24000));  // 'g' exists now

  q = ureg.makeQuantity<double>(24, "kJ");
  CHECK(q.to("kJ").value() == Approx(24));
  CHECK(q.to("J").value() == Approx(24000));
  CHECK(q.to("mJ").value() == Approx(24000000));
  CHECK(q.to("g m^2 / s^2").value() == Approx(24000000));
  CHECK(q.to("g m^2 / ms^2").value() == Approx(24));
  CHECK(q.to_base_units().value() == Approx(24000));

  q = ureg.makeQuantity<double>(24, "mW");
  CHECK(q.to("kW").value() == Approx(0.000024));
  CHECK(q.to("W").value() == Approx(0.024));
  CHECK(q.to("mW").value() == Approx(24));
  CHECK(q.to_base_units().value() == Approx(0.024));
  }

  SECTION("Adding Boost.Units unit")
  {
    CHECK_THROWS( ureg.makeQuantity<double>("1 g") );
    ureg.addUnit(boost::units::cgs::mass());
    ureg.makeQuantity<double>("1 g");
  }

  SECTION("Converting to Boost.Units quantity")
  {
    auto L = ureg.makeQuantity<double>("100 cm").to<boost::units::si::length>();
    auto T = ureg.makeQuantity<double>("2 ms").to<boost::units::si::time>();

    auto V = L/T;

    CHECK( V.value() == Approx( (100./2)*(1./100)*(1000./1)   ) );
  }
  
}
