#pragma once

/** @file basic_unit_registry.hpp
 * @author C.D. Clark III
 * @date 03/27/22
 */

#include <iostream>
#include <map>
#include "./basic_quantity.hpp"
#include "./parsers.hpp"
#include "./registered_quantity.hpp"

namespace unit_convert
{
/**
 * @brief A class to store unit definitions.
 */
template <typename UNIT_TYPE, typename VALUE_TYPE = double>
class basic_unit_registry
{
 public:
  using unit_type = UNIT_TYPE;
  using value_type = VALUE_TYPE;
  using dimension_type = typename unit_type::dimension_type;
  using quantity = registered_quantity<basic_unit_registry,
                                       basic_quantity<unit_type, value_type>>;

  enum class EXISTING_UNIT_POLICY { Warn, Throw, Ignore, Overwrite };
  EXISTING_UNIT_POLICY existing_unit_policy = EXISTING_UNIT_POLICY::Throw;

  void add_unit(std::string a_unit_def) {}
  void add_unit(std::string a_symbol, unit_type a_unit)
  {
    auto ptr = m_UnitStore.insert(pair_type{a_symbol, a_unit});
    if (!ptr.second) {  // unit was already in registry
      if (existing_unit_policy == EXISTING_UNIT_POLICY::Throw) {
        throw std::runtime_error("ERROR: unit '" + a_symbol +
                                 "' already exists in the unit registry.");
      }
      if (existing_unit_policy == EXISTING_UNIT_POLICY::Warn) {
        std::cerr << "Warning: '" + a_symbol +
                         "' was already in the unit registry and has been "
                         "overwritten.\n";
      }
      if (existing_unit_policy == EXISTING_UNIT_POLICY::Ignore) {
      }
      if (existing_unit_policy == EXISTING_UNIT_POLICY::Overwrite) {
        m_UnitStore[a_symbol] = a_unit;
      }
    }
  }
  void add_unit(std::string a_symbol, dimension_type a_dim)
  {
    unit_type unit(std::move(a_dim));
    this->add_unit(std::move(a_symbol), std::move(unit));
  }

  size_t size() const { return m_UnitStore.size(); }

  /**
   * Retrieves a unit from the store given its name/symbol.
   *
   * @param a_symbol the unit's name as stored in the registry
   *
   * Throws an exception if the unit is not found.
   */
  const unit_type& get_unit(std::string a_symbol) const
  {
    try {
      return this->m_UnitStore.at(a_symbol);
    } catch (...) {
        throw std::runtime_error("Key Error: Could not find symbol "+a_symbol+" in the unit registry.");
    }
  }
  const unit_type& operator[](std::string a_symbol) const
  {
    return this->get_unit(a_symbol);
  }

  quantity make_quantity() const { return quantity(*this); }

  template <typename T>
  quantity make_quantity(T a_val, const unit_type& a_unit) const
  {
    return quantity(*this, typename quantity::base_type(a_val, a_unit));
  }

  template <typename T>
  quantity make_quantity(T a_val, const std::string& a_unit) const
  {
    return this->make_quantity(a_val, this->get_unit(a_unit));
  }

  basic_unit_registry() = default;

 protected:
  using pair_type = std::pair<std::string, unit_type>;
  using store_type = std::map<std::string, unit_type>;

  store_type m_UnitStore;
};
}  // namespace unit_convert
