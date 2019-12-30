#include "./Dimension.hpp"

UnitConvert::Dimension::Dimension()
{
	for (int i = 0; i < N; ++i) m_Powers[i] = 0;
}

UnitConvert::Dimension::Dimension(Name dim)
{
	for (int i = 0; i < N; ++i) m_Powers[i] = 0;

	m_Powers[int(dim)] = 1;
}

UnitConvert::Dimension UnitConvert::Dimension::operator*(const Dimension& other) const /*<<< Return the dimension resulting in */ // multiplying this dimension with another.
{
	Dimension result;
	for (int i = 0; i < result.m_Powers.size(); ++i)
		result.m_Powers[i] = this->m_Powers[i] + other.m_Powers[i];
	return result;
}

UnitConvert::Dimension& UnitConvert::Dimension::operator*=(const Dimension& other)
{
	for (int i = 0; i < this->m_Powers.size(); ++i)
		this->m_Powers[i] += other.m_Powers[i];
	return *this;
}

UnitConvert::Dimension UnitConvert::Dimension::operator/(const Dimension& other) const /*<<< Return the dimension resulting in */ // dividing this dimension by another.
{
	Dimension result;
	for (int i = 0; i < result.m_Powers.size(); ++i)
		result.m_Powers[i] = this->m_Powers[i] - other.m_Powers[i];
	return result;
}

UnitConvert::Dimension& UnitConvert::Dimension::operator/=(const Dimension& other)
{
	for (int i = 0; i < this->m_Powers.size(); ++i)
		this->m_Powers[i] -= other.m_Powers[i];
	return *this;
}

bool UnitConvert::Dimension::operator==(const Dimension& other) const /*<<< Return true if dimensions are the same. i.e. all powers of */ // base dimensions are the same.
{
	for (size_t i = 0; i < this->N; ++i) {
		if (m_Powers[i] != other.m_Powers[i]) return false;
	}
	return true;
}

bool UnitConvert::Dimension::operator!=(const Dimension& other) const //<<< Return true if dimensions are not the same
{
	return !(*this == other);
}

std::ostream& UnitConvert::operator<<(std::ostream& out, const Dimension& dim)
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
