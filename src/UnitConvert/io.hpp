#pragma once

/** @file io.hpp
  * @author C.D. Clark III
  * @date 03/28/22
  */

namespace unit_convert
{
  template<size_t, typename>
  class basic_dimension;
  template<typename, typename>
  class basic_unit;

  template<size_t SIZE, typename EXPONENT_TYPE>
  std::ostream& operator<<(std::ostream& out, const basic_dimension<SIZE, EXPONENT_TYPE>& dim)
  {
    for (size_t i = 0; i < SIZE; ++i) {
      if(i > 0)
        out << " ";
      out << "[" << static_cast<char>(i+65) << "]^" << dim.powers()[i];
    }
    return out;
  }

  template<typename DIMENSION_TYPE, typename NUMERIC_TYPE>
  std::ostream& operator<<(
      std::ostream& out, const basic_unit<DIMENSION_TYPE, NUMERIC_TYPE>& unit)
  {
    out << unit.scale() << " ";
    out << unit.dimension();
    if (unit.is_offset()) out << " + " << unit.offset();
    return out;
  }


}

