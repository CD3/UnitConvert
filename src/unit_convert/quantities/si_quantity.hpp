#pragma once

/** @file si_quantity.hpp
 * @brief
 * @author C.D. Clark III
 * @date 03/26/22
 */

#include "../units/si_unit.hpp"

namespace unit_convert
{
template <typename NUMERIC_TYPE>
class si_quantity : public basic_quantity<si_unit<NUMERIC_TYPE>, NUMERIC_TYPE>
{
 public:
  using base_type = basic_quantity<si_unit<NUMERIC_TYPE>,NUMERIC_TYPE>;
  using unit_type = typename base_type::unit_type;
  using value_type = typename base_type::value_type;

  using base_type::base_type;
  si_quantity(base_type u): base_type(std::move(u)){}
};
}  // namespace unit_convert
