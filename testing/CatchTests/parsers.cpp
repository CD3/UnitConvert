#include "catch.hpp"

#include <numeric>
#include <unit_convert/unit_registries/basic_unit_registry.hpp>
#include <unit_convert/unit_registries/parsers.hpp>
#include <unit_convert/units/basic_unit.hpp>

TEST_CASE("Spirit Testing")
{
  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;
  SECTION("Basic")
  {
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

TEST_CASE("1.0 Parsers")
{
  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;
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
  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;
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
  CHECK(unit == meter * meter);

  unit_str = "(m/s)**2";
  CHECK(qi::parse(unit_str.begin(), unit_str.end(), parser, unit));
  CHECK(unit == meter * meter / second / second);

  unit_str = "(km/s)**2";
  CHECK_THROWS(qi::parse(unit_str.begin(), unit_str.end(), parser, unit));


  unit_str = "K - 273.15";
  CHECK(qi::parse(unit_str.begin(), unit_str.end(), parser, unit));
  CHECK(unit.offset() == Approx(-273.15));

  unit_str = "Q: K - 273.15";
  CHECK(qi::parse(unit_str.begin(), unit_str.end(), parser, unit));
  CHECK(unit.offset() == Approx(273.15));

}
