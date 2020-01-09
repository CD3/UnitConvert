#ifndef UnitConvert_Dimension_hpp
#define UnitConvert_Dimension_hpp

/** @file Dimension.hpp
 * @brief
 * @author C.D. Clark III
 * @date 09/02/18
 */

#include <array>
#include <ostream>

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

namespace UnitConvert {
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
class Dimension
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
    Dimensionless  // WARNING: This needs to be the last entry so that the two
                   // lines below work.
  };
  static const int           N = static_cast<int>(Name::Dimensionless) + 1;
  typedef std::array<int, N> ArrayType;

 protected:
  ArrayType m_Powers;

 public:
 public:
  Dimension();
  Dimension(Name dim);

  int operator[](Name dim)
      const;  //<<< Return the power of a base dimension for this dimension
  ArrayType powers() const;  //<<< Return the powers for each base dimension for
                             // this dimension in an array

  friend std::ostream& operator<<(std::ostream&    out,
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
template<Dimension::Name DIM>
class BaseDimension : public Dimension
{
 public:
  BaseDimension() : Dimension(DIM) {}
};

/**
 * Specialization for dimensionless base dimension.
 */
template<>
class BaseDimension<Dimension::Name::Dimensionless> : public Dimension
{
 public:
  BaseDimension() : Dimension() {}
};

/**
 * A template meta-function for getting the Dimension::Name value
 * corresponding to a Boost.Units dimensino
 */
template<typename D>
struct get_dimension_name {
};

template<>
struct get_dimension_name<boost::units::length_dimension> {
  static const Dimension::Name value = Dimension::Name::Length;
};
template<>
struct get_dimension_name<boost::units::mass_dimension> {
  static const Dimension::Name value = Dimension::Name::Mass;
};
template<>
struct get_dimension_name<boost::units::time_dimension> {
  static const Dimension::Name value = Dimension::Name::Time;
};
template<>
struct get_dimension_name<boost::units::current_dimension> {
  static const Dimension::Name value = Dimension::Name::ElectricalCurrent;
};
template<>
struct get_dimension_name<boost::units::temperature_dimension> {
  static const Dimension::Name value = Dimension::Name::Temperature;
};
template<>
struct get_dimension_name<boost::units::amount_dimension> {
  static const Dimension::Name value = Dimension::Name::Amount;
};
template<>
struct get_dimension_name<boost::units::luminous_intensity_dimension> {
  static const Dimension::Name value = Dimension::Name::LuminousIntensity;
};

// boost defines separate dimensions for plane angles and solid angles.
// in the SI system, these are dimensionless.
template<>
struct get_dimension_name<boost::units::plane_angle_dimension> {
  static const Dimension::Name value = Dimension::Name::Dimensionless;
};
template<>
struct get_dimension_name<boost::units::solid_angle_dimension> {
  static const Dimension::Name value = Dimension::Name::Dimensionless;
};
template<>
struct get_dimension_name<boost::units::dimensionless_type> {
  static const Dimension::Name value = Dimension::Name::Dimensionless;
};

/**
 * Meta-function to get the Dimension::Name for a boost unit
 */
template<class D, class S>
struct get_dimension_name<boost::units::unit<D, S>> {
  static const Dimension::Name value = get_dimension_name<D>::value;
};

template<class D, class S>
struct get_dimension_name<boost::units::absolute<boost::units::unit<D, S>>> {
  static const Dimension::Name value = get_dimension_name<D>::value;
};
}

#endif  // include protector
