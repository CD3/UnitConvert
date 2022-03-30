#pragma once

/** @file add_si_prefixes.hpp
 * @author C.D. Clark III
 * @date 03/27/22
 */

#include "./basic_unit_registry.hpp"
#include "./parsers.hpp"
#include "./si_unit.hpp"

namespace unit_convert
{
  /**
   * A mixin class that adds support for automatically
   * handling SI unit prefixes to all units the registry.
   */
template <typename BASE>
class add_si_prefixes : public BASE
{
 public:
  using base_type = BASE;
  using unit_type = typename base_type::unit_type;
  using dimension_type = typename base_type::dimension_type;
  template<typename T>
  using base_quantity_type = typename base_type::template base_quantity_type<T>;
  template<typename T>
  using registered_quantity_type = registered_quantity<add_si_prefixes, base_quantity_type<T>>;

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

  template<typename T>
  registered_quantity_type<T> make_quantity() const { return registered_quantity_type<T>(*this); }

  template <typename T>
  registered_quantity_type<T> make_quantity(T a_val, const unit_type& a_unit) const
  {
    return registered_quantity_type<T>(*this, base_quantity_type<T>(a_val, a_unit));
  }

  template <typename T>
  registered_quantity_type<T> make_quantity(T a_val, const std::string& a_unit) const
  {
    return this->make_quantity<T>(a_val, this->get_unit(a_unit));
  }


};

}  // namespace unit_convert

