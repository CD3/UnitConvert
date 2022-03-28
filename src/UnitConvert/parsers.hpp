#pragma once
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <string>
#include "./si_dimension.hpp"

/** @file parsers.hpp
 * @author C.D. Clark III
 * @date 03/27/22
 */

namespace unit_convert
{
namespace spt = boost::spirit;
namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

/**
 * Symbol table parser to get exponent for all SI prefixes.
 */
struct si_prefix_parser : qi::symbols<char, int> {
  si_prefix_parser()
  {
    add("Y", 24)("yotta", 24)("Z", 21)("zetta", 21)("E", 18)("exa", 18)(
        "P", 15)("peta", 15)("T", 12)("tera", 12)("G", 9)("giga", 9)("M", 6)(
        "mega", 6)("k", 3)("kilo", 3)("h", 2)("hecto", 2)("da", 1)("deca", 1)(
        "d", -1)("deci", -1)("c", -2)("centi", -2)("m", -3)("milli", -3)(
        "u", -6)("micro", -6)("n", -9)("nano", -9)("p", -12)("pico", -12)(
        "f", -15)("femto", -15)("a", -18)("atto", -18)("z", -21)("zepto", -21)(
        "y", -24)("yocto", -24);
  }
};

/**
 * Symbol table parser to return a dimension instance for dimension symbol.
 */
template <typename DIMENSION_TYPE>
struct dimension_symbol_parser : qi::symbols<char, DIMENSION_TYPE> {
  using dimension_type = DIMENSION_TYPE;
  using base_type = qi::symbols<char, DIMENSION_TYPE>;
  dimension_symbol_parser() = default;
  template<typename INDEX_TYPE>
  void add_dimension(std::string a_symbol, INDEX_TYPE a_index)
  {
    this->add(std::move(a_symbol), dimension_type(a_index) );
  }

};

struct si_dimension_symbol_parser : dimension_symbol_parser<si_dimension> {
  using dimension_type = si_dimension;
  si_dimension_symbol_parser()
  {
    add("L", si_dimension(si_dimension::name::Length))(
        "M", si_dimension(si_dimension::name::Mass))(
        "T", si_dimension(si_dimension::name::Time))(
        "I", si_dimension(si_dimension::name::ElectricalCurrent))(
        "THETA", si_dimension(si_dimension::name::Temperature))(
        "N", si_dimension(si_dimension::name::Amount))(
        "J", si_dimension(si_dimension::name::LuminousIntensity))(
        "1", si_dimension());
  }
};

/**
 * A Boost.Spirit grammar for parsing SI dimension strings.
 */
template <typename DIMENSION_SYMBOL_PARSER_TYPE>
struct dimension_expression_parser
    : spt::qi::grammar<std::string::iterator, typename DIMENSION_SYMBOL_PARSER_TYPE::dimension_type()> {
  using iterator = std::string::iterator;
  using dimension_symbol_parser_type = DIMENSION_SYMBOL_PARSER_TYPE;
  using dimension_type = typename dimension_symbol_parser_type::dimension_type;

  dimension_symbol_parser_type dimension_symbol;
  qi::rule<iterator, dimension_type()> factor, term, group, dimension;
  qi::rule<iterator, int()> exponent;
  qi::rule<iterator> mul, div, pow, add, sub;

  dimension_expression_parser() : dimension_expression_parser::base_type(dimension)
  {
    exponent = qi::int_;
    auto space = qi::lit(" ");

    mul = *space >> "*" >> *space | +space;
    div = *space >> "/" >> *space;
    pow = *space >> (qi::lit("^") | qi::lit("**")) >> *space;
    add = *space >> "+" >> *space;
    sub = *space >> "-" >> *space;

    // a factor is a dimension symbol or a group, possibly raised to an
    factor = (dimension_symbol | group)[qi::_val = qi::_1] >>
             *(pow >> exponent)[qi::_val ^= qi::_1];

    // a term is a factor, possibly multiplied or divided by another factor
    term = factor[qi::_val = qi::_1] >> *(mul >> factor[qi::_val *= qi::_1] |
                                          div >> factor[qi::_val /= qi::_1]);

    // a group is a term wrapped in parenthesis
    group = '(' >> term[qi::_val = qi::_1] >> ')';

    // a dimension is a term wrapped in square brackets
    dimension = qi::lit("[") >> *space >> term[qi::_val = qi::_1] >> *space
    >>
                qi::lit("]");
  }
};

/**
 * A Boost.Spirit grammar for parsing unit strings.
 */
// template <typename unit_type>
// struct UnitParser : spt::qi::grammar<std::string::iterator, unit_type()> {
//   using Iterator = std::string::iterator;
//   using ThisType = UnitParser;

//   qi::rule<Iterator, unit_type()> named_unit, factor, term, group, scale,
//       expression, unit;
//   qi::rule<Iterator, double()> offset;
//   qi::rule<Iterator, int()> exponent;
//   qi::rule<Iterator> mul, div, pow, add, sub;
//   qi::rule<Iterator, char()> unit_name_begin_chars, unit_name_other_chars;

//   const UnitRegistry& ureg;

//   /**
//    * compute a unit raised to an integer power)
//    */
//   unit_type exponentiate(const unit_type& b, const int e);

//   /**
//    * retrieve a named unit from the registry
//    */
//   unit_type getUnitFromRegistry(const std::string& unit);

//   UnitParser(const UnitRegistry& registry);
// };

}  // namespace unit_convert
