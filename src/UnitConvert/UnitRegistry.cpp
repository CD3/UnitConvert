
#include "./UnitRegistry.hpp"

  bool
UnitRegistry::isSIPrefix(char p) const
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


  double
UnitRegistry::getSIPrefixScale(char p) const
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


  void
UnitRegistry::addUnit(const std::string& k, const Unit& v)
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

  void
UnitRegistry::addUnit(const std::string& unit_equation)
{
  // get left and right sides of equation
  unit_string_tokenizer eq_toks(unit_equation,
      boost::char_separator<char>("="));
  auto it = eq_toks.begin();
  std::string left = *it;
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

  char* end = 0;
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


  const Unit&
UnitRegistry::getUnit(const std::string& a_unit) const
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
    Unit
UnitRegistry::makeUnit(const std::string& a_unit) const
      {
        // assume a dimensionless base unit.
        Unit r_unit = BaseUnit<Dimension::Name::Dimensionless>();

        // need to split the unit string into individual units
        // and process one at a time. units in the string will
        // be separated by either a '*', a '/', or a space ' ',
        // which we will interpret as multiplication.
        //
        // we are going to use boost tokenizer, and we want to keep
        // the characters that separate each unit.
        std::string operators = "+*/";
        unit_string_tokenizer tokens(
            a_unit, boost::char_separator<char>(" ", operators.c_str()));

        // need to keep track of the current operation.
        // the first one will be multiplication
        std::string op = "*";
        for (auto it : tokens) {
          // if the token is an operator, then just set
          // the current operator.
          if (it.size() == 1 && operators.find(it) != std::string::npos) {
            op = it;
            if (op == " ") op = "*";
          } else {
            // otherwise, do some more processing

            // it could either be a unit string, or a number.
            // it is (probably) more likely to be a unit string
            // so we'll try that first.

            // a unit or number may have a power, which will be specified
            // with a ^. so we need to tokenize the current token

            std::string base;
            // by default, assume we failed to parse the base. it's safer that
            // way...
            bool base_parsed = false;
            int power = 1;
            unit_string_tokenizer tok(it, boost::char_separator<char>("^"));
            auto iit = tok.begin();
            base = *iit;
            ++iit;
            if (iit != tok.end()) {  // a power was given
              power = boost::lexical_cast<int>(*iit);
            }

            // check if base is a unit in the store
            auto ptr = m_UnitStore.find(base);
            Unit t_unit = BaseUnit<Dimension::Name::Dimensionless>();
            if (ptr != m_UnitStore.end()) {
              // found unit in the store
              t_unit = ptr->second;
              base_parsed = true;
            } else {
              // didn't find unit in the store
              // check if base is an SI-prefixed unit that is
              // in the store.
              if (base.size() > 0 && isSIPrefix(base[0])) {
                // first character of base could be an SI prefix
                // check if the rest of base is a unit in the store.
                auto ptr = m_UnitStore.find(base.substr(1));
                if (ptr != m_UnitStore.end()) {
                  // found unit in store. don't forget to multiply by prefix.
                  t_unit = getSIPrefixScale(base[0]) * ptr->second;
                  base_parsed = true;
                }
              }

              if (!base_parsed) {
                // could not find base in the unit store.
                // check if it is a number.

                char* end = 0;
                double val = std::strtod(base.c_str(), &end);
                if (end != 0 && *end == 0) {
                  t_unit = t_unit * val;  // t_unit is dimensionless, so this is OK
                  base_parsed = true;
                }
              }
            }

            if (!base_parsed) {
              throw std::runtime_error("Error: unit '" + it +
                  "' does not exist in the registry.");
            }

            // if the power is negative, then we want to reverse the operation
            // and make the power positive.
            if (power < 0) {
              if (op == "/")
                op = "*";
              else
                op = "/";
              power = -power;
            }

            // multiply or divide by the unit n times
            while (power > 0) {
              if (op == "*")
                r_unit *= t_unit;
              else if (op == "/")
                r_unit /= t_unit;
              power--;
            }

            op = "*";  // reset operator to multiplication
          }
        }

        return r_unit;
      }



    std::ostream&
      operator<<(std::ostream& out, const UnitRegistry& reg)
      {
        for( auto &p : reg.m_UnitStore )
        {
          out << p.first << " -> " << p.second << "\n";
        }
        return out;
      }
