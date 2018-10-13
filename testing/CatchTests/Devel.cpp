#include "catch.hpp"

#include <boost/units/systems/si.hpp>

/**
 * This file is used for developement. As new classes are created, small tests
 * are written here so that we can try to compile and use them.
 */

#include <UnitConvert.hpp>

TEST_CASE("Dimension Devel", "[devel]")
{
  BaseDimension<Dimension::Name::Length>            L;
  BaseDimension<Dimension::Name::Mass>              M;
  BaseDimension<Dimension::Name::Time>              T;
  BaseDimension<Dimension::Name::ElectricalCurrent> I;
  BaseDimension<Dimension::Name::Temperature>       THETA;
  BaseDimension<Dimension::Name::Amount>            N;
  BaseDimension<Dimension::Name::LuminousIntensity> J;

  Dimension V = L / T;
  Dimension E = M * V * V;
  Dimension P = E / T;

  BaseDimension<Dimension::Name::Length> Length;

  CHECK(P[Dimension::Name::Length] == 2);
  CHECK(P[Dimension::Name::Mass] == 1);
  CHECK(P[Dimension::Name::Time] == -3);
  CHECK(P[Dimension::Name::ElectricalCurrent] == 0);
  CHECK(P[Dimension::Name::Temperature] == 0);
  CHECK(P[Dimension::Name::Amount] == 0);
  CHECK(P[Dimension::Name::LuminousIntensity] == 0);

  CHECK(L == Length);
  CHECK(M != Length);
  CHECK(M != L);
  CHECK(E != P);
  CHECK(E == P * T);
}

TEST_CASE("Unit Devel", "[devel]")
{
  BaseUnit<Dimension::Name::Length>      meter;
  BaseUnit<Dimension::Name::Mass>        kilogram;
  BaseUnit<Dimension::Name::Time>        second;
  BaseUnit<Dimension::Name::Temperature> kelvin;

  Unit gram = kilogram / 1000;

  Unit centimeter = meter / 100;
  Unit inch       = 2.54 * centimeter;
  Unit foot       = 12 * inch;

  Unit joule = kilogram * meter * meter / second / second;

  Unit celsius    = kelvin - 273.15;
  Unit fahrenheit = (5. / 9) * celsius + 32;

  CHECK(meter.scale() == 1);
  CHECK(meter.offset() == 0);
  CHECK(kilogram.scale() == 1);
  CHECK(kilogram.offset() == 0);
  CHECK(second.scale() == 1);
  CHECK(second.offset() == 0);
  CHECK(kelvin.scale() == 1);
  CHECK(kelvin.offset() == 0);

  CHECK(gram.scale() == Approx(0.001));
  CHECK(gram.offset() == 0);

  CHECK(centimeter.scale() == Approx(0.01));
  CHECK(centimeter.offset() == 0);
  CHECK(inch.scale() == Approx(0.0254));
  CHECK(inch.offset() == 0);
  CHECK(foot.scale() == Approx(12 * 0.0254));
  CHECK(foot.offset() == 0);

  CHECK(joule.scale() == Approx(1));
  CHECK(joule.offset() == Approx(0));

  CHECK(celsius.scale() == Approx(1));
  CHECK(celsius.offset() == Approx(-273.15));

  CHECK(fahrenheit.scale() == Approx(5. / 9));
  CHECK(fahrenheit.offset() == Approx(32 - (9. / 5) * 273.15));

  SECTION("Quantity Devel", "[devel]")
  {
    Quantity<double> distance(100, foot);

    CHECK(distance.to(meter).value() == Approx(30.48));
    CHECK_THROWS(distance.to(second));

    Quantity<double> temperature(37, celsius);

    CHECK(temperature.to(kelvin).value() == Approx(273.15 + 37));
    CHECK(temperature.to(fahrenheit).value() == Approx(98.6));

  }
}

TEST_CASE("UnitRegistry Devel", "[devel]")
{
  UnitRegistry ureg;

  ureg.addBaseUnit<Dimension::Name::Length>("m");
  ureg.addBaseUnit<Dimension::Name::Mass>("kg");
  ureg.addBaseUnit<Dimension::Name::Time>("s");
  ureg.addBaseUnit<Dimension::Name::Temperature>("K");
  ureg.addBaseUnit<Dimension::Name::Amount>("mol");

  ureg.addUnit("100 cm = 1 m");
  ureg.addUnit("1 in = 2.54 cm");
  ureg.addUnit("1 ft = 12 in");
  ureg.addUnit("1 J = 1 kg*m^2*s^-2");
  ureg.addUnit("1 W = 1 J/s");
  ureg.addUnit("1 cal = 4.184 J");

  Quantity<double> q = ureg.Quantity<double>(200, "cm");

  CHECK(q.value() == Approx(200));
  // getUnit returns a unit that is in the registry
  CHECK(q.to(ureg.getUnit("m")).value() == Approx(2));
  CHECK_THROWS(q.to(ureg.getUnit("s")));

  q = ureg.Quantity<double>(0.25, "s");
  CHECK(q.value() == Approx(0.25));
  // makeUnit can create a unit from the registry,
  // including SI prefixed versions of the units in the registry
  CHECK(q.to(ureg.makeUnit("ms")).value() == Approx(250));
  CHECK_THROWS(q.to(ureg.getUnit("m")));

  q = ureg.Quantity<double>(100, "cal");
  CHECK(q.value() == Approx(100));
  // the Quantity class can convert to a unit string
  // if it was created by a UnitRegistry
  CHECK(q.to("J").value() == Approx(100 * 4.184));

  // string based conversion can convert to SI prefixed version of any unit
  // in the registry, even if they are not explicitly put in the registry,
  CHECK(q.to("mJ").value() == Approx(100 * 4.184 * 1000));

  // string based conversion can also convert to derived unit representations
  // that contain SI prefixed versions of units in the registry
  CHECK(q.to("kg*km^2/ms^2").value() == Approx(100 * 4.184 / 1e12));
  CHECK(q.to("kg*m*km/ms/s").value() == Approx(100 * 4.184 / 1e6));
  CHECK(q.to("mW*s").value() == Approx(100 * 4.184 * 1000));
  CHECK(q.to("mW*cs").value() == Approx(100 * 4.184 * 1000 * 100));

  CHECK_THROWS(q.to("m"));

  SECTION("Converting to Boost.Units quantity")
  {
    Quantity<double> distance = ureg.Quantity<double>(100, "ft");
    boost::units::quantity<boost::units::si::length> L =
        distance.to<boost::units::si::length>();
    CHECK(boost::units::quantity_cast<double>(L) == Approx(30.48));
  }
}

TEST_CASE("BoostUnitRegistry Devel", "[devel]")
{
  BoostUnitRegistry<boost::units::si::system> ureg;

  // base units are automatically loaded into the
  // registry.

  ureg.addUnit("100 cm = 1 m");
  ureg.addUnit("1 in = 2.54 cm");
  ureg.addUnit("1 ft = 12 in");
  ureg.addUnit("1 J = 1 kg*m^2*s^-2");
  ureg.addUnit("1 W = 1 J/s");
  ureg.addUnit("1 cal = 4.184 J");

  Quantity<double> q = ureg.Quantity<double>(200, "cm");

  CHECK(q.value() == Approx(200));
  // getUnit returns a unit that is in the registry
  CHECK(q.to(ureg.getUnit("m")).value() == Approx(2));
  CHECK_THROWS(q.to(ureg.getUnit("s")));

  q = ureg.Quantity<double>(0.25, "s");
  CHECK(q.value() == Approx(0.25));
  // makeUnit can create a unit from the registry,
  // including SI prefixed versions of the units in the registry
  CHECK(q.to(ureg.makeUnit("ms")).value() == Approx(250));
  CHECK_THROWS(q.to(ureg.getUnit("m")));

  q = ureg.Quantity<double>(100, "cal");
  CHECK(q.value() == Approx(100));
  // the Quantity class can convert to a unit string
  // if it was created by a UnitRegistry
  CHECK(q.to("J").value() == Approx(100 * 4.184));

  // string based conversion can convert to SI prefixed version of any unit
  // in the registry, even if they are not explicitly put in the registry,
  CHECK(q.to("mJ").value() == Approx(100 * 4.184 * 1000));

  // string based conversion can also convert to derived unit representations
  // that contain SI prefixed versions of units in the registry
  CHECK(q.to("kg*km^2/ms^2").value() == Approx(100 * 4.184 / 1e12));
  CHECK(q.to("kg*m*km/ms/s").value() == Approx(100 * 4.184 / 1e6));
  CHECK(q.to("mW*s").value() == Approx(100 * 4.184 * 1000));
  CHECK(q.to("mW*cs").value() == Approx(100 * 4.184 * 1000 * 100));

  CHECK_THROWS(q.to("m"));

  SECTION("Converting to Boost.Units quantity")
  {
    Quantity<double> distance = ureg.Quantity<double>(100, "ft");
    boost::units::quantity<boost::units::si::length> L =
        distance.to<boost::units::si::length>();
    CHECK(boost::units::quantity_cast<double>(L) == Approx(30.48));
  }

}
