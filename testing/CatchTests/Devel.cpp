#include "catch.hpp"

#include <boost/units/systems/si.hpp>

#include <UnitConvert.hpp>

TEST_CASE("Spirit Testing")
{
  SECTION("Basic")
  {
  namespace qi    = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;

  std::string text = "1.3";
  double      val  = 0;

  auto it = text.begin();
  bool r  = qi::parse(it, text.end(), qi::double_[qi::_val = qi::_1 + 10], val);

  CHECK(val == Approx(11.3));
  }


  SECTION("Unit Parsing")
  {
  namespace qi    = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;
  using qi::_1;
  using qi::_val;

  std::string       text = "1.3 m / s / ( cm / s )";
  double            val  = 0;
  std::string       unit;
  std::vector<char> buffer;

  auto it = text.begin();
  bool r =
      qi::phrase_parse(it, text.end(), qi::double_ >> qi::lexeme[*qi::char_],
                       ascii::space, val, unit);

  std::cout << "val: " << val << std::endl;
  std::cout << "unit: " << unit << std::endl;

  CHECK(r);
  CHECK(it == text.end());
  CHECK(val == Approx(1.3));
  CHECK(unit == "m / s / ( cm / s )");
  }

}

TEST_CASE("Unit String Parsing")
{
    UnitRegistry ureg;

    ureg.addBaseUnit<Dimension::Name::Length>("m");
    ureg.addBaseUnit<Dimension::Name::Mass>("kg");
    ureg.addBaseUnit<Dimension::Name::Time>("s");
    ureg.addBaseUnit<Dimension::Name::Temperature>("K");
    ureg.addBaseUnit<Dimension::Name::Amount>("mol");
    ureg.addBaseUnit<Dimension::Name::Dimensionless>("");

    ureg.addUnit("100 cm = 1 m");
    ureg.addUnit("1 in = 2.54 cm");
    ureg.addUnit("1 ft = 12 in");
    ureg.addUnit("1 J = 1 kg*m^2*s^-2");
    ureg.addUnit("1 W = 1 J/s");
    ureg.addUnit("1 cal = 4.184 J");
    ureg.addUnit("1 N = 1 kg m / s^2");

    SECTION("Expression")
    {
    Unit u = ureg.getUnit("");

    BaseDimension<Dimension::Name::Length>            L;
    BaseDimension<Dimension::Name::Mass>              M;
    BaseDimension<Dimension::Name::Time>              T;
    BaseDimension<Dimension::Name::ElectricalCurrent> I;
    BaseDimension<Dimension::Name::Temperature>       THETA;
    BaseDimension<Dimension::Name::Amount>            N;
    BaseDimension<Dimension::Name::LuminousIntensity> J;
    BaseDimension<Dimension::Name::Dimensionless>     D;


    std::string unit = "m";
    auto it = unit.begin();
    auto r = qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK( u.dimension() == L );
    CHECK(u.scale() == 1);


    unit = "m*s";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK( u.dimension() == L*T );
    CHECK(u.scale() == 1);

    unit = "m/s";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK( u.dimension() == L/T );
    CHECK(u.scale() == 1);

    unit = "m*m/s";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK( u.dimension() == L*L/T );
    CHECK(u.scale() == 1);

    unit = "m/s*m";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK( u.dimension() == L*L/T );
    CHECK(u.scale() == 1);

    unit = "s^3";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK( u.dimension() == T*T*T );
    CHECK(u.scale() == 1);

    unit = "s^-3";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK( u.dimension() == D/T/T/T );
    CHECK(u.scale() == 1);

    unit = "kg * m / s^2";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M*L/T/T);
    CHECK(u.scale() == 1);

    unit = "kg   m";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M*L);
    CHECK(u.scale() == 1);

    unit = "kg   m / s^2";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M*L/T/T);
    CHECK(u.scale() == 1);

    unit = "kg / (m / s^2)";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M*T*T/L);
    CHECK(u.scale() == 1);

    unit = "kg * (m / s)^2";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M*L*L/T/T);
    CHECK(u.scale() == 1);

    unit = "kg   (m / s)^2";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M*L*L/T/T);
    CHECK(u.scale() == 1);

    unit = "kg   (m / s)^-2";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M*T*T/L/L);
    CHECK(u.scale() == 1);

    unit = "kg * m * s^-2";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M*L/T/T);
    CHECK(u.scale() == 1);

    unit = "3 * kg";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M);
    CHECK(u.scale() == 3);

    unit = "3 * kg";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M);
    CHECK(u.scale() == 3);

    unit = "-3 * kg";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M);
    CHECK(u.scale() == -3);

    unit = "3 * kg / 2 / m";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M/L);
    CHECK(u.scale() == Approx(3/2.));

    unit = "3 * kg / -2 / m";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M/L);
    CHECK(u.scale() == Approx(-3/2.));

    unit = "3 * kg * 2 * m * s^-2";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M*L/T/T);
    CHECK(u.scale() == 6);

    unit = "100 * m - 20";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == L);
    CHECK(u.scale() == 100);
    CHECK(u.offset() == -20);

    unit = "100 * m + 30";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == L);
    CHECK(u.scale() == 100);
    CHECK(u.offset() == 30);

    unit = "100 * m + -30";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == L);
    CHECK(u.scale() == 100);
    CHECK(u.offset() == -30);

    unit = "(3/10) m + 30";
    it   = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == L);
    CHECK(u.scale() == Approx(0.3));
    CHECK(u.offset() == 30);

    }

    SECTION("SI Prefixes")
    {
      UnitRegistry::SIPrefixParser parser;

      std::string unit;
      auto it = unit.begin();
      int i;
      bool r;

      unit = "Ym";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == 24);

      unit = "Zm";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == 21);



      unit = "Em";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == 18);

      unit = "Pm";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == 15);

      unit = "Tm";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == 12);



      unit = "Gm";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == 9);

      unit = "Mm";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == 6);




      unit = "km";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == 3);

      unit = "Mm";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == 6);

      unit = "km";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == 3);



      unit = "dm";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == -1);

      unit = "cm";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == -2);

      unit = "mm";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == -3);




      unit = "um";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == -6);

      unit = "nm";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == -9);

      unit = "pm";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == -12);




      unit = "fm";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == -15);

      unit = "am";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == -18);

      unit = "zm";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == -21);


      unit = "ym";
      it = unit.begin();
      r = qi::parse(it, unit.end(), parser, i);
      CHECK(r);
      CHECK(unit.end() - it == 1);
      CHECK(i == -24);

    }
}

/**
 * This file is used for development. As new classes are created,
 * small tests are written here so that we can try to compile and use
 * them.
 */

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

  Unit unit = celsius;

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

  CHECK(unit.offset() == Approx(-273.15));
  unit += 100;
  CHECK(unit.offset() == Approx(-173.15));
  unit += 100;
  CHECK(unit.offset() == Approx(-73.15));
  unit -= 50;
  CHECK(unit.offset() == Approx(-123.15));

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

  Unit u = BaseUnit<Dimension::Name::Dimensionless>();
  Quantity<double> q = ureg.makeQuantity<double>(200, "cm");

  u = ureg.getUnit("m");
  CHECK(u.scale() == Approx(1));
  u = ureg.getUnit("cm");
  CHECK(u.scale() == Approx(0.01));
  CHECK_THROWS(ureg.getUnit("mm"));
  u = ureg.getUnit("mm",true);
  CHECK(u.scale() == Approx(0.001));
  u = ureg.getUnit("millim",true);
  CHECK(u.scale() == Approx(0.001));

  CHECK(q.value() == Approx(200));
  // getUnit returns a unit that is in the registry
  CHECK(q.to(ureg.getUnit("m")).value() == Approx(2));
  CHECK_THROWS(q.to(ureg.getUnit("s")));

  q = ureg.makeQuantity<double>(0.25, "s");
  CHECK(q.value() == Approx(0.25));
  // makeUnit can create a unit from the registry,
  // including SI prefixed versions of the units in the registry
  CHECK(q.to(ureg.makeUnit("ms")).value() == Approx(250));
  CHECK_THROWS(q.to(ureg.getUnit("m")));

  q = ureg.makeQuantity<double>(100, "cal");
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
    Quantity<double> distance = ureg.makeQuantity<double>(100, "ft");
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

  Quantity<double> q = ureg.makeQuantity<double>(200, "cm");

  CHECK(q.value() == Approx(200));
  // getUnit returns a unit that is in the registry
  CHECK(q.to(ureg.getUnit("m")).value() == Approx(2));
  CHECK_THROWS(q.to(ureg.getUnit("s")));

  q = ureg.makeQuantity<double>(0.25, "s");
  CHECK(q.value() == Approx(0.25));
  // makeUnit can create a unit from the registry,
  // including SI prefixed versions of the units in the registry
  CHECK(q.to(ureg.makeUnit("ms")).value() == Approx(250));
  CHECK_THROWS(q.to(ureg.getUnit("m")));

  q = ureg.makeQuantity<double>(100, "cal");
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
    Quantity<double> distance = ureg.makeQuantity<double>(100, "ft");
    boost::units::quantity<boost::units::si::length> L =
        distance.to<boost::units::si::length>();
    CHECK(boost::units::quantity_cast<double>(L) == Approx(30.48));
  }
}
