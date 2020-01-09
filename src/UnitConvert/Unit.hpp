#ifndef UnitConvert_Unit_hpp
#define UnitConvert_Unit_hpp

/** @file Unit.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 09/03/18
  */

#include <boost/optional.hpp>

#include "./Dimension.hpp"

namespace UnitConvert {
/**
 * A class representing a unit. A unit has a dimension, a scale, and possibly an
 * offset.
 */
class Unit
{
 protected:
  Dimension m_Dimension;
  double m_Scale;
  boost::optional<double> m_Offset;

 public:

  Unit():m_Dimension(BaseDimension<Dimension::Name::Dimensionless>()),m_Scale(1){}
  Unit(double s, const Dimension& d);
  Unit(double s, double o, const Dimension& d);


  /**
   * Return the unit's scale.
   * This is the "size" of the unit compared to the unit consisting of base
   * units only.
   */
  const double&
  scale() const;

  /**
   * Return the dimension of this unit.
   */
  const Dimension&
  dimension() const;

  /**
   * Reuturn the offset for this dimension.
   *
   * The offset is always expressed in the scale of the unit. It is the
   * value that must be subtracted to make the unit an absolute unit.
   */
  const double
  offset() const;

  /**
   * Return true if this is an offset unit
   */
  bool
  is_offset() const;

  /**
   * Output operator for the unit.
   */
  friend std::ostream&
  operator<<(std::ostream& out, const Unit& unit);

  /**
   * Return the unit resulting from multiplying this unit by another.
   */
  Unit operator*(const Unit& other) const;

  /**
   * Return the unit resulting from multiplying this unit by a scale.
   */
  Unit operator*(const double& scale) const;

  /**
   * Free function version of operator* to support scaling factor
   * in front of units.
   */
  friend Unit operator*(const double& scale, const Unit& other)
  {
    return other * scale;
  }

  /**
   * Multiplying this unit by another.
   */
  Unit&
  operator*=(const Unit& other);

  /**
   * Scale this unit
   */
  Unit&
  operator*=(const double& scale);

  /**
   * Return the unit resulting from dividing this unit by another.
   */
  Unit
  operator/(const Unit& other) const;

  /**
   * Return the unit resulting from dividing this unit by a scale factor.
   */
  Unit
  operator/(const double& scale) const;

  Unit&
  operator/=(const Unit& other);

  Unit&
  operator/=(const double& scale);

  /**
   * Return the unit resulting from adding an offset to this unit.
   *
   * The resulting unit will have the same scale.
   */
  Unit
  operator+(const double& offset) const;

  Unit&
  operator+=(const double& offset);

  /**
   * Return the unit resulting from subtracting an offset to this unit.
   *
   * The resulting unit will have the same scale.
   */
  Unit
  operator-(const double& offset) const;

  Unit&
  operator-=(const double& offset);
};

/**
 * A class for constructing base units. Base units are units for one of the base
 * dimensions
 * that have a scale of 1 and a zero offset.
 */
template <Dimension::Name DIM>
class BaseUnit : public Unit
{
 public:
  BaseUnit() : Unit(1., BaseDimension<DIM>{}) {}
};

}


#endif // include protector
