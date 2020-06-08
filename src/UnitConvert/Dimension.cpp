#include "./Dimension.hpp"

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
  for (int i = 0; i < N; ++i) m_Powers[i] = 0;
}

Dimension::Dimension(Name dim)
{
  for (int i = 0; i < N; ++i) m_Powers[i] = 0;

  m_Powers[int(dim)] = 1;
}

int Dimension::operator[](Name dim) const
{
  return m_Powers[static_cast<int>(dim)];
}

Dimension::ArrayType Dimension::powers() const { return m_Powers; }

Dimension Dimension::operator*(const Dimension& other) const
{
  Dimension result;
  for (size_t i = 0; i < result.m_Powers.size(); ++i)
    result.m_Powers[i] = this->m_Powers[i] + other.m_Powers[i];
  return result;
}

Dimension& Dimension::operator*=(const Dimension& other)
{
  for (size_t i = 0; i < this->m_Powers.size(); ++i)
    this->m_Powers[i] += other.m_Powers[i];
  return *this;
}

Dimension Dimension::operator/(const Dimension& other) const
{
  Dimension result;
  for (size_t i = 0; i < result.m_Powers.size(); ++i)
    result.m_Powers[i] = this->m_Powers[i] - other.m_Powers[i];
  return result;
}
Dimension& Dimension::operator/=(const Dimension& other)
{
  for (size_t i = 0; i < this->m_Powers.size(); ++i)
    this->m_Powers[i] -= other.m_Powers[i];
  return *this;
}

bool Dimension::operator==(const Dimension& other) const
{
  for (size_t i = 0; i < this->N; ++i) {
    if (m_Powers[i] != other.m_Powers[i]) return false;
  }
  return true;
}

bool Dimension::operator!=(const Dimension& other) const
{
  return !(*this == other);
}
}
