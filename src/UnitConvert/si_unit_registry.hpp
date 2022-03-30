#pragma once

/** @file si_unit_registry.hpp
 * @author C.D. Clark III
 * @date 03/27/22
 */

#include "./basic_unit_registry.hpp"
#include "./parsers.hpp"
#include "./si_unit.hpp"

namespace unit_convert
{
template <typename T = double>
class si_unit_registry : public basic_unit_registry<si_unit<T>>
{
 public:
  using base_type = basic_unit_registry<si_unit<T>>;
  using unit_type = typename base_type::unit_type;
  using value_type = typename base_type::value_type;
  using dimension_type = typename base_type::dimension_type;
  using quantity = typename base_type::quantity;

  si_prefix_parser m_si_prefix_parser;

  /**
   * Retrieves a unit from the store given its name/symbol.
   *
   * @param a_symbol the unit's name as stored in the registry
   * @param a_try_si_prefix if true and the unit symbol was not found in the
   * registry, see if the symbol is an SI prefix of a unit that is in the
   * registry.
   */
  unit_type get_unit(std::string a_symbol, bool a_try_si_prefix = true) const
  {
    auto ptr = this->m_UnitStore.find(a_symbol);
    if (ptr != this->m_UnitStore.end()) {
      return ptr->second;
    } else {
      if (a_try_si_prefix) {
        // check if the unit is an SI prefix'ed version of
        // a unit in the store.
        auto it = a_symbol.begin();
        int power = 0;
        if (qi::parse(it, a_symbol.end(), m_si_prefix_parser, power) &&
            it != a_symbol.end()) {
          std::string symbol(it, a_symbol.end());
          ptr = this->m_UnitStore.find(symbol);
          if (ptr != this->m_UnitStore.end()) {
            return ptr->second * pow(10, power);
          }
        }
      }
      // can't find the unit in the store.
      // throw an exception.
      throw std::runtime_error("Error: unit '" + a_symbol +
                               "' does not exist in the registry.");
    }
    return ptr->second;
  }
};

}  // namespace unit_convert
