
#include "./UnitRegistry.hpp"
#include "./detail//functions.hpp"

#include <fstream>

void UnitRegistry::addUnit(const std::string& k, const Unit& v)
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

void UnitRegistry::addUnit(std::string unit_equation)
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
  auto uchars = m_UnitParser.unit_name_chars;
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

void UnitRegistry::loadUnits(std::istream& in)
{
  while (!in.eof()) {
    std::string line;
    std::getline(in, line);
    // skip comments and blank lines
    detail::trim(line);
    if( line.size() < 1 )
      continue;
    if( line[0] == '#' )
      continue;
    this->addUnit(line);
  }
}

void UnitRegistry::loadUnits(std::string filename)
{
  std::ifstream in(filename.c_str());
  this->loadUnits(in);
  in.close();
}

const Unit& UnitRegistry::getUnit(std::string a_unit) const
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

Unit UnitRegistry::getUnit(std::string a_unit, bool a_trySIPrefix) const
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
Unit UnitRegistry::makeUnit(std::string a_unit) const
{
  auto it     = a_unit.begin();
  Unit r_unit = BaseUnit<Dimension::Name::Dimensionless>();
  auto r      = qi::parse(it, a_unit.end(), m_UnitParser, r_unit);
  if (!r || it != a_unit.end()) {
    throw std::runtime_error(
        "There was an error parsing unit in UnitRegistry::makeUnit: " + a_unit);
  }
  return r_unit;
}

std::ostream& operator<<(std::ostream& out, const UnitRegistry& reg)
{
  for (auto& p : reg.m_UnitStore) {
    out << p.first << " -> " << p.second << "\n";
  }
  return out;
}

/**
 * compute a unit raised to an integer power)
 */
Unit UnitRegistry::UnitParser::exponentiate(const Unit& b, const int e)
{
  Unit r;
  for (int i = 0; i < abs(e); i++) {
    if (e > 0) r *= b;
    if (e < 0) r /= b;
  }
  return r;
}

Unit UnitRegistry::UnitParser::getUnitFromRegistry(const std::string& unit)
{
  return ureg.getUnit(unit, true);
}

UnitRegistry::UnitParser::UnitParser(const UnitRegistry& registry)
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

  unit_name_chars = qi::char_("a-zA-Z");
  named_unit =
      spt::as_string[(+unit_name_chars)]
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
