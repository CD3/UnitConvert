#pragma once
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <string>
#include "../dimensions/basic_dimension.hpp"
#include "../dimensions/si_dimension.hpp"

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
  template <typename INDEX_TYPE>
  void add_dimension(std::string a_symbol, INDEX_TYPE a_index)
  {
    this->add(std::move(a_symbol), dimension_type(a_index));
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
    : spt::qi::grammar<
          std::string::iterator,
          typename DIMENSION_SYMBOL_PARSER_TYPE::dimension_type()> {
  using iterator = std::string::iterator;
  using dimension_symbol_parser_type = DIMENSION_SYMBOL_PARSER_TYPE;
  using dimension_type = typename dimension_symbol_parser_type::dimension_type;

  dimension_symbol_parser_type dimension_symbol;
  qi::rule<iterator, dimension_type()> factor, term, group, dimension;
  qi::rule<iterator, int()> exponent;
  qi::rule<iterator> mul, div, pow, add, sub;

  dimension_expression_parser()
      : dimension_expression_parser::base_type(dimension)
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
    dimension = qi::lit("[") >> *space >> term[qi::_val = qi::_1] >> *space >>
                qi::lit("]");
  }
};

/**
 * A Boost.Spirit grammar to parse unit strings for a unit registry.
 */
template <typename UNIT_REGISTRY_TYPE>
struct unit_expression_parser
    : spt::qi::grammar<std::string::iterator,
                       typename UNIT_REGISTRY_TYPE::unit_type()> {
  using iterator = std::string::iterator;
  using unit_registry_type = UNIT_REGISTRY_TYPE;
  using unit_type = typename unit_registry_type::unit_type;

  qi::rule<iterator, unit_type()> named_unit, factor, term, group, scale,
      expression, unit;
  qi::rule<iterator, double()> offset;
  qi::rule<iterator, int()> exponent;
  qi::rule<iterator> mul, div, pow, add, sub;
  qi::rule<iterator, char()> unit_name_begin_chars, unit_name_other_chars;

  unit_expression_parser(const unit_registry_type& a_reg)
      : unit_expression_parser::base_type(unit), m_unit_registry(a_reg)
  {
    offset = qi::double_;
    exponent = qi::int_;

    scale = qi::double_[qi::_val *= qi::_1];

    auto space = qi::lit(" ");

    mul = *space >> "*" >> *space | +space;
    div = *space >> "/" >> *space;
    pow = *space >> (qi::lit("^") | qi::lit("**")) >> *space;
    add = *space >> "+" >> *space;
    sub = *space >> "-" >> *space;

    unit_name_begin_chars = qi::char_("a-zA-Z");
    unit_name_other_chars = qi::char_("a-zA-Z_0-9");
    named_unit =
        spt::as_string[+unit_name_begin_chars >> *unit_name_other_chars]
                      [qi::_val = phx::bind(&unit_registry_type::get_single_unit,
                                            &m_unit_registry, qi::_1)];

    factor = (named_unit | scale | group)[qi::_val = qi::_1] >>
             *(pow >> exponent[qi::_val ^= qi::_1 ]);

    term = factor[qi::_val = qi::_1] >> *(mul >> factor[qi::_val *= qi::_1] |
                                          div >> factor[qi::_val /= qi::_1]);

    group = '(' >> term[qi::_val = qi::_1] >> ')';

    expression = *space >> term[qi::_val = qi::_1] >>
                 *(add >> offset[qi::_val += qi::_1] |
                   sub >> offset[qi::_val -= qi::_1]) >>
                 *space;

    unit = expression;
  }

 protected:
  const unit_registry_type& m_unit_registry;
};

}  // namespace unit_convert
