#pragma once

/**
 * This macro is used to reduce code duplication in unit registry classes
 */
#define UNIT_CONVERT_ADD_MAKE_QUANTITY_METHODS_FOR_UNIT_REGISTRY \
  template<typename T> \
  registered_quantity_type<T> make_quantity() const { return registered_quantity_type<T>(*this); } \
 \
  template <typename T> \
  registered_quantity_type<T> make_quantity(T a_val, const unit_type& a_unit) const \
  { \
    return registered_quantity_type<T>(*this, base_quantity_type<T>(a_val, a_unit)); \
  } \
 \
  template <typename T> \
  registered_quantity_type<T> make_quantity(T a_val, const std::string& a_unit) const \
  { \
    return this->make_quantity<T>(a_val, this->get_unit(a_unit)); \
  }


