#pragma once

/** @file add_unit_expression_parser.hpp
 * @author C.D. Clark III
 * @date 03/29/22
 */

namespace unit_convert
{
/**
 * @brief a mixin class to add a unit expression parser to a unit registry.
 *
 * A unit registry maps symbols/names to unit instances, which are retrieved
 * with `.get_unit(string)`. This mixin adds a method named `.make_unit(...)`
 * that can parse unit expresions (i.e. m/s^2) and build, builds the
 * corresponding unit from the underlying unit registry, and returns it.
 */
template <typename BASE>
class add_unit_expression_parser : public BASE
{
 public:
  using base_type = BASE;
  using unit_parser_type = unit_registry_parser<base_type>;
  using unit_type = typename BASE::unit_type;
  using quantity = typename BASE::quantity;

  add_unit_expression_parser() : m_Parser(*this) {}

  /**
   * @ Create a unit from a unit expression.
   *
   * A unit expressino is a combination of units in the store that are multiple,
   * divided, raised to a power, etc.
   *
   * For example, if the store contains "m", "s", and "kg", then
   *
   * "kg (m/s)^2"
   *
   * will return the unit for Joul.
   */
  unit_type make_unit(std::string a_unit_expr) const
  {
    unit_type ret_unit;
    // check if the unit is already stored in the registry
    auto ptr = this->m_UnitStore.find(a_unit_expr);
    if (ptr != this->m_UnitStore.end()) {  // unit was already in registry
      ret_unit = ptr->second;
    } else {
      auto it = a_unit_expr.begin();
      bool sucess = qi::parse(it, a_unit_expr.end(), m_Parser, ret_unit);
      if (!sucess or it != a_unit_expr.end()) {
        throw std::runtime_error("Parsing Error: Cannot parse unit string '" +
                                 a_unit_expr + "'.");
      }
    }
    return ret_unit;
  }

  /**
   * Override the make_quantity(...) method to add unit expressions
   */
  template <typename T>
  quantity make_quantity(T a_val, const std::string& a_unit_expression) const
  {
    return BASE::make_quantity(a_val, this->make_unit(a_unit_expression));
  }


 protected:
  unit_parser_type m_Parser;
};
}  // namespace unit_convert
