#pragma once
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
/** @file parsers.hpp
 * @author C.D. Clark III
 * @date 03/27/22
 */

namespace unit_convert
{
namespace spt = boost::spirit;
namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

struct unit_parser {
  /**
   * Symbol table parser to get exponent for all SI prefixes.
   */
  struct SIPrefixParser : qi::symbols<char, int> {
    SIPrefixParser()
    {
      add("Y", 24)("yotta", 24)("Z", 21)("zetta", 21)("E", 18)("exa", 18)(
          "P", 15)("peta", 15)("T", 12)("tera", 12)("G", 9)("giga", 9)("M", 6)(
          "mega", 6)("k", 3)("kilo", 3)("h", 2)("hecto", 2)("da", 1)("deca", 1)(
          "d", -1)("deci", -1)("c", -2)("centi", -2)("m", -3)("milli", -3)(
          "u", -6)("micro", -6)("n", -9)("nano", -9)("p", -12)("pico", -12)(
          "f", -15)("femto", -15)("a", -18)("atto", -18)("z", -21)(
          "zepto", -21)("y", -24)("yocto", -24);
    }
  };

  /**
   * A Boost.Spirit grammar for parsing unit strings.
   */
  struct UnitParser : spt::qi::grammar<std::string::iterator, Unit()> {
    using Iterator = std::string::iterator;
    using ThisType = UnitParser;

    qi::rule<Iterator, Unit()> named_unit, factor, term, group, scale, expression, unit;
    qi::rule<Iterator, double()> offset;
    qi::rule<Iterator, int()> exponent;
    qi::rule<Iterator> mul, div, pow, add, sub;
    qi::rule<Iterator, char()> unit_name_begin_chars, unit_name_other_chars;

    const UnitRegistry& ureg;

    /**
     * compute a unit raised to an integer power)
     */
    Unit exponentiate(const Unit& b, const int e);

    /**
     * retrieve a named unit from the registry
     */
    Unit getUnitFromRegistry(const std::string& unit);

    UnitParser(const UnitRegistry& registry);
  };

  /**
   * Symbol table parser to return base dimension for SI dimension symbols.
   */
  struct SIBaseDimensionSymbolParser : qi::symbols<char, Dimension> {
    SIBaseDimensionSymbolParser()
    {
      add("L", Dimension::Name::Length)("M", Dimension::Name::Mass)(
          "T", Dimension::Name::Time)("I", Dimension::Name::ElectricalCurrent)(
          "THETA", Dimension::Name::Temperature)("N", Dimension::Name::Amount)(
          "J", Dimension::Name::LuminousIntensity)(
          "1", Dimension::Name::Dimensionless);
    }
  };

  /**
   * A Boost.Spirit grammar for parsing SI dimension strings.
   */
  struct SIDimensionParser
      : spt::qi::grammar<std::string::iterator, Dimension()> {
    using Iterator = std::string::iterator;
    using ThisType = SIDimensionParser;

    SIBaseDimensionSymbolParser base_dimension_symbol;
    qi::rule<Iterator, Dimension()> factor, term, group, dimension;
    qi::rule<Iterator, int()> exponent;
    qi::rule<Iterator> mul, div, pow, add, sub;

    /**
     * compute a dimension raised to an integer power)
     */
    Dimension exponentiate(const Dimension& b, const int e);

    SIDimensionParser();
  };
}

}  // namespace unit_convert
