#pragma once

/** @file expression_unit_registry.hpp
 * @author C.D. Clark III
 * @date 04/02/22
 */
#include <optional>
#include "./parsers.hpp"
#include "./unit_registry_base.hpp"

namespace unit_convert
{
template <typename UNIT_TYPE, typename KEY_TYPE = std::string>
class expression_unit_registry : public unit_registry_base<UNIT_TYPE, KEY_TYPE>
{
 public:
  using this_type = expression_unit_registry<UNIT_TYPE, KEY_TYPE>;
  using base_type = unit_registry_base<UNIT_TYPE>;
  using unit_type = typename base_type::unit_type;
  using key_type = typename base_type::key_type;
  using dimension_type = typename base_type::dimension_type;
  template <typename T>
  using base_quantity_type = typename base_type::template base_quantity_type<T>;
  template <typename T>
  using registered_quantity_type =
      typename base_type::template registered_quantity_type<T>;

  expression_unit_registry()
      : m_si_prefix_parser(),
        m_unit_expression_parser(*this),
        m_dimension_expression_parser()
  {
  }

  using base_type::make_quantity;
  template <typename T>
  registered_quantity_type<T> make_quantity(const std::string& a_quantity) const
  {
    double value = 0;
    std::string unit;
    // parse quantity string
    // a quantity should have a numerical value and a unit that makeUnit will
    // parse
    auto it = a_quantity.begin();
    auto r = qi::parse(it, a_quantity.end(),
                       -qi::double_ >> qi::as_string[+qi::char_], value, unit);
    if (!r) {
      throw std::runtime_error(
          "ERROR: Could not parse string to form a quantity: " + a_quantity +
          "\nQuantity strings should be a numerical value followed by a valid "
          "unit string..");
    }
    return this->make_quantity<T>(static_cast<T>(value), unit);
  }

  void add_dimension_symbol(const std::string& a_symbol, dimension_type a_dim)
  {
    m_dimension_expression_parser.dimension_symbol_parser.add(a_symbol,
                                                              std::move(a_dim));
  }

  /**
   * @brief Return a named unit from the registry.
   *
   * A named unit is a unit that
   */
  unit_type get_named_unit(const key_type& a_key) const
  {
    auto ptr = this->m_unit_store.find(a_key);
    if (ptr != this->m_unit_store.end()) {
      // if the unit is in the store, return it.
      return ptr->second;
    } else {
      // otherwise,
      // check if the unit is an SI prefix'ed version of
      // a unit in the store.
      auto it = a_key.begin();
      int power = 0;
      if (qi::parse(it, a_key.end(), m_si_prefix_parser, power) &&
          it != a_key.end()) {
        std::string unit(it, a_key.end());
        ptr = this->m_unit_store.find(unit);
        if (ptr != this->m_unit_store.end()) {
          return ptr->second * pow(10, power);
        }
      }
      // well, can't find the unit in the store.
      // throw an exception.
      throw std::runtime_error("Error: unit '" + a_key +
                               "' does not exist in the registry.");
    }
    return this->base_type::get_unit(a_key);
  }

  /**
   * Return a unit specified by a name or unit expression.
   */
  unit_type get_unit(const key_type& a_unit_expr) const
  {
    unit_type ret_unit;
    auto ptr = this->m_unit_store.find(a_unit_expr);
    if (ptr != this->m_unit_store.end()) {  // unit was already in registry
      ret_unit = ptr->second;
    } else {
      key_type unit_expr(a_unit_expr);
      auto it = unit_expr.begin();
      bool sucess =
          qi::parse(it, unit_expr.end(), m_unit_expression_parser, ret_unit);
      if (sucess and it == unit_expr.end()) {
      } else {
        throw std::runtime_error("Parsing Error: Cannot parse unit string '" +
                                 a_unit_expr + "'.");
      }
    }
    return ret_unit;
  }

  using base_type::add_unit;
  /**
   * Parse a unit equation and add the defined unit to the registry.
   */
  void add_unit(const key_type& a_unit_equation)
  {
    auto it = a_unit_equation.begin();

    std::optional<double> scale;
    std::string LHS, RHS;
    bool is_quantity_expression;

    // parse unit equation
    // LHS should be a new named unit (no derived units) with an optional scale.
    // RHS can be an arbitrary expression of units or dimensions.
    // examples: 1 J = 1 kg * m^2 / s^s
    //           100 cm = 1 m
    //           kg = [M]
    auto space = qi::lit(" ");
    auto eq = qi::lit("=");
    auto uchars = qi::char_("a-zA-Z_/*+-^");
    auto r =
        qi::parse(it, a_unit_equation.end(),
                  -qi::matches["Q:"] >>
                  -qi::double_ >> *space >> qi::as_string[+uchars] >> *space >>
                      eq >> *space >> qi::as_string[+qi::char_] >> *space,
                  is_quantity_expression, scale, LHS, RHS);

    if (!scale) scale = 1;
    if(is_quantity_expression)
    {
      RHS = "Q: "+RHS;
      scale = 1./scale.value();
    }
    if (r) {
      bool error = true;

      auto it = RHS.begin();
      if (qi::parse(it, RHS.end(), m_unit_expression_parser) &&
          it == RHS.end()) {
        this->add_unit(LHS, get_unit(RHS) / scale.value());
        error = false;
      } else {
        it = RHS.begin();
        if (qi::parse(it, RHS.end(), m_dimension_expression_parser) &&
            it == RHS.end()) {
          dimension_type dim;
          qi::parse(RHS.begin(), RHS.end(), m_dimension_expression_parser, dim);
          this->add_unit(LHS, unit_type(1, dim) / scale.value());
          error = false;
        }
      }

      if (error) {
        throw std::runtime_error(
            "Could not parse right-hand side of unit equation: " +
            a_unit_equation);
      }

    } else {
      throw std::runtime_error("Could not parse unit equation: " +
                               a_unit_equation);
    }
  }

 protected:
  using expression_parser_type = unit_expression_parser<this_type>;
  si_prefix_parser m_si_prefix_parser;
  expression_parser_type m_unit_expression_parser;
  dimension_expression_parser<dimension_symbol_parser<dimension_type>>
      m_dimension_expression_parser;
};

}  // namespace unit_convert
