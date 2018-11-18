
#include "./UnitRegistry.hpp"


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
  std::string LHS, RHS;

  // parse unit equation
  // LHS should be a new named unit (no derived units) with an optional scale.
  // RHS can be an arbitrary string that makeUnit will parse
  // examples: 1 J = 1 kg * m^2 / s^s
  //           100 cm = 1 m
  auto space = qi::lit(" ");
  auto eq = qi::lit("=");
  auto uchars = m_UnitParser.unit_name_chars;
  auto r = qi::parse( it, unit_equation.end(), -qi::double_ >> *space >> qi::as_string[+uchars] >> *space >> eq >> *space >> qi::as_string[+qi::char_] >> *space, scale, LHS, RHS );
  if( r )
  {
    if( !scale )
      scale = 1;
    addUnit(LHS, makeUnit(RHS)/scale.get());
  }
  else
  {
    throw std::runtime_error("Could not parse unit equation: "+unit_equation);
  }

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
      throw std::runtime_error("There was an error parsing unit in UnitRegistry::makeUnit: "+a_unit);
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
