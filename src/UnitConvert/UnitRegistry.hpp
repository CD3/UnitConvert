#ifndef UnitConvert_UnitRegistry_hpp
#define UnitConvert_UnitRegistry_hpp

/** @file UnitRegistry.hpp
 * @brief
 * @author C.D. Clark III
 * @date 09/03/18
 */

#include <map>
#include <iostream>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include "./detail/functions.hpp"
#include "./Unit.hpp"


template<typename T>
class Quantity;

/**
 * A class for storing units according to their string representation.
 */
class UnitRegistry
{
  public:
    typedef std::pair<std::string, Unit> PairType;
    typedef std::map<std::string, Unit> StoreType;
    typedef boost::tokenizer<boost::char_separator<char>> unit_string_tokenizer;

  protected:
    StoreType m_UnitStore;

    /**
     * Returns true if character is one of the 20 SI prefixes.
     *
     * Note: there are *two* modifications to the standard SI prefix list.
     *
     * 'D' is used instead of "da" for deca so that a single character can be
     * used. 'u' is used instead of "\mu" so that plain ASCII can be used.
     */
    bool
      isSIPrefix(char p);

    double
      getSIPrefixScale(char p);



  public:
    enum class EXISTING_UNIT_POLICY { Warn, Throw, Ignore };
    EXISTING_UNIT_POLICY existing_unit_policy = EXISTING_UNIT_POLICY::Throw;

    void
      addUnit(const std::string& k, const Unit& v);

    void
      addUnit(const std::string& unit_equation);

    template <Dimension::Name DIM>
      void
      addBaseUnit(const std::string& k);

    const Unit&
      getUnit(const std::string& a_unit);

    /**
     * Parse a string and string and return the unit it represents.
     *
     * All units in the string must already exist in the unit registry.
     *
     * Example:
     *
     * 'kg m^2 / s^2'
     *
     */
    Unit
      makeUnit(const std::string& a_unit);

    template <typename T>
      Quantity<T>
      makeQuantity(const T& val, const std::string& a_unit);


    /** Output operator.
    */
    friend std::ostream&
      operator<<(std::ostream& out, const UnitRegistry& reg);
};

template <Dimension::Name DIM>
  void
UnitRegistry::addBaseUnit(const std::string& k)
{
  this->addUnit(k, BaseUnit<DIM>());
}

template <typename T>
  ::Quantity<T>
UnitRegistry::makeQuantity(const T& val, const std::string& a_unit)
{
  return ::Quantity<T>(val, makeUnit(a_unit), this);
}

#endif // include protector
