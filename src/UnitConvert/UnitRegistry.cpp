
#include "./UnitRegistry.hpp"

bool UnitRegistry::isSIPrefix(char p) const
{
  // check the common ones first
  if (p == 'k') return true;
  if (p == 'c') return true;
  if (p == 'm') return true;
  if (p == 'u') return true;
  if (p == 'n') return true;
  if (p == 'p') return true;
  if (p == 'f') return true;

  if (p == 'Y') return true;
  if (p == 'Z') return true;
  if (p == 'E') return true;
  if (p == 'P') return true;
  if (p == 'T') return true;
  if (p == 'G') return true;
  if (p == 'M') return true;
  if (p == 'h') return true;
  if (p == 'D') return true;

  if (p == 'd') return true;
  if (p == 'a') return true;
  if (p == 'z') return true;
  if (p == 'y') return true;

  return false;
}

double UnitRegistry::getSIPrefixScale(char p) const
{
  if (p == 'Y') return 1e24;
  if (p == 'Z') return 1e21;
  if (p == 'E') return 1e18;
  if (p == 'P') return 1e15;
  if (p == 'T') return 1e12;
  if (p == 'G') return 1e9;
  if (p == 'M') return 1e6;
  if (p == 'k') return 1e3;
  if (p == 'h') return 1e2;
  if (p == 'D') return 1e1;

  if (p == 'd') return 1e-1;
  if (p == 'c') return 1e-2;
  if (p == 'm') return 1e-3;
  if (p == 'u') return 1e-6;
  if (p == 'n') return 1e-9;
  if (p == 'p') return 1e-12;
  if (p == 'f') return 1e-15;
  if (p == 'a') return 1e-18;
  if (p == 'z') return 1e-21;
  if (p == 'y') return 1e-24;

  throw std::runtime_error("Error: '" + std::string(1, p) +
                           "' is not a recognized SI prefix.");
}

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

void UnitRegistry::addUnit(const std::string& unit_equation)
{
  // get left and right sides of equation
  unit_string_tokenizer eq_toks(unit_equation,
                                boost::char_separator<char>("="));
  auto                  it   = eq_toks.begin();
  std::string           left = *it;
  detail::trim(left);

  ++it;
  if (it == eq_toks.end())
    throw std::runtime_error(
        "Error: string passed to addUnit must be a unit equation. No '=' "
        "found in '" +
        unit_equation + "'.");

  std::string right = *it;
  detail::trim(right);

  // need to check left hand side for a value.
  // for example, theuser might say something like
  //
  // 100 in = 254 cm
  //
  // we can choose not to support this, but either way,
  // we have to check for it

  unit_string_tokenizer l_toks(left, boost::char_separator<char>(" "));
  it = l_toks.begin();

  char*  end = 0;
  double val = std::strtod((*it).c_str(), &end);
  if (end == 0 || *end != 0) {
    val = 1;
  } else {
    ++it;
  }

  // need to rebuild left hand side of equation without value
  left = *it;
  while (++it != l_toks.end()) {
    left += " " + *it;
  }

  addUnit(left, makeUnit(right) / val);
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
