%module pyUnitConvert
%{
#define UNITCONVERT_NO_BACKWARD_COMPATIBLE_NAMESPACE
#include <UnitConvert.hpp>
using namespace UnitConvert;
%}

%include <std_string.i>
%include <exception.i>

// this is a generic exception handler that will work for multiple
// target languages. it basically just forwards any exceptions that are thrown
// to the swig exceptiosn library. specializations can be added before the
// default 'catch(...)' if more control is needed.
%exception {
  try {
    $action
  } catch (const std::invalid_argument& e) {
    SWIG_exception(SWIG_ValueError, e.what());
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}



%rename(TQuantity) Quantity;
template<typename T>
class Quantity
{
 public:
  tQuantity() {};
  const T& value() const;
  Quantity<T> to(const std::string& unit) const;
};
%template(Quantity) Quantity<double>;


class UnitRegistry
{
 public:
  UnitRegistry() : m_UnitParser(*this){};

  enum class EXISTING_UNIT_POLICY { Warn, Throw, Ignore };
  EXISTING_UNIT_POLICY existing_unit_policy = EXISTING_UNIT_POLICY::Throw;

  void addUnit(std::string unit_equation);
  template<typename T>
  Quantity<T> makeQuantity(std::string a_unit) const;

};
%template(makeQuantity) UnitRegistry::makeQuantity<double>;



