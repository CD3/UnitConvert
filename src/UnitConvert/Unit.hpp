#ifndef UnitConvert_Unit_hpp
#define UnitConvert_Unit_hpp

/** @file Unit.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 09/03/18
  */

#include <boost/optional.hpp>

#include "./Dimension.hpp"

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

  Unit()
	  : m_Dimension(BaseDimension<Dimension::Name::Dimensionless>())
	  , m_Scale(1)
  {
  }

  Unit(double s, const Dimension& d)
	  : m_Scale(s)
	  , m_Dimension(d) 
  {}

  Unit(double s, double o, const Dimension& d)
	  : m_Scale(s)
	  , m_Offset(o)
	  , m_Dimension(d)
  {
  }

  /**
   * Return the unit's scale.
   * This is the "size" of the unit compared to the unit consisting of base
   * units only.
   */
  const double&
  scale() const
  { 
	  return m_Scale; 
  }

  /**
   * Return the dimension of this unit.
   */
  const Dimension&
  dimension() const
  { 
	  return m_Dimension; 
  }

  /**
   * Reuturn the offset for this dimension.
   *
   * The offset is always expressed in the scale of the unit. It is the
   * value that must be subtracted to make the unit an absolute unit.
   */
  const double
  offset() const
  {
	  if (m_Offset)
		  return m_Offset.get();
	  else
		  return 0;
  }

  /**
   * Return true if this is an offset unit
   */
  bool
  is_offset() const
  { 
	  return static_cast<bool>(m_Offset); 
  }

  /**
   * Return the unit resulting from multiplying this unit by another.
   */
  Unit operator*(const Unit& other) const
  {
	  if (this->is_offset() || other.is_offset())
		  throw std::runtime_error(
			  "Error: cannot multiply offset units by another unit. You should use "
			  "a delta unit.");
	  return Unit(this->m_Scale * other.m_Scale,
		  this->m_Dimension * other.m_Dimension);
  }

  /**
   * Return the unit resulting from multiplying this unit by a scale.
   */
  Unit operator*(const double& scale) const
  {
	  // if this is an offset unit, we need update the offset as well
	  // offset should be the number of *this* units that must be subtracted
	  // to get an absolute unit. *not* the number of base units.
	  // so, if the unit scale increases, then the offset should decrease...
	  if (this->is_offset())
		  return Unit(this->m_Scale * scale, this->offset() / scale,
			  this->m_Dimension);
	  else
		  return Unit(this->m_Scale * scale, this->m_Dimension);
  }

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
  operator*=(const Unit& other)
  {
	  if (this->is_offset() || other.is_offset())
		  throw std::runtime_error(
			  "Error: cannot multiply offset units by another unit. You should use "
			  "a delta unit.");
	  this->m_Scale *= other.m_Scale;
	  this->m_Dimension *= other.m_Dimension;
	  return *this;
  }

  /**
   * Scale this unit
   */
  Unit&
  operator*=(const double& scale)
  {
	  this->m_Scale *= scale;

	  if (this->is_offset()) this->m_Offset = this->m_Offset.get() / scale;

	  return *this;
  }

  /**
   * Return the unit resulting from dividing this unit by another.
   */
  Unit
  operator/(const Unit& other) const
  {
	  if (this->is_offset() || other.is_offset())
		  throw std::runtime_error(
			  "Error: cannot divide offset units by another unit. You should use a "
			  "delta unit.");
	  return Unit(this->m_Scale / other.m_Scale,
		  this->m_Dimension / other.m_Dimension);
  }

  /**
   * Return the unit resulting from dividing this unit by a scale factor.
   */
  Unit
  operator/(const double& scale) const
  {
	  return (*this) * (1. / scale);
  }

  Unit&
  operator/=(const Unit& other)
  {
	  if (this->is_offset() || other.is_offset())
		  throw std::runtime_error(
			  "Error: cannot divide offset units by another unit. You should use a "
			  "delta unit.");
	  this->m_Scale /= other.m_Scale;
	  this->m_Dimension /= other.m_Dimension;
	  return *this;
  }

  Unit&
  operator/=(const double& scale)
  {
	  this->m_Scale /= scale;

	  if (this->is_offset()) this->m_Offset = this->m_Offset.get() * scale;

	  return *this;
  }

  /**
   * Return the unit resulting from adding an offset to this unit.
   *
   * The resulting unit will have the same scale.
   */
  Unit
  operator+(const double& offset) const
  {
	  // offset specifies the value that must be subtraced
	  // to get an absolute unit.
	  return Unit(this->m_Scale, this->offset() + offset, this->m_Dimension);
  }

  Unit&
  operator+=(const double& offset)
  {
	  if(!this->m_Offset)
		  this->m_Offset = 0;

	  this->m_Offset.get() += offset;

	  return *this;
  }

  /**
   * Return the unit resulting from subtracting an offset to this unit.
   *
   * The resulting unit will have the same scale.
   */
  Unit
  operator-(const double& offset) const
  {
	  return *this + (-offset);
  }

  Unit&
  operator-=(const double& offset)
  {
	  if(!this->m_Offset)
		  this->m_Offset = 0;

	  this->m_Offset.get() -= offset;

	  return *this;
  }

  /**
  * Output operator for the unit.
  */
  friend std::ostream&
	  operator<<(std::ostream& out, const Unit& unit);

};

inline std::ostream&
operator<<(std::ostream& out, const Unit& unit)
{
	out << unit.m_Scale << " ";
	out << unit.m_Dimension << " ";
	if (unit.m_Offset) out << " + " << unit.m_Offset.get();
	return out;
}

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



#endif // include protector
