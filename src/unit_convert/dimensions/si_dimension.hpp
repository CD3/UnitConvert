#pragma once

/** @file si_dimension.hpp
 * @brief
 * @author C.D. Clark III
 * @date 03/25/22
 */
#include "./basic_dimension.hpp"

namespace unit_convert
{
class si_dimension : public basic_dimension<7>
{
 public:
  using base_type = basic_dimension<7>;
  enum class name {
    Length = 0,
    Mass,
    Time,
    ElectricalCurrent,
    Temperature,
    Amount,
    LuminousIntensity
  };

  si_dimension(name dim) : base_type(static_cast<exponent_type>(dim)) {}

  si_dimension() = default;
  si_dimension(const si_dimension&) = default;
  si_dimension(si_dimension&&) = default;
  si_dimension& operator=(const si_dimension&) = default;
  si_dimension& operator=(si_dimension&&) = default;

};

template <si_dimension::name DIM>
class si_base_dimension : public si_dimension
{
  public:
  si_base_dimension():si_dimension(DIM){}
};

}  // namespace unit_convert
