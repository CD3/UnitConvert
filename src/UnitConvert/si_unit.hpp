#pragma once

/** @file si_unit.hpp
 * @brief
 * @author C.D. Clark III
 * @date 03/25/22
 */
#include "./basic_unit.hpp"
#include "./si_dimension.hpp"

namespace unit_convert
{

template <typename NUMERIC_TYPE = double>
class si_unit : public basic_unit<si_dimension, NUMERIC_TYPE>
{
 public:
  using base_type = basic_unit<si_dimension, NUMERIC_TYPE>;
  using dimension_type = typename base_type::dimension_type;
  using numeric_type = typename base_type::numeric_type;

  using base_type::base_type;
  si_unit(base_type u): base_type(std::move(u)){}
  
};

template <si_dimension::name DIM, typename NUMERIC_TYPE>
class si_base_unit : public si_unit<NUMERIC_TYPE>
{
 public:
  using base_type = si_unit<NUMERIC_TYPE>;
  using dimension_type = typename base_type::dimension_type;
  using numeric_type = typename base_type::numeric_type;

  
  si_base_unit(base_type u): base_type(std::move(u)){}

  si_base_unit():base_type(DIM){}
};

}  // namespace unit_convert
