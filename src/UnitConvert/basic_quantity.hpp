#pragma once

#include <sstream>

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

  basic_quantity to(const unit_type& a_unit) const
  {
    if (this->m_Unit.dimension() != a_unit.dimension()) {
      std::stringstream msg;
      msg << "Dimension Error: Cannot convert from"
          << this->m_Unit.dimension()
          << " to "
          << a_unit.dimension();
      throw std::runtime_error( msg.str() );
    }
    // see writup in doc directory...
    value_type value = (this->m_Unit.scale()*this->m_Value + this->m_Unit.offset() - a_unit.offset())/a_unit.scale();
    return {value, a_unit};
  }

  basic_quantity operator+(const basic_quantity& a_other) const
  {
    if (this->m_Unit.dimension() != a_other.m_Unit.dimension()) {
      std::stringstream msg;
      msg << "Dimension Error: Cannot add quantities with different dimensions. Attempted "
          << this->m_Unit.dimension()
          << " + "
          << a_other.m_Unit.dimension();

      throw std::runtime_error( msg.str() );
    }

    basic_quantity ret = *this;
    ret.m_Value += a_other.to(ret.m_Unit).value();
    return ret;
  }


  basic_quantity operator-(const basic_quantity& a_other) const
  {
    if (this->m_Unit.dimension() != a_other.m_Unit.dimension()) {
      std::stringstream msg;
      msg << "Dimension Error: Cannot add quantities with different dimensions. Attempted "
          << this->m_Unit.dimension()
          << " - "
          << a_other.m_Unit.dimension();

      throw std::runtime_error( msg.str() );
    }

    basic_quantity ret = *this;
    ret.m_Value -= a_other.to(ret.m_Unit).value();
    return ret;
  }


 protected:
  unit_type m_Unit;
  value_type m_Value;
};

template<typename T, typename U>
basic_quantity<U,T> make_quantity(T a_val, U a_unit)
{
  return {std::move(a_val),std::move(a_unit)};
}

}  // namespace unit_convert
