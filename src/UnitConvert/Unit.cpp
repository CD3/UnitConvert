#include "./Unit.hpp"

  std::ostream&
  operator<<(std::ostream& out, const Unit& unit)
  {
    out << unit.m_Scale << " ";
    out << unit.m_Dimension << " ";
    if (unit.m_Offset) out << " + " << unit.m_Offset.get();
    return out;
  }



Unit::Unit(double s, const Dimension& d) : m_Scale(s), m_Dimension(d) {}
Unit::Unit(double s, double o, const Dimension& d)
    : m_Scale(s), m_Offset(o), m_Dimension(d)
{
}

const double&    Unit::scale() const { return m_Scale; }
const Dimension& Unit::dimension() const { return m_Dimension; }
const double     Unit::offset() const
{
  if (m_Offset)
    return m_Offset.get();
  else
    return 0;
}
bool Unit::is_offset() const { return static_cast<bool>(m_Offset); }

Unit Unit::operator*(const Unit& other) const
{
  if (this->is_offset() || other.is_offset())
    throw std::runtime_error(
        "Error: cannot multiply offset units by another unit. You should use "
        "a delta unit.");
  return Unit(this->m_Scale * other.m_Scale,
              this->m_Dimension * other.m_Dimension);
}

Unit Unit::operator*(const double& scale) const
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


  Unit&
Unit::operator*=(const Unit& other)
  {
    if (this->is_offset() || other.is_offset())
      throw std::runtime_error(
          "Error: cannot multiply offset units by another unit. You should use "
          "a delta unit.");
    this->m_Scale *= other.m_Scale;
    this->m_Dimension *= other.m_Dimension;
    return *this;
  }


  Unit&
Unit::operator*=(const double& scale)
  {
    this->m_Scale *= scale;

    if (this->is_offset()) this->m_Offset = this->m_Offset.get() / scale;

    return *this;
  }


  Unit
  Unit::operator/(const Unit& other) const
  {
    if (this->is_offset() || other.is_offset())
      throw std::runtime_error(
          "Error: cannot divide offset units by another unit. You should use a "
          "delta unit.");
    return Unit(this->m_Scale / other.m_Scale,
                this->m_Dimension / other.m_Dimension);
  }



  Unit
Unit::operator/(const double& scale) const
  {
    return (*this) * (1. / scale);
  }
  Unit&
Unit::operator/=(const Unit& other)
  {
    if (this->is_offset() || other.is_offset())
      throw std::runtime_error(
          "Error: cannot divide offset units by another unit. You should use a "
          "delta unit.");
    this->m_Scale /= other.m_Scale;
    this->m_Dimension /= other.m_Dimension;
  }
  Unit&
Unit::operator/=(const double& scale)
  {
    this->m_Scale /= scale;

    if (this->is_offset()) this->m_Offset = this->m_Offset.get() * scale;

    return *this;
  }


  Unit
Unit::operator+(const double& offset) const
  {
    // offset specifies the value that must be subtraced
    // to get an absolute unit.
    return Unit(this->m_Scale, this->offset() + offset, this->m_Dimension);
  }

  Unit
  Unit::operator-(const double& offset) const
  {
    return *this + (-offset);
  }
