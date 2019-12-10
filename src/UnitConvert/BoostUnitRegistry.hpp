#ifndef UnitConvert_BoostUnitRegistry_hpp
#define UnitConvert_BoostUnitRegistry_hpp

/** @file BoostUnitRegistry.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 09/07/18
  */

#include "./UnitRegistry.hpp"


namespace UnitConvert {
namespace detail {

// function to add the base units of a Boost.Units system
// to a unit registry.
template <class SYSTEM, class DIM_LIST = void>
struct addSystemBaseUnits {
  static void
  apply(UnitRegistry* ureg)
  {
    typedef typename DIM_LIST::item::dimension_type DIMENSION;
    typedef boost::units::unit<DIMENSION, SYSTEM> BOOST_UNIT;
    // create unit and add it to the registry
    ureg->addBaseUnit< get_dimension_name<DIMENSION>::value >( str(BOOST_UNIT{}) );

    addSystemBaseUnits<SYSTEM, typename DIM_LIST::next>::apply(ureg);
  }
};


template <class SYSTEM>
struct addSystemBaseUnits<SYSTEM,void> {
  static void
  apply(UnitRegistry* ureg)
  {
    addSystemBaseUnits<SYSTEM, typename SYSTEM::type>::apply(ureg);
  }
};

template <class SYSTEM>
struct addSystemBaseUnits<SYSTEM,boost::units::dimensionless_type> {
  static void
  apply(UnitRegistry* ureg)
  {
  }
};

}
}

/**
 * A unit registry that integrates with Boost.Units. The template parameter SYSTEM
 * specifies a Boost.Units system that will be use to automatically load
 * base units into the registry.
 */
template <class SYSTEM>
class BoostUnitRegistry : public UnitRegistry
{
 public:
  BoostUnitRegistry()
  {
    UnitConvert::detail::addSystemBaseUnits<SYSTEM>::apply(this);
  }

  using UnitRegistry::addUnit;
  // add unit to registry using boost quantity instance
  template <class D, class S>
  void
  addUnit(const boost::units::unit<D, S>& u)
  {
    // need to store the base unit systems
    // i.e. if we get addBaseUnit(i::m), then we store the system for i::m
    // either that, or we take the system as a template argument.
    typedef boost::units::unit<D, SYSTEM> si_base_unit;
    si_base_unit su;
    this->addUnit(UnitConvert::detail::str(u), boost::units::conversion_factor(u, su) *
                              this->makeUnit(
                                  UnitConvert::detail::str(su, boost::units::format_mode::raw_fmt)));
  }
};


#endif // include protector
