#ifndef UnitConvert_UnitRegistry_hpp
#define UnitConvert_UnitRegistry_hpp

/** @file UnitRegistry.hpp
 * @brief
 * @author C.D. Clark III
 * @date 09/03/18
 */

#include <iostream>
#include <map>

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
namespace spt = boost::spirit;
namespace qi  = boost::spirit::qi;
namespace phx = boost::phoenix;

#include "./Unit.hpp"
#include "./detail/functions.hpp"

template<typename T>
class Quantity;

/**
 * A class for storing units according to their string representation.
 */
class UnitRegistry
{
 public:
  typedef std::pair<std::string, Unit> PairType;
  typedef std::map<std::string, Unit>  StoreType;

 protected:
  StoreType m_UnitStore;

 public:
  enum class EXISTING_UNIT_POLICY { Warn, Throw, Ignore };
  EXISTING_UNIT_POLICY existing_unit_policy = EXISTING_UNIT_POLICY::Throw;

  void addUnit(const std::string& k, const Unit& v);

  void addUnit(std::string unit_equation);

  template<Dimension::Name DIM>
  void addBaseUnit(const std::string& k);

  /**
   * Querry the registry for a named unit and return
   * the unit it represents. If the named unit is not
   * found in the registry, an exception is thrown.
   */
  const Unit& getUnit(std::string a_unit) const;

  /**
   * Querry the registry for a named unit and return
   * a copy of the unit it represents. If unit isn't found
   * and a_trySIPrefix is true, then the function
   * checks if unit is an SI prefix'ed version of a unit
   * in the registry. If no unit is
   * found in the registry, an exception is thrown.
   */
  Unit getUnit(std::string a_unit, bool a_trySIPrefix) const;

  /**
   * Parse a string and return the unit it represents.
   *
   * All units in the string must already exist in the unit registry.
   *
   * Example:
   *
   * 'kg m^2 / s^2'
   *
   */
  Unit makeUnit(std::string a_unit) const;

  template<typename T>
  Quantity<T> makeQuantity(const T& val, const std::string& a_unit) const;

  /** Output operator.
   */
  friend std::ostream& operator<<(std::ostream& out, const UnitRegistry& reg);

  /*
   * A default constructable unit
   *
   * Used for parsing unit strings.
   */
  class DUnit : public Unit
  {
   public:
    DUnit() : Unit(1, BaseDimension<Dimension::Name::Dimensionless>()) {}
    DUnit(const Unit& u) : Unit(u) {}
  };

  /**
   * A Boost.Spirit grammar for parsing unit strings.
   */
  struct UnitParser : spt::qi::grammar<std::string::iterator, DUnit()> {
    using Iterator = std::string::iterator;
    using ThisType = UnitParser;

    qi::rule<Iterator, DUnit()> named_unit, factor, term, group, scale,
        expression, unit;
    qi::rule<Iterator, double()> offset;
    qi::rule<Iterator, int()>    exponent;
    qi::rule<Iterator>           mul, div, pow, add, sub;
    qi::rule<Iterator, char()>   unit_name_chars;

    const UnitRegistry& ureg;

    /**
     * compute a unit raised to an integer power)
     */
    DUnit exponentiate(const DUnit& b, const int e);

    /**
     * retrieve a named unit from the registry
     */
    Unit getUnitFromRegistry(const std::string& unit);

    UnitParser(const UnitRegistry& registry);
  };

  struct SIPrefixParser : qi::symbols<char, int> {
    SIPrefixParser()
    {
      add("Y", 24)("yotta", 24)("Z", 21)("zetta", 21)("E", 18)("exa", 18)(
          "P", 15)("peta", 15)("T", 12)("tera", 12)("G", 9)("giga", 9)("M", 6)(
          "mega", 6)("k", 3)("kilo", 3)("h", 2)("hecto", 2)("da", 1)("deca", 1)(
          "d", -1)("deci", -1)("c", -2)("centi", -2)("m", -3)("milli", -3)(
          "u", -6)("micro", -6)("n", -9)("nano", -9)("p", -12)("pico", -12)(
          "f", -15)("femto", -15)("a", -18)("atto", -18)("z", -21)(
          "zepto", -21)("y", -24);
      ("yocto", -24);
    }
  };

 protected:
  UnitParser m_UnitParser;
  SIPrefixParser                    m_SIPrefixParser;

 public:
  UnitRegistry() : m_UnitParser(*this){};

  const UnitParser& getUnitParser() const
  {
    return m_UnitParser;
  }
};

template<Dimension::Name DIM>
void UnitRegistry::addBaseUnit(const std::string& k)
{
  this->addUnit(k, BaseUnit<DIM>());
}

template<typename T>
::Quantity<T> UnitRegistry::makeQuantity(const T&           val,
                                         const std::string& a_unit) const
{
  return ::Quantity<T>(val, makeUnit(a_unit), this);
}

#endif  // include protector
