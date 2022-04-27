#pragma once

/** @file unit_registry_base.hpp
 * @author C.D. Clark III
 * @date 03/30/22
 */

#include <map>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include "../quantities/basic_quantity.hpp"
#include "../quantities/add_registry.hpp"

namespace unit_convert
{
template <typename UNIT_TYPE, typename KEY_TYPE = std::string>
class unit_registry_base
{
 public:
  using unit_type = UNIT_TYPE;
  using key_type = KEY_TYPE;
  using dimension_type = typename unit_type::dimension_type;
  template <typename T>
  using base_quantity_type = basic_quantity<unit_type, T>;
  template <typename T>
  using registered_quantity_type =
      add_registry<unit_registry_base, base_quantity_type<T>>;

  enum class EXISTING_UNIT_POLICY { Warn, Throw, Ignore, Overwrite };
  EXISTING_UNIT_POLICY existing_unit_policy = EXISTING_UNIT_POLICY::Throw;

  virtual ~unit_registry_base() = default;
  virtual void add_unit(const key_type& a_key, dimension_type a_dimension);
  virtual void add_unit(const key_type& a_key, unit_type a_unit);
  virtual size_t size() const { return m_unit_store.size(); }
  virtual unit_type get_unit(const key_type& a_key) const;

  template <typename T>
  registered_quantity_type<T> make_quantity() const;
  template <typename T>
  registered_quantity_type<T> make_quantity(T a_val,
                                            const key_type& a_key) const;

  std::vector<key_type> get_unit_symbols() const
  {
    std::vector<key_type> keys;

    std::transform(m_unit_store.begin(), m_unit_store.end(), std::back_inserter(keys), [](auto& p)->key_type{return p.first;});
    return keys;
  }

 protected:
  using pair_type = std::pair<key_type, unit_type>;
  using store_type = std::map<key_type, unit_type>;

  store_type m_unit_store;
};

template <typename UNIT_TYPE, typename KEY_TYPE>
void unit_registry_base<UNIT_TYPE, KEY_TYPE>::add_unit(
    const key_type& a_key, dimension_type a_dimension)
{
  unit_type unit(std::move(a_dimension));
  this->add_unit(a_key, std::move(unit));
}

template <typename UNIT_TYPE, typename KEY_TYPE>
void unit_registry_base<UNIT_TYPE, KEY_TYPE>::add_unit(const key_type& a_key,
                                                       unit_type a_unit)
{
  auto ptr = m_unit_store.insert(pair_type{a_key, a_unit});
  if (!ptr.second) {  // unit was already in registry
    if (existing_unit_policy == EXISTING_UNIT_POLICY::Throw) {
      throw std::runtime_error("ERROR: unit '" + a_key +
                               "' already exists in the unit registry.");
    }
    if (existing_unit_policy == EXISTING_UNIT_POLICY::Warn) {
      std::cerr << "Warning: '" + a_key +
                       "' was already in the unit registry and has been "
                       "overwritten.\n";
    }
    if (existing_unit_policy == EXISTING_UNIT_POLICY::Ignore) {
    }
    if (existing_unit_policy == EXISTING_UNIT_POLICY::Overwrite) {
      m_unit_store[a_key] = a_unit;
    }
  }
}

template <typename UNIT_TYPE, typename KEY_TYPE>
typename unit_registry_base<UNIT_TYPE, KEY_TYPE>::unit_type
unit_registry_base<UNIT_TYPE, KEY_TYPE>::get_unit(const key_type& a_key) const
{
  try {
    return this->m_unit_store.at(a_key);
  } catch (...) {
    throw std::runtime_error("Key Error: Could not find symbol " + a_key +
                             " in the unit registry.");
  }
}

template <typename UNIT_TYPE, typename KEY_TYPE>
template <typename T>
unit_registry_base<UNIT_TYPE, KEY_TYPE>::registered_quantity_type<T>
unit_registry_base<UNIT_TYPE, KEY_TYPE>::make_quantity() const
{
  return registered_quantity_type<T>(*this);
}

template <typename UNIT_TYPE, typename KEY_TYPE>
template <typename T>
unit_registry_base<UNIT_TYPE, KEY_TYPE>::registered_quantity_type<T>
unit_registry_base<UNIT_TYPE, KEY_TYPE>::make_quantity(
    T a_val, const key_type& a_key) const
{
  return registered_quantity_type<T>(
      *this, base_quantity_type<T>(a_val, this->get_unit(a_key)));
}

}  // namespace unit_convert
