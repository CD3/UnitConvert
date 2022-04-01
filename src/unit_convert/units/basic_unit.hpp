#pragma once

/** @file basic_unit.hpp
 * @brief
 * @author C.D. Clark III
 * @date 03/24/22
 */

namespace unit_convert
{
/**
 * @brief A simple class representing a physical unit
 *
 * @tparam DIMENSION_TYPE The type used to store the unit's dimension.
 * @tparam NUMERIC_TYPE The type used to store the unit's scale and offset.
 *
 * A physical unit is a physical dimension with a scale attached. Base units
 * have a base dimension and a scale of 1. Derived units may have a derived
 * dimension, or a non-unity scale.
 *
 * Optionally, units may have an offest. The offset specifies how
 * far the unit's "zero" is from absolute zero, expressed in **base units**.
 *
 * For example, using Kelvin as the base unit, Celsius has an offest of +273.15,
 * because 0 C is 273.15 K. Fahrenheight has an offset of 255.37, because 0 F is
 * 255.37 K.
 *
 * Physicsl units can be multiplied and devided, so the `operator*(...)` and
 * `operator/(...)` functions are implemented. The compound assignment versions,
 * `operator*=(...)` and `operator/=(...)` are also implemented. These turn out
 * to be useful when building up dimensions while parsing a string representing
 * some unit or dimension relationship.
 *
 * Addition and subraction operators, `operator+(...)`, `operator+=(...)`,
 * `operator-(...)`, `operator-=(...)`, are also provided for the numeric_type
 * (i.e. two units cannot be added together). These operators adjust the offset
 * of a unit and are necessary to allow units to be easily defined in terms of
 * other units. For example, this works:
 *
 * auto fahrenheit = (9./5) * celsius + 32;
 *
 *
 * Offset units are tricky, and must be handled carefully. For example, offset
 * units cannot be multiplied together (2 C * 2 C is not 4 C^2). However,
 * we *can* multily an offset unit by a pure number, that is what makes the
 * definition of fahrenheit above possible.  When an offset unit is converted
 * to another offset unit, both the scale *and* the offset must be converted.
 *
 * This class takes care to handle all of this correctly so that the user does
 * not have to worry about it in most cases. However, if you try to perform
 * an operation that is not valid with an offset unit, a std::runtime_error will
 * be thrown. The exceptions message _should_ specify what when wrong.
 */
template <typename DIMENSION_TYPE, typename NUMERIC_TYPE = double>
class basic_unit
{
 public:
  using dimension_type = DIMENSION_TYPE;
  using numeric_type = NUMERIC_TYPE;

  basic_unit() : m_Dimension(), m_Scale(1) {}
  basic_unit(const dimension_type& d) : m_Dimension(d), m_Scale(1){};
  basic_unit(numeric_type s, const dimension_type& d)
      : m_Dimension(d), m_Scale(s){};
  basic_unit(numeric_type s, numeric_type o, const dimension_type& d)
      : m_Dimension(d), m_Scale(s), m_Offset(o)
  {
  }

  dimension_type dimension() const { return m_Dimension; }
  numeric_type scale() const { return m_Scale; }
  numeric_type offset() const
  {
    if (m_Offset)
      return m_Offset.value();
    else
      return 0;
  }
  bool is_offset() const { return static_cast<bool>(m_Offset); }

  bool is_base() const { return m_Scale == 1 && m_Dimension.is_base(); }

  bool has_same_dimensions_as(const basic_unit& other)
  {
    return m_Dimension == other.m_Dimension;
  }

  bool has_same_dimensions_as(const dimension_type& other)
  {
    return m_Dimension == other;
  }

  basic_unit& operator*=(const basic_unit& other)
  {
    if (this->is_offset() || other.is_offset())
      throw std::runtime_error(
          "Error: cannot multiply offset units by another unit. You should use "
          "a delta unit.");
    this->m_Scale *= other.m_Scale;
    this->m_Dimension *= other.m_Dimension;

    return *this;
  }

  basic_unit& operator*=(numeric_type scale)
  {
    this->m_Scale *= scale;
    return *this;
  }

  basic_unit operator*(const basic_unit& other) const
  {
    auto result = *this;
    return result *= other;
  }
  basic_unit operator*(numeric_type scale) const
  {
    auto result = *this;
    return result *= scale;
  }
  friend basic_unit operator*(numeric_type scale, const basic_unit& other)
  {
    return other * scale;
  }

  basic_unit& operator/=(const basic_unit& other)
  {
    if (this->is_offset() || other.is_offset())
      throw std::runtime_error(
          "Error: cannot divide offset units by another unit. You should use a "
          "delta unit.");
    this->m_Scale /= other.m_Scale;
    this->m_Dimension /= other.m_Dimension;
    return *this;
  }

  basic_unit& operator/=(numeric_type scale)
  {
    this->m_Scale /= scale;
    return *this;
  }

  basic_unit operator/(const basic_unit& other) const
  {
    auto result = *this;
    return result /= other;
  }

  basic_unit operator/(numeric_type scale) const
  {
    auto result = *this;
    return result /= scale;
  }

  basic_unit& operator+=(numeric_type offset)
  {
    // offset specifies the value that must be *subtraced*
    // to get an absolute unit.
    if (!this->m_Offset) this->m_Offset = 0;
    this->m_Offset.value() += offset;
    return *this;
  }

  basic_unit operator+(numeric_type offset) const
  {
    auto result = *this;
    return result += offset;
  }

  basic_unit& operator-=(numeric_type offset)
  {
    if (!this->m_Offset) this->m_Offset = 0;
    this->m_Offset.value() -= offset;
    return *this;
  }

  basic_unit operator-(numeric_type offset) const
  {
    auto result = *this;
    return result -= offset;
  }

  basic_unit& operator^=(int a_power)
  {
    auto b = *this;
    if (a_power > 0) {
      for (int i = 0; i < abs(a_power) - 1; i++) {
        *this *= b;
      }
    } else {
      for (int i = 0; i < abs(a_power) + 1; i++) {
        *this /= b;
      }
    }

    return *this;
  }

  basic_unit operator^(int power) const
  {
    auto r = *this;
    return r ^= power;
  }

  bool operator==(const basic_unit& a_other) const
  {
    return (this->m_Dimension == a_other.m_Dimension) &&
           (this->m_Scale == a_other.m_Scale) &&
           (this->m_Offset == a_other.m_Offset);
  }
  bool operator!=(const basic_unit& a_other) const
  {
    return !(*this == a_other);
  }

 protected:
  dimension_type m_Dimension;
  numeric_type m_Scale;
  std::optional<numeric_type> m_Offset;
};
}  // namespace unit_convert
