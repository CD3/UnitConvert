#pragma once

/** @file add_registry.hpp
 * @author C.D. Clark III
 * @date 03/27/22
 */

namespace unit_convert
{
template <typename UNIT_REGISTRY_TYPE, typename QUANTITY_TYPE>
class add_registry
    : public QUANTITY_TYPE
{
 public:
  using unit_registry_type = UNIT_REGISTRY_TYPE;
  using quantity_type = QUANTITY_TYPE;
  using base_type = quantity_type;
  using value_type = typename base_type::value_type;
  using unit_type = typename base_type::unit_type;
  using this_type = add_registry;

  const unit_registry_type& m_unit_registry;

  add_registry(const unit_registry_type& a_ureg)
      : base_type(), m_unit_registry(a_ureg)
  {
  }
  add_registry(const unit_registry_type& a_ureg, base_type a_quantity)
      : base_type(std::move(a_quantity)), m_unit_registry(a_ureg)
  {
  }
  // template<typename ...Args>
  // add_registry(const unit_registry_type& a_ureg, Args ...args)
  //     : base_type(std::forwared<Args...>(args)), m_unit_registry(a_ureg)
  // {
  // }

  add_registry to(const unit_type&  a_unit) const
  {
    return add_registry( m_unit_registry, base_type::to(a_unit) );
  }

  add_registry to(std::string a_symbol) const
  {
    return add_registry( m_unit_registry, base_type::to(m_unit_registry.get_unit(a_symbol)) );
  }

  add_registry& operator=(const base_type& a_other)
  {
    // since add_registry "is a" base type, we can use the base type assignment operator.
    static_cast<base_type&>(*this) = a_other;
    return *this;
  }
  add_registry& operator=(const add_registry& a_other)
  {
    // we don't want to (can't) copy the unit registry reference.
    // just copy the underlying quantity type
    static_cast<base_type&>(*this) = static_cast<const base_type&>(a_other);
    return *this;
  }
};

// FIXME: port registered_quantity -> add_registry so this can be removed
template <typename UNIT_REGISTRY_TYPE, typename QUANTITY_TYPE>
using registered_quantity = add_registry<UNIT_REGISTRY_TYPE, QUANTITY_TYPE>;

}  // namespace unit_convert
