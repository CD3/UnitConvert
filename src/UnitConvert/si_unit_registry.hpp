#pragma once

/** @file si_unit_registry.hpp
 * @author C.D. Clark III
 * @date 03/27/22
 */

#include "./basic_unit_registry.hpp"
#include "./si_unit.hpp"

namespace unit_convert
{
template <typename NUMERIC_TYPE = double>
class si_unit_registry : public basic_unit_registry<si_unit<NUMERIC_TYPE>>
{
  public:

  using base_type = basic_unit_registry<si_unit<NUMERIC_TYPE>>;
  using unit_type = typename base_type::unit_type;
  using dimension_type = typename base_type::dimension_type;
  template<typename T>
  using base_quantity_type = typename base_type::template base_quantity_type<T>;
  template<typename T>
  using registered_quantity_type = registered_quantity<add_si_prefixes<NUMERIC_TYPE>, base_quantity_type<T>>;

  si_unit_registry()
  {
    this->add_unit("m", si_base_dimension<si_dimension::name::Length>());
    this->add_unit("kg", si_base_dimension<si_dimension::name::Mass>());
    this->add_unit("s", si_base_dimension<si_dimension::name::Time>());
    this->add_unit("K", si_base_dimension<si_dimension::name::Temperature>());
    this->add_unit("A", si_base_dimension<si_dimension::name::ElectricalCurrent>());
    this->add_unit("mol", si_base_dimension<si_dimension::name::Amount>());
    this->add_unit("cd", si_base_dimension<si_dimension::name::LuminousIntensity>());
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
