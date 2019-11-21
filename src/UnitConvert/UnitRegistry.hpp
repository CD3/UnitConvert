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
  void addUnit(const std::string& k, const Unit& v)
  {
	  auto ptr = m_UnitStore.insert(PairType(k, v));
	  if (!ptr.second) {  // unit was already in registry
		  if (existing_unit_policy == EXISTING_UNIT_POLICY::Throw) {
			  throw std::runtime_error("ERROR: unit '" + k +
				  "' already exists in the unit registry.");
		  }
		  if (existing_unit_policy == EXISTING_UNIT_POLICY::Warn) {
			  std::cerr << "Warning: '" + k +
				  "' was already in the unit registry and has been "
				  "overwritten.\n";
		  }
		  if (existing_unit_policy == EXISTING_UNIT_POLICY::Ignore) {
		  }
	  }
  }

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
  void addUnit(std::string unit_equation)
  {
	  auto it = unit_equation.begin();

	  boost::optional<double> scale;
	  std::string             LHS, RHS;

	  // parse unit equation
	  // LHS should be a new named unit (no derived units) with an optional scale.
	  // RHS can be an arbitrary string that makeUnit will parse
	  // examples: 1 J = 1 kg * m^2 / s^s
	  //           100 cm = 1 m
	  auto space  = qi::lit(" ");
	  auto eq     = qi::lit("=");
	  auto uchars = qi::char_("a-zA-Z_/*+-^");
	  auto r =
		  qi::parse(it, unit_equation.end(),
			  -qi::double_ >> *space >> qi::as_string[+uchars] >> *space >>
			  eq >> *space >> qi::as_string[+qi::char_] >> *space,
			  scale, LHS, RHS);
	  if (r) {
		  if (!scale) scale = 1;
		  addUnit(LHS, makeUnit(RHS) / scale.get());
	  } else {
		  throw std::runtime_error("Could not parse unit equation: " + unit_equation);
	  }
  }

  /**
   * Load a set of units from a stream by reading lines from the stream and
   * passing them to the addUnit(std::string) function.
   */
  void loadUnits(std::istream& in)
  {
	  while (!in.eof()) {
		  std::string line;
		  std::getline(in, line);
		  // skip comments and blank lines
		  detail::trim(line);
		  if (line.size() < 1) continue;
		  if (line[0] == '#') continue;
		  this->addUnit(line);
	  }
  }

  /**
   * Load a set of units from a text file. The text file should contain lines
   * that could be passed to the addUnit(std::string) function.
   */
  void loadUnits(std::string filename)
  {
	  std::ifstream in(filename.c_str());
	  this->loadUnits(in);
	  in.close();
  }

  /**
   * Querry the registry for a named unit and return
   * the unit it represents. If the named unit is not
   * found in the registry, an exception is thrown.
   */
  const Unit& getUnit(std::string a_unit) const
  {
	  auto ptr = m_UnitStore.find(a_unit);
	  if (ptr != m_UnitStore.end()) {
		  return ptr->second;
	  } else {
		  // can't find the unit in the store. throw an exception.
		  throw std::runtime_error("Error: unit '" + a_unit +
			  "' does not exist in the registry.");
	  }
  }

  /**
   * Querry the registry for a named unit and return
   * a copy of the unit it represents. If unit isn't found
   * and a_trySIPrefix is true, then the function
   * checks if unit is an SI prefix'ed version of a unit
   * in the registry. If no unit is
   * found in the registry, an exception is thrown.
   */
  Unit getUnit(std::string a_unit, bool a_trySIPrefix) const
  {
	  auto ptr = m_UnitStore.find(a_unit);
	  if (ptr != m_UnitStore.end()) {
		  return ptr->second;
	  } else {
		  if (a_trySIPrefix) {
			  // check if the unit is an SI prefix'ed version of
			  // a unit in the store.
			  auto it    = a_unit.begin();
			  int  power = 0;
			  if (qi::parse(it, a_unit.end(), m_SIPrefixParser, power) &&
				  it != a_unit.end()) {
				  std::string unit(it, a_unit.end());
				  ptr = m_UnitStore.find(unit);
				  if (ptr != m_UnitStore.end()) {
					  return ptr->second * pow(10, power);
				  }
			  }
		  }
		  // can't find the unit in the store.
		  // throw an exception.
		  throw std::runtime_error("Error: unit '" + a_unit +
			  "' does not exist in the registry.");
	  }
  }

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
  Unit makeUnit(std::string a_unit) const
  {
	  auto it     = a_unit.begin();
	  Unit r_unit = BaseUnit<Dimension::Name::Dimensionless>();
	  auto r      = qi::parse(it, a_unit.end(), m_UnitParser, r_unit);
	  if (!r || it != a_unit.end()) {
		  // try to parse as a dimension
		  Dimension r_dim;
		  it = a_unit.begin();
		  r  = qi::parse(it, a_unit.end(), m_DimensionParser, r_dim);
		  if (!r || it != a_unit.end()) {
			  throw std::runtime_error(
				  "There was an error parsing unit in UnitRegistry::makeUnit: " +
				  a_unit);
		  }

		  r_unit = Unit(1, r_dim);
	  }
	  return r_unit;
  }

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
    Unit exponentiate(const Unit& b, const int e)
	{
		Unit r;
		for (int i = 0; i < abs(e); i++) {
			if (e > 0) r *= b;
			if (e < 0) r /= b;
		}
		return r;
	}

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
	UnitParser(const UnitRegistry& registry)
		: UnitRegistry::UnitParser::base_type(unit), ureg(registry)
	{
		offset   = qi::double_;
		exponent = qi::int_;

		scale = qi::double_[qi::_val *= qi::_1];

		auto space = qi::lit(" ");

		mul = *space >> "*" >> *space | +space;
		div = *space >> "/" >> *space;
		pow = *space >> (qi::lit("^") | qi::lit("**")) >> *space;
		add = *space >> "+" >> *space;
		sub = *space >> "-" >> *space;

		unit_name_begin_chars = qi::char_("a-zA-Z");
		unit_name_other_chars = qi::char_("a-zA-Z_0-9");
		named_unit =
			spt::as_string[+unit_name_begin_chars >> *unit_name_other_chars]
			[qi::_val = phx::bind(&ThisType::getUnitFromRegistry, this,
				qi::_1)];

		factor = (named_unit | scale | group)[qi::_val = qi::_1] >>
			*(pow >> exponent[qi::_val = phx::bind(&ThisType::exponentiate, this,
				qi::_val, qi::_1)]);

		term = factor[qi::_val = qi::_1] >> *(mul >> factor[qi::_val *= qi::_1] |
			div >> factor[qi::_val /= qi::_1]);

		group = '(' >> term[qi::_val = qi::_1] >> ')';

		expression = *space >> term[qi::_val = qi::_1] >>
			*(add >> offset[qi::_val += qi::_1] |
				sub >> offset[qi::_val -= qi::_1]) >>
			*space;

		unit = expression;
	}

  };

  /**
   * Symbol table parser to return base dimension for dimension symbols.
   */
  struct BaseDimensionSymbolParser : qi::symbols<char, Dimension> {
    BaseDimensionSymbolParser()
    {
      add("L", Dimension::Name::Length)("M", Dimension::Name::Mass)(
          "T", Dimension::Name::Time)("I", Dimension::Name::ElectricalCurrent)(
          "THETA", Dimension::Name::Temperature)("N", Dimension::Name::Amount)(
          "J", Dimension::Name::LuminousIntensity)("1",Dimension::Name::Dimensionless);
    }
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
    Dimension exponentiate(const Dimension& b, const int e)
	{
		Dimension r;
		for (int i = 0; i < abs(e); i++) {
			if (e > 0) r *= b;
			if (e < 0) r /= b;
		}
		return r;
	}

	/**
	* The dimension parser setup
	*/
	DimensionParser()
		: UnitRegistry::DimensionParser::base_type(dimension)
	{
		exponent   = qi::int_;
		auto space = qi::lit(" ");

		mul = *space >> "*" >> *space | +space;
		div = *space >> "/" >> *space;
		pow = *space >> (qi::lit("^") | qi::lit("**")) >> *space;
		add = *space >> "+" >> *space;
		sub = *space >> "-" >> *space;

		// a factor is a dimension symbol or a group, possibly raised to an exponent
		factor = (base_dimension_symbol | group)[qi::_val = qi::_1] >>
			*(pow >> exponent)[qi::_val = phx::bind(&ThisType::exponentiate,
				this, qi::_val, qi::_1)];

		// a term is a factor, possibly multiplied or divided by another factor
		term = factor[qi::_val = qi::_1] >> *(mul >> factor[qi::_val *= qi::_1] |
			div >> factor[qi::_val /= qi::_1]);

		// a group is a term wrapped in parenthesis
		group = '(' >> term[qi::_val = qi::_1] >> ')';

		// a dimension is a term wrapped in square brackets
		dimension = qi::lit("[") >> *space >> term[qi::_val = qi::_1] >> *space >>
			qi::lit("]");
	}
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
::Quantity<T> UnitRegistry::makeQuantity(const T&           val,
                                         const std::string& a_unit) const
{
  return ::Quantity<T>(val, makeUnit(a_unit), this);
}

template<typename T>
::Quantity<T> UnitRegistry::makeQuantity(std::string a_quantity) const
{
  double      value;
  std::string unit;
  // parse quantity string
  // a quantity should have a numerical value and a unit that makeUnit will
  // parse
  auto it = a_quantity.begin();
  auto r  = qi::parse(it, a_quantity.end(),
                     -qi::double_ >> qi::as_string[+qi::char_], value, unit);
  return ::Quantity<T>(static_cast<T>(value), makeUnit(unit), this);
}

#endif  // include protector
