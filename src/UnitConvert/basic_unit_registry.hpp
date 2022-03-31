#pragma once

/** @file basic_unit_registry.hpp
 * @author C.D. Clark III
 * @date 03/27/22
 */

#include "./unit_registry_base.hpp"


namespace unit_convert
{
/**
 * @brief A class to store unit definitions.
 */
template <typename UNIT_TYPE>
class basic_unit_registry: public unit_registry_base<UNIT_TYPE>
{
 public:
  using base_type = unit_registry_base<UNIT_TYPE>;
  using unit_type = typename base_type::unit_type;
  using dimension_type = typename base_type::dimension_type;
  template<typename T>
  using base_quantity_type = typename base_type::template base_quantity_type<T>;
  template<typename T>
  using registered_quantity_type = typename base_type::template registered_quantity_type<T>;
};
}  // namespace unit_convert
