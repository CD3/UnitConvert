#pragma once

/** @file si_unit_registry.hpp
 * @author C.D. Clark III
 * @date 03/27/22
 */

#include "./expression_unit_registry.hpp"
#include "../units/si_unit.hpp"

namespace unit_convert
{
  /**
   * An expression-parsing unit registry for SI units that is pre-loaded with SI base units.
   */
template <typename NUMERIC_TYPE = double>
class si_expression_unit_registry : public expression_unit_registry<si_unit<NUMERIC_TYPE>>
{
  public:

  using base_type = expression_unit_registry<si_unit<NUMERIC_TYPE>>;
  using unit_type = typename base_type::unit_type;
  using dimension_type = typename base_type::dimension_type;
  template<typename T>
  using base_quantity_type = typename base_type::template base_quantity_type<T>;
  template<typename T>
  using registered_quantity_type = registered_quantity<si_expression_unit_registry<NUMERIC_TYPE>, base_quantity_type<T>>;

  si_expression_unit_registry()
  {
    this->add_unit("m", si_base_dimension<si_dimension::name::Length>());
    this->add_unit("kg", si_base_dimension<si_dimension::name::Mass>());
    this->add_unit("g", this->get_unit("kg")/1000);
    this->add_unit("s", si_base_dimension<si_dimension::name::Time>());
    this->add_unit("K", si_base_dimension<si_dimension::name::Temperature>());
    this->add_unit("A", si_base_dimension<si_dimension::name::ElectricalCurrent>());
    this->add_unit("mol", si_base_dimension<si_dimension::name::Amount>());
    this->add_unit("cd", si_base_dimension<si_dimension::name::LuminousIntensity>());
  }


};
}  // namespace unit_convert
