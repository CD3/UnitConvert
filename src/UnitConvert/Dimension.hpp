#ifndef UnitConvert_Dimension_hpp
#define UnitConvert_Dimension_hpp

/** @file Dimension.hpp
 * @brief
 * @author C.D. Clark III
 * @date 09/02/18
 */

#include <algorithm>
#include <cmath>
#include <numeric>
#include <array>
#include <ostream>

namespace UnitConvert
{

namespace v1
{
  /**
   * @brief A class representing a physical dimension.
   */
template <size_t SIZE, typename EXPONENT_TYPE = int>
class Dimension
{
 public:
  static const size_t N = SIZE;
  using exponent_type = EXPONENT_TYPE;
  using array_type = std::array<exponent_type, N>;
  Dimension() { m_Powers.fill(0); }
  Dimension(size_t i)
  {
    m_Powers.fill(0);
    m_Powers[i] = 1;
  }

  Dimension(const Dimension&) = default;
  Dimension(Dimension&&) = default;
  Dimension& operator=(const Dimension&) = default;
  Dimension& operator=(Dimension&&) = default;

  bool is_base() const
  {
    return std::accumulate(m_Powers.begin(), m_Powers.end(), 0, [](exponent_type sum, exponent_type power){ return std::move(sum) + abs(power); }) == 1;
  }
  bool is_derived() const
  {
    return !this->is_base();
  }
  

  int operator[](size_t dim) const { return m_Powers[dim]; }

  Dimension operator*(const Dimension& other) const
  {
    Dimension result = *this;
    return result *= other;
  }

  Dimension& operator*=(const Dimension& other)
  {
    // no raw loops...
    std::transform(this->m_Powers.begin(), this->m_Powers.end(),
                   other.m_Powers.begin(), this->m_Powers.begin(),
                   std::plus<exponent_type>{});
    return *this;
  }

  Dimension operator/(const Dimension& other) const
  {
    Dimension result = *this;
    return result /= other;
  }

  Dimension& operator/=(const Dimension& other)
  {
    std::transform(this->m_Powers.begin(), this->m_Powers.end(),
                   other.m_Powers.begin(), this->m_Powers.begin(),
                   std::minus<exponent_type>{});
    return *this;
  }

  bool operator==(const Dimension& other) const
  {
    return m_Powers == other.m_Powers;
  }
  bool operator!=(const Dimension& other) const
  {
    return m_Powers != other.m_Powers;
  }

  friend std::ostream& operator<<(std::ostream& out,
                                  const Dimension<SIZE, EXPONENT_TYPE>& dim)
  {
    for (size_t i = 0; i < dim.m_Powers.size(); ++i) {
      out << " [" << i << "]^" << dim.m_Powers[i];
    }
    return out;
  }

 protected:
  array_type m_Powers;
};
}  // namespace v1

/**
 * A class representing a physical dimension. A physicsl dimension
 * consists of 0 or more of the seven SI base dimensions
 * (see https://en.wikipedia.org/wiki/SI_base_unit):
 *
 * Length
 * Mass
 * Time
 * Electrical Current
 * Temperature
 * Amount
 * Lumnous Intensity
 *
 * @todo Use expression templates to improve dimension algebra performance
 */
class Dimension : v1::Dimension<8>
{
 public:
  enum class Name {
    Length = 0,
    Mass,
    Time,
    ElectricalCurrent,
    Temperature,
    Amount,
    LuminousIntensity,
    Dimensionless
  };

 public:
  Dimension();
  Dimension(Name dim);

  int operator[](Name dim)
      const;  //<<< Return the power of a base dimension for this dimension
  array_type powers() const;  //<<< Return the powers for each base dimension for
                             // this dimension in an array

  friend std::ostream& operator<<(std::ostream& out,
                                  const Dimension& dim);  //<<< Output operator.

  Dimension operator*(const Dimension& other)
      const;  //<<< Return the dimension resulting in
              // multiplying this dimension with another.

  Dimension& operator*=(const Dimension& other);

  Dimension operator/(
      const Dimension& other) const;  //<<< Return the dimension resulting in
                                      // dividing this dimension by another.

  Dimension& operator/=(const Dimension& other);

  bool operator==(const Dimension& other)
      const;  //<<< Return true if dimensions are the same. i.e. all powers of
              // base dimensions are the same.

  bool operator!=(const Dimension& other)
      const;  //<<< Return true if dimensions are not the same
};

/**
 * A class for constructing base dimensions.
 *
 * The Dimension class cannot constructed directly. This class
 * is used to construct base dimensions, which can then be used
 * to build non-base dimensions.
 */
template <Dimension::Name DIM>
class BaseDimension : public Dimension
{
 public:
  BaseDimension() : Dimension(DIM) {}
};

/**
 * Specialization for dimensionless base dimension.
 */
template <>
class BaseDimension<Dimension::Name::Dimensionless> : public Dimension
{
 public:
  BaseDimension() : Dimension() {}
};

/**
 * A template meta-function for getting the Dimension::Name value
 * corresponding to a Boost.Units dimensino
 */
template <typename D>
struct get_dimension_name {
};

}  // namespace UnitConvert

#include <boost/units/base_dimension.hpp>
#include <boost/units/physical_dimensions/amount.hpp>
#include <boost/units/physical_dimensions/current.hpp>
#include <boost/units/physical_dimensions/length.hpp>
#include <boost/units/physical_dimensions/luminance.hpp>
#include <boost/units/physical_dimensions/mass.hpp>
#include <boost/units/physical_dimensions/plane_angle.hpp>
#include <boost/units/physical_dimensions/solid_angle.hpp>
#include <boost/units/physical_dimensions/temperature.hpp>
#include <boost/units/physical_dimensions/time.hpp>

namespace UnitConvert
{

template <>
struct get_dimension_name<boost::units::length_dimension> {
  static const Dimension::Name value = Dimension::Name::Length;
};
template <>
struct get_dimension_name<boost::units::mass_dimension> {
  static const Dimension::Name value = Dimension::Name::Mass;
};
template <>
struct get_dimension_name<boost::units::time_dimension> {
  static const Dimension::Name value = Dimension::Name::Time;
};
template <>
struct get_dimension_name<boost::units::current_dimension> {
  static const Dimension::Name value = Dimension::Name::ElectricalCurrent;
};
template <>
struct get_dimension_name<boost::units::temperature_dimension> {
  static const Dimension::Name value = Dimension::Name::Temperature;
};
template <>
struct get_dimension_name<boost::units::amount_dimension> {
  static const Dimension::Name value = Dimension::Name::Amount;
};
template <>
struct get_dimension_name<boost::units::luminous_intensity_dimension> {
  static const Dimension::Name value = Dimension::Name::LuminousIntensity;
};

// boost defines separate dimensions for plane angles and solid angles.
// in the SI system, these are dimensionless.
template <>
struct get_dimension_name<boost::units::plane_angle_dimension> {
  static const Dimension::Name value = Dimension::Name::Dimensionless;
};
template <>
struct get_dimension_name<boost::units::solid_angle_dimension> {
  static const Dimension::Name value = Dimension::Name::Dimensionless;
};
template <>
struct get_dimension_name<boost::units::dimensionless_type> {
  static const Dimension::Name value = Dimension::Name::Dimensionless;
};

/**
 * Meta-function to get the Dimension::Name for a boost unit
 */
template <class D, class S>
struct get_dimension_name<boost::units::unit<D, S>> {
  static const Dimension::Name value = get_dimension_name<D>::value;
};

template <class D, class S>
struct get_dimension_name<boost::units::absolute<boost::units::unit<D, S>>> {
  static const Dimension::Name value = get_dimension_name<D>::value;
};
}  // namespace UnitConvert

#endif  // include protector
