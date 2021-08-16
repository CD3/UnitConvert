#include "./Dimension.hpp"
#include <algorithm>

namespace UnitConvert {

std::ostream& operator<<(std::ostream& out, const Dimension& dim)
{
  out << " L^" << dim.m_Powers[(int)Dimension::Name::Length];
  out << " M^" << dim.m_Powers[(int)Dimension::Name::Mass];
  out << " T^" << dim.m_Powers[(int)Dimension::Name::Time];
  out << " I^" << dim.m_Powers[(int)Dimension::Name::ElectricalCurrent];
  out << " THETA^" << dim.m_Powers[(int)Dimension::Name::Temperature];
  out << " N^" << dim.m_Powers[(int)Dimension::Name::Amount];
  out << " J^" << dim.m_Powers[(int)Dimension::Name::LuminousIntensity];
  return out;
}

Dimension::Dimension()
{
  m_Powers.fill(0);
}

Dimension::Dimension(Name dim)
{
  m_Powers.fill(0);
  m_Powers[int(dim)] = 1;
}

int Dimension::operator[](Name dim) const
{
  return m_Powers[static_cast<int>(dim)];
}

Dimension::ArrayType Dimension::powers() const { return m_Powers; }

Dimension Dimension::operator*(const Dimension& other) const
{
  Dimension result = *this;
  return result *= other;
}

Dimension& Dimension::operator*=(const Dimension& other)
{
  // no raw loops...
  std::transform(this->m_Powers.begin(), this->m_Powers.end(), other.m_Powers.begin(), this->m_Powers.begin(), std::plus<int>{});
  return *this;
}

Dimension Dimension::operator/(const Dimension& other) const
{
  Dimension result = *this;
  return result /= other;
}
Dimension& Dimension::operator/=(const Dimension& other)
{
  std::transform(this->m_Powers.begin(), this->m_Powers.end(), other.m_Powers.begin(), this->m_Powers.begin(), std::minus<int>{});
  return *this;
}

bool Dimension::operator==(const Dimension& other) const
{
  return m_Powers == other.m_Powers;
}

bool Dimension::operator!=(const Dimension& other) const
{
  return !(*this == other);
}
}
