#ifndef UnitConvert_Quantity_hpp
#define UnitConvert_Quantity_hpp

/** @file Quantity.hpp
 * @brief
 * @author C.D. Clark III
 * @date 09/03/18
 */

#include "./UnitRegistry.hpp"
#include "./detail/functions.hpp"

/** A class for storing quantities. A quantity has a value and a unit.
 *
 * This class also stores a pointer to a unit registry that can be
 * used to provided string-based unit conversions. i.e. unit convert
 * the quantity to "kg*m/s/s".
 */
template<typename T>
class Quantity
{
 protected:
  Unit m_Unit;
  T    m_Value;

  UnitRegistry const* m_UnitRegistry = nullptr;
  friend UnitRegistry;

 public:
  template<typename TT>
  Quantity(TT v, Unit u);

  template<typename TT>
  Quantity(TT v, Unit u, UnitRegistry const* r);

  Quantity();

  const T& value() const;

  const Unit& unit() const;

  UnitRegistry const* const unitRegistry() const;

  template<typename TT>
  friend std::ostream& operator<<(std::ostream& out, const Quantity<TT>& quant);

  Quantity<T> to(const Unit& unit) const;

  Quantity<T> to(const std::string& unit) const;

  template<class Dim, typename System>
  Quantity<T> to(const boost::units::unit<Dim, System>& unit) const;

  template<class UNIT, typename TT = T>
  boost::units::quantity<UNIT, TT> to() const;

  Quantity<T> to_base_units() const;
};

template<typename TT>
std::ostream& operator<<(std::ostream& out, const Quantity<TT>& quant)
{
  out << quant.m_Value << " ";
  out << quant.m_Unit;
  return out;
}

template<typename T>
template<typename TT>
Quantity<T>::Quantity(TT v, Unit u) : m_Unit(u), m_Value(v)
{
}
template<typename T>
Quantity<T>::Quantity() : m_Unit(BaseUnit<Dimension::Name::Dimensionless>())
{
}

template<typename T>
template<typename TT>
Quantity<T>::Quantity(TT v, Unit u, UnitRegistry const* r)
    : m_Unit(u), m_Value(v), m_UnitRegistry(r)
{
}

template<typename T>
UnitRegistry const* const Quantity<T>::unitRegistry() const
{
  return m_UnitRegistry;
}

template<typename T>
const T& Quantity<T>::value() const
{
  return m_Value;
}
template<typename T>
const Unit& Quantity<T>::unit() const
{
  return m_Unit;
}
template<typename T>
Quantity<T> Quantity<T>::to(const Unit& unit) const
{
  if (this->m_Unit.dimension() != unit.dimension()) {
    throw std::runtime_error("Dimension Error: Cannot convert from " +
                             UnitConvert::detail::str(this->m_Unit.dimension()) + " to " +
                             UnitConvert::detail::str(unit.dimension()));
  }
  T value = this->m_Value;
  // if current unit has an offset, we need subtract it before
  // we scale the value
  if (this->m_Unit.is_offset()) value -= this->m_Unit.offset();

  // now scale the value.
  value *= this->m_Unit.scale() / unit.scale();

  // if the new unit has an offset, we need to add it
  if (unit.is_offset()) value += unit.offset();

  return Quantity<T>(value, unit);
}

template<typename T>
Quantity<T> Quantity<T>::to(const std::string& unit) const
{
  if (!this->m_UnitRegistry)
    throw std::runtime_error(
        "ERROR: Cannot convert quantity based on unit string. This "
        "Quantity<T> instance does not have a pointer to a UnitRegistry, "
        "which is required to parse unit strings.");
  return this->to(this->m_UnitRegistry->makeUnit(unit));
}

template<typename T>
template<typename Dim, typename System>
Quantity<T> Quantity<T>::to(const boost::units::unit<Dim, System>& unit) const
{
  return this->to(UnitConvert::detail::str(unit, boost::units::format_mode::raw_fmt));
}

template<typename T>
template<class UNIT, typename TT>
boost::units::quantity<UNIT, TT> Quantity<T>::to() const
{
  return boost::units::quantity<UNIT, TT>::from_value(
      this->to(UnitConvert::detail::str(UNIT{}, boost::units::format_mode::raw_fmt))
          .value());
}

template<typename T>
Quantity<T> Quantity<T>::to_base_units() const
{
  // the unit scale is the number we have to multiply the
  // quantity by to represent it in the base units. so all we
  // need to do is scale the quantity's value by the unit's scale.
  // then set the unit to the current unit divided by the scale.

  T value = this->m_Value;
  // if current unit has an offset, we need subtract it before
  // we scale the value
  if (this->m_Unit.is_offset()) value -= this->m_Unit.offset();

  // now scale the value
  value *= this->m_Unit.scale();

  // base units aren't allowed to have an offset
  // so we need to subract the offset from the current unit if it has one.

  if (this->m_Unit.is_offset())
    return Quantity<T>(
        value, (this->m_Unit - this->m_Unit.offset()) / this->m_Unit.scale());
  else
    return Quantity<T>(value, this->m_Unit / this->m_Unit.scale());
}

#endif  // include protector
