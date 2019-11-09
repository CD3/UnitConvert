#include <catch.hpp>
#include <UnitConvert.hpp>
#include <sstream>

#include <boost/units/systems/si.hpp>

TEST_CASE("UnitRegisty base units")
{
UnitRegistry ureg;

// add base units to the registry
ureg.addBaseUnit<Dimension::Name::Length>("cm");
ureg.addBaseUnit<Dimension::Name::Mass>("g");
ureg.addBaseUnit<Dimension::Name::Time>("s");
ureg.addBaseUnit<Dimension::Name::Temperature>("K");
ureg.addBaseUnit<Dimension::Name::Amount>("mol");

SECTION("Adding unit directly")
{


// add some derived units
ureg.addUnit("m = 100 cm");
ureg.addUnit("1 in = 2.54 cm");
ureg.addUnit("1 ft = 12 in");
ureg.addUnit("1 J = 1 kg*m^2*s^-2");
ureg.addUnit("1 W = 1 J/s");
ureg.addUnit("1 cal = 4.184 J");
ureg.addUnit("1 degC = K - 273.15");


Quantity<double> q;

q = ureg.makeQuantity<double>( 24, "cm" );
CHECK( q.to("m").value() == Approx(0.24) );
CHECK( q.to("cm").value() == Approx(24) );
CHECK( q.to("mcm").value() == Approx(24000) );
CHECK( q.to("mm").value() == Approx(240) );
CHECK( q.to_base_units().value() == Approx(24) );

q = ureg.makeQuantity<double>( 10, "in" );
CHECK( q.to_base_units().value() == Approx(25.4) );

q = ureg.makeQuantity<double>(1,"J");
// base units are g and cm so...
// kg m^-2 s^-2   g/kg cm/m cm/m = 1000 100 100 g cm^-2 s^-2 = 10,000,000 g cm^-2 s^-2
CHECK( q.to_base_units().value() == Approx(1e7) );

q = ureg.makeQuantity<double>(2,"cal");
CHECK( q.to_base_units().value() == Approx(2*4.184*1e7) );

q = ureg.makeQuantity<double>("100 degC");
CHECK( q.to("K").value() == Approx(373.15) );
CHECK( q.to_base_units().value() == Approx(373.15) );

CHECK( q.unit().is_offset() );
CHECK( q.unit().offset() == Approx(-273.15) );
CHECK(!q.to("K").unit().is_offset() );
CHECK( q.to("K").unit().offset() == Approx(0) );
// to_base_units will return a quantity with an offset unit here,
// but the offset will be zero.
CHECK( q.to_base_units().unit().is_offset() );
CHECK( q.to_base_units().unit().offset() == Approx(0) );
}

SECTION("Loading units")
{
  std::string units = "m = 100 cm\nN = kg m / s^2\nJ = N m\nW = J/s";
  std::stringstream in(units);
  ureg.loadUnits(in);

  CHECK( ureg.makeQuantity<double>(10, "g m^2 / s^3").to("W").value() == 0.01);
}

}

