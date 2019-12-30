#ifndef UnitConvert_UnitRegistry_hpp
#define UnitConvert_UnitRegistry_hpp

/** @file UnitRegistry.hpp
 * @brief
 * @author C.D. Clark III
 * @date 09/03/18
 */

#include <iostream>
#include <fstream>
#include <map>

#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
namespace spt = boost::spirit;
namespace qi  = boost::spirit::qi;
namespace phx = boost::phoenix;

#include "./Unit.hpp"
#include "./detail/functions.hpp"

namespace UnitConvert
{

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

  /**
   * Add a base unit for a given dimension to the registry. Base units have a
   * scale of one. This function takes a template argument that specifies the
   * dimension. A base unit for the dimension will be created and added to the
   * registry under the name specified by the string argument.
   */
  template<Dimension::Name DIM>
  void addBaseUnit(const std::string& k);

  /**
   * Add a unit to the registry. This overload takes a string, which is the
   * name/symbol that will be added to the database, and a Unit instance, which
   * contains the scale, offset, and dimension that will be added.
   */
  void addUnit(const std::string& k, const Unit& v);

  /**
   * Add a unit to the registry. This overload will parse a unit equation,
   * which defines some unit in terms of other units already in the registry,
   * and add a unit to the registry. For example:
   *
   * addUnit("J = kg m^2 / s^2")
   *
   * will a unit "J" to the registry by building the unit defined by "kg m^2 /
   * s^2".
   */
  void addUnit(std::string unit_equation);

  /**
   * Load a set of units from a stream by reading lines from the stream and
   * passing them to the addUnit(std::string) function.
   */
  void loadUnits(std::istream& in);

  /**
   * Load a set of units from a text file. The text file should contain lines
   * that could be passed to the addUnit(std::string) function.
   */
  bool loadUnits(std::string filename);

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
   * All units in the string must already exist in the unit registry,
   * but the unit itself does not have to be.
   *
   * Example:
   *
   * 'kg m^2 / s^2'
   *
   * as long as 'g', 'm', and 's' are in the registry, this derived
   * unit can be built and returned.
   */
  Unit makeUnit(std::string a_unit) const;

  /**
   * Create a Quantity from a value and unit string. The string is parsed to
   * create a Unit, which is then used to create a quantity.
   */
  template<typename T>
  Quantity<T> makeQuantity(const T& val, const std::string& a_unit) const;

  /**
   * Create a Quantity from a string. This function first parses the string into
   * a value and unit string and then calls the version above.
   *
   * This function is useful for reading user input into a quantity.
   */
  template<typename T>
  Quantity<T> makeQuantity(std::string a_unit) const;

  /**
   * Symbol table parser to get exponent for all SI prefixes.
   */
  struct SIPrefixParser : qi::symbols<char, int> {
    SIPrefixParser();
  };

  /**
   * A Boost.Spirit grammar for parsing unit strings.
   */
  struct UnitParser : spt::qi::grammar<std::string::iterator, Unit()> {
    using Iterator = std::string::iterator;
    using ThisType = UnitParser;

    qi::rule<Iterator, Unit()> named_unit, factor, term, group, scale,
        expression, unit;
    qi::rule<Iterator, double()> offset;
    qi::rule<Iterator, int()>    exponent;
    qi::rule<Iterator>           mul, div, pow, add, sub;
    qi::rule<Iterator, char()>   unit_name_begin_chars, unit_name_other_chars;

    const UnitRegistry& ureg;

    /**
     * compute a unit raised to an integer power)
     */
    Unit exponentiate(const Unit& b, const int e);

    /**
     * retrieve a named unit from the registry
     */
    Unit getUnitFromRegistry(const std::string& unit)
	{
		return ureg.getUnit(unit, true);
	}

	/**
	* The unit parser setup
	*/
	UnitParser(const UnitRegistry& registry);

  };

  /**
   * Symbol table parser to return base dimension for dimension symbols.
   */
  struct BaseDimensionSymbolParser : qi::symbols<char, Dimension> {
    BaseDimensionSymbolParser();
  };

  /**
   * A Boost.Spirit grammar for parsing dimension strings.
   */
  struct DimensionParser
      : spt::qi::grammar<std::string::iterator, Dimension()> {
    using Iterator = std::string::iterator;
    using ThisType = DimensionParser;

    BaseDimensionSymbolParser       base_dimension_symbol;
    qi::rule<Iterator, Dimension()> factor, term, group, dimension;
    qi::rule<Iterator, int()>       exponent;
    qi::rule<Iterator>              mul, div, pow, add, sub;

    /**
     * compute a dimension raised to an integer power)
     */
    Dimension exponentiate(const Dimension& b, const int e);

	/**
	* The dimension parser setup
	*/
	DimensionParser();
  };

 protected:
  UnitParser     m_UnitParser;
  SIPrefixParser m_SIPrefixParser;
  DimensionParser m_DimensionParser;

 public:
  UnitRegistry() : m_UnitParser(*this){};

  /**
   * Returns a reference to the registry's unit parser.
   */
  const UnitParser& getUnitParser() const { return m_UnitParser; }

  /** Output operator.
  */
  friend std::ostream& operator<<(std::ostream& out, const UnitRegistry& reg);

};

inline std::ostream& operator<<(std::ostream& out, const UnitRegistry& reg)
{
	for (auto& p : reg.m_UnitStore) {
		out << p.first << " -> " << p.second << "\n";
	}
	return out;
}

template<Dimension::Name DIM>
void UnitRegistry::addBaseUnit(const std::string& k)
{
  this->addUnit(k, BaseUnit<DIM>());
}

template<typename T>
UnitConvert::Quantity<T> UnitRegistry::makeQuantity(const T&           val,
                                         const std::string& a_unit) const
{
  return UnitConvert::Quantity<T>(val, makeUnit(a_unit), this);
}

template<typename T>
UnitConvert::Quantity<T> UnitRegistry::makeQuantity(std::string a_quantity) const
{
  double      value;
  std::string unit;
  // parse quantity string
  // a quantity should have a numerical value and a unit that makeUnit will
  // parse
  auto it = a_quantity.begin();
  auto r  = qi::parse(it, a_quantity.end(),
                     -qi::double_ >> qi::as_string[+qi::char_], value, unit);
  return UnitConvert::Quantity<T>(static_cast<T>(value), makeUnit(unit), this);
}

} // end namespace UnitConvert

#endif  // include protector
