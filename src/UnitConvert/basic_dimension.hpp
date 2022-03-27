#pragma once
#include <array>
#include <ostream>
#include <algorithm>
#include <numeric>

/** @file basic_dimension.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 03/24/22
  */


namespace unit_convert {
  /**
   * @brief A simple class representing a physical dimension.
   *
   * @tparam SIZE Number of base dimensions.
   * @tparam EXPONENT_TYPE The type to use for storing base dimension powers.
   * 
   *
   * Physical dimensions are defined by a _system_. Each system defines
   * a set of base dimensions, and all other dimensions (called derived dimensions)
   * are expressed as the product of base dimensions raised to some power.
   *
   * To define a dimension, we only need to know:
   * 1) how many base dimensions are there
   * 2) what power is each based dimension raised to.
   *
   * This class stores an array of base dimension powers. The number of base dimensions
   * is specified as a template argument. Additinally, the type to use for dimension powers
   * can be specified.
   *
   * This allows the user to use a smaller type to build a unit system if they are
   * concerned about memory. i.e.
   *
   * sizeof(basic_dimension<3,long int>) > sizeof(basic_dimension<2,long int>) > sizeof(basic_dimension<2,int>)
   *
   * By default, `int` is used to store powers, which should be sufficient for most uses. However,
   * if extremely large powers are expected (I'm not sure why they would be...), a larger type can be used,
   * or the user can use any smaller type capable of representing the powers they need.
   *
   * Physicsl dimensions can be multiplied and devided, so the `operator*(...)` and `operator/(...)` functions are implemented.
   * The compound assignment versions, `operator*=(...)` and `operator/=(...)` are also implemented. These turn
   * out to be useful when building up dimensions while parsing a string representing some unit or dimension relationship.
   */
template <size_t SIZE, typename EXPONENT_TYPE = int>
class basic_dimension
{
 public:
  static const size_t N = SIZE;
  using exponent_type = EXPONENT_TYPE;
  using array_type = std::array<exponent_type, N>;
  basic_dimension() { m_Powers.fill(0); }
  basic_dimension(size_t i)
  {
    m_Powers.fill(0);
    m_Powers[i] = 1;
  }

  basic_dimension(const basic_dimension&) = default;
  basic_dimension(basic_dimension&&) = default;
  basic_dimension& operator=(const basic_dimension&) = default;
  basic_dimension& operator=(basic_dimension&&) = default;

  bool is_base() const
  {
    auto count = std::accumulate(m_Powers.begin(), m_Powers.end(), 0, [](exponent_type sum, exponent_type power){ return std::move(sum) + abs(power); });
    return count == 1 || count == 0;
  }
  bool is_derived() const
  {
    return !this->is_base();
  }
  bool is_dimensionless() const
  {
    return !std::any_of(m_Powers.begin(), m_Powers.end(), [](exponent_type p){return p != 0; } );
  }
  

  int operator[](size_t dim) const { return m_Powers[dim]; }

  basic_dimension operator*(const basic_dimension& other) const
  {
    basic_dimension result = *this;
    return result *= other;
  }

  basic_dimension& operator*=(const basic_dimension& other)
  {
    // no raw loops...
    std::transform(this->m_Powers.begin(), this->m_Powers.end(),
                   other.m_Powers.begin(), this->m_Powers.begin(),
                   std::plus<exponent_type>{});
    return *this;
  }

  basic_dimension operator/(const basic_dimension& other) const
  {
    basic_dimension result = *this;
    return result /= other;
  }

  basic_dimension& operator/=(const basic_dimension& other)
  {
    std::transform(this->m_Powers.begin(), this->m_Powers.end(),
                   other.m_Powers.begin(), this->m_Powers.begin(),
                   std::minus<exponent_type>{});
    return *this;
  }

  bool operator==(const basic_dimension& other) const
  {
    return m_Powers == other.m_Powers;
  }
  bool operator!=(const basic_dimension& other) const
  {
    return m_Powers != other.m_Powers;
  }

  friend std::ostream& operator<<(std::ostream& out,
                                  const basic_dimension<SIZE, EXPONENT_TYPE>& dim)
  {
    for (size_t i = 0; i < dim.m_Powers.size(); ++i) {
      if(i > 0)
        out << " ";
      out << "[" << static_cast<char>(i+65) << "]^" << dim.m_Powers[i];
    }
    return out;
  }

 protected:
  array_type m_Powers;
};
 
}
