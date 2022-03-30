#pragma once

/** @file registered_quantity.hpp
 * @author C.D. Clark III
 * @date 03/27/22
 */

#include "./basic_quantity.hpp"

namespace unit_convert
{
template <typename UNIT_REGISTRY_TYPE, typename QUANTITY_TYPE>
class registered_quantity
    : public QUANTITY_TYPE
{
 public:
  using unit_registry_type = UNIT_REGISTRY_TYPE;
  using quantity_type = QUANTITY_TYPE;
  using base_type = quantity_type;
  using value_type = typename base_type::value_type;
  using unit_type = typename base_type::unit_type;
  using this_type = registered_quantity;

  const unit_registry_type& m_unit_registry;

  registered_quantity(const unit_registry_type& a_ureg)
      : base_type(), m_unit_registry(a_ureg)
  {
  }
  registered_quantity(const unit_registry_type& a_ureg, base_type a_quantity)
      : base_type(std::move(a_quantity)), m_unit_registry(a_ureg)
  {
  }
  // template<typename ...Args>
  // registered_quantity(const unit_registry_type& a_ureg, Args ...args)
  //     : base_type(std::forwared<Args...>(args)), m_unit_registry(a_ureg)
  // {
  // }

  registered_quantity to(const unit_type&  a_unit) const
  {
    return registered_quantity( m_unit_registry, base_type::to(a_unit) );
  }

  registered_quantity to(std::string a_symbol) const
  {
    return registered_quantity( m_unit_registry, base_type::to(m_unit_registry.get_unit(a_symbol)) );
  }

  registered_quantity& operator=(const base_type& a_other)
  {
    // since registered_quantity "is a" base type, we can use the base type assignment operator.
    static_cast<base_type&>(*this) = a_other;
    return *this;
  }
  registered_quantity& operator=(const registered_quantity& a_other)
  {
    // we don't want to (can't) copy the unit registry reference.
    // just copy the underlying quantity type
    static_cast<base_type&>(*this) = static_cast<const base_type&>(a_other);
    return *this;
  }
};

}  // namespace unit_convert
