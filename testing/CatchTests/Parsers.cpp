#include "catch.hpp"

#include <UnitConvert.hpp>
#include <unit_convert/unit_registries//basic_unit_registry.hpp>
#include <unit_convert/unit_registries/parsers.hpp>
#include <unit_convert/units/basic_unit.hpp>
#include <numeric>

TEST_CASE("Spirit Testing")
{
  SECTION("Basic")
  {
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    std::string text = "1.3";
    double val = 0;

    auto it = text.begin();
    bool r =
        qi::parse(it, text.end(), qi::double_[qi::_val = qi::_1 + 10], val);

    CHECK(r);
    CHECK(val == Approx(11.3));
  }

  SECTION("Unit Parsing")
  {
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    using qi::_1;
    using qi::_val;

    std::string text = "1.3 m / s / ( cm / s )";
    double val = 0;
    std::string unit;
    std::vector<char> buffer;

    auto it = text.begin();
    bool r =
        qi::phrase_parse(it, text.end(), qi::double_ >> qi::lexeme[*qi::char_],
                         ascii::space, val, unit);

    CHECK(r);
    CHECK(it == text.end());
    CHECK(val == Approx(1.3));
    CHECK(unit == "m / s / ( cm / s )");
  }
}

TEST_CASE("Unit String Parsing")
{
  using namespace UnitConvert;

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
  ureg.addUnit("N = 1 kg m / s^2");

  SECTION("Expression")
  {
    Unit u = ureg.getUnit("");

    BaseDimension<Dimension::Name::Length> L;
    BaseDimension<Dimension::Name::Mass> M;
    BaseDimension<Dimension::Name::Time> T;
    BaseDimension<Dimension::Name::ElectricalCurrent> I;
    BaseDimension<Dimension::Name::Temperature> THETA;
    BaseDimension<Dimension::Name::Amount> N;
    BaseDimension<Dimension::Name::LuminousIntensity> J;
    BaseDimension<Dimension::Name::Dimensionless> D;

    std::string unit = "m";
    auto it = unit.begin();
    auto r = qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == L);
    CHECK(u.scale() == 1);

    unit = "m ";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == L);
    CHECK(u.scale() == 1);

    unit = " m";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == L);
    CHECK(u.scale() == 1);

    unit = " m  ";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == L);
    CHECK(u.scale() == 1);

    unit = "m*s";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == L * T);
    CHECK(u.scale() == 1);

    unit = "m/s";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == L / T);
    CHECK(u.scale() == 1);

    unit = "m*m/s";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == L * L / T);
    CHECK(u.scale() == 1);

    unit = "m/s*m";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == L * L / T);
    CHECK(u.scale() == 1);

    unit = "s^3";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == T * T * T);
    CHECK(u.scale() == 1);

    unit = "s^-3";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == D / T / T / T);
    CHECK(u.scale() == 1);

    unit = "kg * m / s^2";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M * L / T / T);
    CHECK(u.scale() == 1);

    unit = "kg   m";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M * L);
    CHECK(u.scale() == 1);

    unit = "kg   m / s^2";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M * L / T / T);
    CHECK(u.scale() == 1);

    unit = "kg / (m / s^2)";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M * T * T / L);
    CHECK(u.scale() == 1);

    unit = "kg * (m / s)^2";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M * L * L / T / T);
    CHECK(u.scale() == 1);

    unit = "kg   (m / s)^2";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M * L * L / T / T);
    CHECK(u.scale() == 1);

    unit = "kg   (m / s)^-2";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M * T * T / L / L);
    CHECK(u.scale() == 1);

    unit = "kg * m * s^-2";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M * L / T / T);
    CHECK(u.scale() == 1);

    unit = "3 * kg";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M);
    CHECK(u.scale() == 3);

    unit = "3 * kg";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M);
    CHECK(u.scale() == 3);

    unit = "-3 * kg";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M);
    CHECK(u.scale() == -3);

    unit = "3 * kg / 2 / m";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M / L);
    CHECK(u.scale() == Approx(3 / 2.));

    unit = "3 * kg / -2 / m";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M / L);
    CHECK(u.scale() == Approx(-3 / 2.));

    unit = "3 * kg * 2 * m * s^-2";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == M * L / T / T);
    CHECK(u.scale() == 6);

    unit = "100 * m - 20";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == L);
    CHECK(u.scale() == 100);
    CHECK(u.offset() == -20);

    unit = "100 * m + 30";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == L);
    CHECK(u.scale() == 100);
    CHECK(u.offset() == 30);

    unit = "100 * m + -30";
    it = unit.begin();
    qi::parse(it, unit.end(), ureg.getUnitParser(), u);
    CHECK(r);
    CHECK(unit.end() - it == 0);
    CHECK(u.dimension() == L);
    CHECK(u.scale() == 100);
    CHECK(u.offset() == -30);

    unit = "(3/10) m + 30";
    it = unit.begin();
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

TEST_CASE("Base Dimension Symbol Parsing")
{
  using namespace UnitConvert;
  UnitRegistry::BaseDimensionSymbolParser parser;

  std::string dimension_str;
  auto it = dimension_str.begin();
  Dimension D;
  auto powers = D.powers();
  bool r;

  dimension_str = "L";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Length] == 1);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 1);

  dimension_str = "M";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Mass] == 1);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 1);

  dimension_str = "T";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Time] == 1);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 1);

  dimension_str = "I";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::ElectricalCurrent] == 1);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 1);

  dimension_str = "THETA";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Temperature] == 1);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 1);

  dimension_str = "N";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Amount] == 1);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 1);

  dimension_str = "J";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::LuminousIntensity] == 1);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 1);

  // not a dimension
  dimension_str = "K";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser, D);
  CHECK(!r);
  CHECK(dimension_str.end() - it == 1);
  powers = D.powers();
  CHECK(D[Dimension::Name::LuminousIntensity] == 1);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 1);
}

TEST_CASE("Dimension String Parsing")
{
  using namespace UnitConvert;
  UnitRegistry::DimensionParser parser;

  std::string dimension_str;
  auto it = dimension_str.begin();
  Dimension D;
  auto powers = D.powers();
  bool r;

  dimension_str = "L";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser.term, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Length] == 1);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 1);

  dimension_str = "L*T";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser.term, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Length] == 1);
  CHECK(D[Dimension::Name::Time] == 1);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 2);

  dimension_str = "L/T";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser.term, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Length] == 1);
  CHECK(D[Dimension::Name::Time] == -1);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 0);

  dimension_str = "L^2";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser.term, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Length] == 2);
  CHECK(D[Dimension::Name::Time] == 0);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 2);

  dimension_str = "T^-2";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser.term, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Length] == 0);
  CHECK(D[Dimension::Name::Time] == -2);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == -2);

  dimension_str = "[L / T]";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Length] == 1);
  CHECK(D[Dimension::Name::Time] == -1);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 0);

  dimension_str = "[M (L / T)^2]";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Mass] == 1);
  CHECK(D[Dimension::Name::Length] == 2);
  CHECK(D[Dimension::Name::Time] == -2);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 1);

  dimension_str = "[L^2 M^1 T^-2 I^0 THETA^0 N^0 J^0]";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Mass] == 1);
  CHECK(D[Dimension::Name::Length] == 2);
  CHECK(D[Dimension::Name::Time] == -2);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 1);

  dimension_str = "[L*L*L]";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Mass] == 0);
  CHECK(D[Dimension::Name::Length] == 3);
  CHECK(D[Dimension::Name::Time] == 0);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 3);

  dimension_str = "[L*L*L/T/T]";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Mass] == 0);
  CHECK(D[Dimension::Name::Length] == 3);
  CHECK(D[Dimension::Name::Time] == -2);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 1);

  dimension_str = "[1]";
  it = dimension_str.begin();
  r = qi::parse(it, dimension_str.end(), parser, D);
  CHECK(r);
  CHECK(dimension_str.end() - it == 0);
  powers = D.powers();
  CHECK(D[Dimension::Name::Mass] == 0);
  CHECK(D[Dimension::Name::Length] == 0);
  CHECK(D[Dimension::Name::Time] == 0);
  CHECK(std::accumulate(powers.begin(), powers.end(), 0) == 1);
}

TEST_CASE("1.0 Parsers")
{
  SECTION("SI Prefixes")
  {
    unit_convert::si_prefix_parser parser;

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

  SECTION("dimension symbol parser")
  {
    std::string dim_str;
    auto it = dim_str.begin();
    bool r;
    SECTION("generic symbols")
    {
      unit_convert::basic_dimension<5> dim;
      unit_convert::dimension_symbol_parser<unit_convert::basic_dimension<5>>
          parser;
      for (size_t i = 0; i < 5; ++i) {
        parser.add_dimension(std::string(1, static_cast<char>(i + 65)), i);
      }

      dim_str = "A";
      it = dim_str.begin();
      r = qi::parse(it, dim_str.end(), parser, dim);
      CHECK(r);
      CHECK(dim == unit_convert::basic_dimension<5>(0));

      dim_str = "B";
      it = dim_str.begin();
      r = qi::parse(it, dim_str.end(), parser, dim);
      CHECK(r);
      CHECK(dim == unit_convert::basic_dimension<5>(1));

      dim_str = "C";
      it = dim_str.begin();
      r = qi::parse(it, dim_str.end(), parser, dim);
      CHECK(r);
      CHECK(dim == unit_convert::basic_dimension<5>(2));

      dim_str = "D";
      it = dim_str.begin();
      r = qi::parse(it, dim_str.end(), parser, dim);
      CHECK(r);
      CHECK(dim == unit_convert::basic_dimension<5>(3));

      dim_str = "E";
      it = dim_str.begin();
      r = qi::parse(it, dim_str.end(), parser, dim);
      CHECK(r);
      CHECK(dim == unit_convert::basic_dimension<5>(4));

      dim_str = "F";
      it = dim_str.begin();
      r = qi::parse(it, dim_str.end(), parser, dim);
      CHECK(!r);
    }

    SECTION("custom symbols")
    {
      unit_convert::basic_dimension<2> dim;
      unit_convert::dimension_symbol_parser<unit_convert::basic_dimension<2>>
          parser;
      parser.add_dimension("L", 0);
      parser.add_dimension("T", 1);

      dim_str = "L";
      CHECK(qi::parse(dim_str.begin(), dim_str.end(), parser, dim));
      CHECK(dim == unit_convert::basic_dimension<2>(0));

      dim_str = "T";
      CHECK(qi::parse(dim_str.begin(), dim_str.end(), parser, dim));
      CHECK(dim == unit_convert::basic_dimension<2>(1));
    }
    SECTION("SI dimension parsers")
    {
      using namespace unit_convert;
      si_dimension dim;
      si_dimension_symbol_parser parser;
      si_dimension ONE;
      si_dimension L(si_dimension::name::Length);
      si_dimension M(si_dimension::name::Mass);
      si_dimension T(si_dimension::name::Time);
      si_dimension I(si_dimension::name::ElectricalCurrent);
      si_dimension THETA(si_dimension::name::Temperature);
      si_dimension N(si_dimension::name::Amount);
      si_dimension J(si_dimension::name::LuminousIntensity);

      SECTION("Single dimension")
      {
        dim_str = "L";
        CHECK(qi::parse(dim_str.begin(), dim_str.end(), parser, dim));
        CHECK(dim == L);

        dim_str = "M";
        CHECK(qi::parse(dim_str.begin(), dim_str.end(), parser, dim));
        CHECK(dim == M);

        dim_str = "T";
        CHECK(qi::parse(dim_str.begin(), dim_str.end(), parser, dim));
        CHECK(dim == T);

        dim_str = "I";
        CHECK(qi::parse(dim_str.begin(), dim_str.end(), parser, dim));
        CHECK(dim == I);

        dim_str = "THETA";
        CHECK(qi::parse(dim_str.begin(), dim_str.end(), parser, dim));
        CHECK(dim == THETA);

        dim_str = "N";
        CHECK(qi::parse(dim_str.begin(), dim_str.end(), parser, dim));
        CHECK(dim == N);

        dim_str = "J";
        CHECK(qi::parse(dim_str.begin(), dim_str.end(), parser, dim));
        CHECK(dim == J);
      }
      SECTION("Derived Dimensions")
      {
        unit_convert::dimension_expression_parser<
            unit_convert::si_dimension_symbol_parser>
            parser;

        dim_str = "[L L]";
        CHECK(qi::parse(dim_str.begin(), dim_str.end(), parser, dim));
        CHECK(dim == L * L);

        dim_str = "[L*L]";
        CHECK(qi::parse(dim_str.begin(), dim_str.end(), parser, dim));
        CHECK(dim == L * L);

        dim_str = "[L/L]";
        CHECK(qi::parse(dim_str.begin(), dim_str.end(), parser, dim));
        CHECK(dim == ONE);

        dim_str = "[L/T]";
        CHECK(qi::parse(dim_str.begin(), dim_str.end(), parser, dim));
        CHECK(dim == L / T);

        dim_str = "[L^2]";
        CHECK(qi::parse(dim_str.begin(), dim_str.end(), parser, dim));
        CHECK(dim == L * L);

        dim_str = "[M (L/T)**2]";
        CHECK(qi::parse(dim_str.begin(), dim_str.end(), parser, dim));
        CHECK(dim == M * L * L / T / T);
      }
    }
  }
}
TEST_CASE("unit_registry_parser")
{
  using namespace unit_convert;
  using dim_type = basic_dimension<3>;
  using unit_type = basic_unit<dim_type, double>;
  basic_unit_registry<unit_type> ureg;
  unit_expression_parser<basic_unit_registry<unit_type>> parser(ureg);
  CHECK(ureg.size() == 0);

  unit_type meter(0);
  unit_type second(1);
  unit_type kelvin(2);
  ureg.add_unit("m", basic_dimension<3>(0));
  ureg.add_unit("s", basic_dimension<3>(1));
  ureg.add_unit("K", basic_dimension<3>(2));

  unit_type unit;
  std::string unit_str = "m";

  CHECK(qi::parse(unit_str.begin(), unit_str.end(), parser, unit));
  CHECK(unit == meter);

  unit_str = "m**2";
  CHECK(qi::parse(unit_str.begin(), unit_str.end(), parser, unit));
  CHECK(unit == meter*meter);

  unit_str = "(m/s)**2";
  CHECK(qi::parse(unit_str.begin(), unit_str.end(), parser, unit));
  CHECK(unit == meter*meter/second/second);

  unit_str = "(km/s)**2";
  CHECK_THROWS(qi::parse(unit_str.begin(), unit_str.end(), parser, unit));

}
