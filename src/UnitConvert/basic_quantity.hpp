#pragma once

/** @file basic_quantity.hpp
 * @brief
 * @author C.D. Clark III
 * @date 03/24/22
 */

namespace unit_convert
{
/**
 * @brief A simple class representing a physical quantity.
 *
 * @tparam UNIT_TYPE The type used to store the quantity's unit.
 * @tparam VALUE_TYPE The type used to store the quantity's value.
 *
 * A physical quantity is a physical unit with a value attached.
 * The unit of course had a dimension, which we say is the quantities dimension
 * as well.
 */
template <typename UNIT_TYPE, typename VALUE_TYPE = double>
class basic_quantity
{
 public:
  using unit_type = UNIT_TYPE;
  using value_type = VALUE_TYPE;

  value_type value() const { return m_Value; }
  unit_type unit() const { return m_Unit; }

  basic_quantity(value_type v, unit_type u)
      : m_Unit(std::move(u)), m_Value(std::move(v))
  {
  }

  basic_quantity() : m_Unit(), m_Value(0) {}

  basic_quantity to(const unit_type& unit)
  {
    if (this->m_Unit.dimension() != unit.dimension()) {
      throw std::runtime_error(
          "Dimension Error: Cannot convert from " +
          UnitConvert::detail::str(this->m_Unit.dimension()) + " to " +
          UnitConvert::detail::str(unit.dimension()));
    }
    // see writup in doc directory...
    value_type value = (this->m_Unit.scale()*this->m_Value + this->m_Unit.offset() - unit.offset())/unit.scale();
    return {value, unit};
  }

 protected:
  unit_type m_Unit;
  value_type m_Value;
};

}  // namespace unit_convert
