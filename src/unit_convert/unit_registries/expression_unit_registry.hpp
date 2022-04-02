#pragma once

/** @file expression_unit_registry.hpp
 * @author C.D. Clark III
 * @date 04/02/22
 */
#include "./parsers.hpp"
#include "./unit_registry_base.hpp"

namespace unit_convert
{

template <typename UNIT_TYPE, typename KEY_TYPE = std::string>
class expression_unit_registry : public unit_registry_base<UNIT_TYPE, KEY_TYPE>
{
 public:
  using this_type = expression_unit_registry<UNIT_TYPE,KEY_TYPE>;
  using base_type = unit_registry_base<UNIT_TYPE>;
  using unit_type = typename base_type::unit_type;
  using key_type = typename base_type::key_type;
  using dimension_type = typename base_type::dimension_type;
  template <typename T>
  using base_quantity_type = typename base_type::template base_quantity_type<T>;
  template <typename T>
  using registered_quantity_type =
      typename base_type::template registered_quantity_type<T>;

  expression_unit_registry() : m_si_prefix_parser(), m_expression_parser(*this)
  {
  }

  unit_type get_single_unit(const key_type& a_key) const
  {
    return this->base_type::get_unit(a_key);
  }

  unit_type get_unit(const key_type& a_unit_expr) const
  {
    unit_type ret_unit;
    auto ptr = this->m_UnitStore.find(a_unit_expr);
    if (ptr != this->m_UnitStore.end()) {  // unit was already in registry
      ret_unit = ptr->second;
    } else {
      key_type unit_expr(a_unit_expr);
      auto it = unit_expr.begin();
      bool sucess = qi::parse(it, unit_expr.end(), m_expression_parser, ret_unit);
      if (!sucess or it != unit_expr.end()) {
        throw std::runtime_error("Parsing Error: Cannot parse unit string '" +
                                 a_unit_expr + "'.");
      }
    }
    return ret_unit;
  }

 protected:
  using expression_parser_type = unit_expression_parser<this_type>;
  si_prefix_parser m_si_prefix_parser;
  expression_parser_type m_expression_parser;
};

}  // namespace unit_convert
