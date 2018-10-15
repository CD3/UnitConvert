%module UnitConvert
%{
#include <UnitConvert.hpp>
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

%rename(output) operator<<;
%rename(tQuantity) Quantity;


%include "../src/UnitConvert/UnitRegistry.hpp"
%template(addLengthBaseUnit) UnitRegistry::addBaseUnit<Dimension::Name::Length>;
%template(addMassBaseUnit) UnitRegistry::addBaseUnit<Dimension::Name::Mass>;
%template(addTimeBaseUnit) UnitRegistry::addBaseUnit<Dimension::Name::Time>;
%template(addElectricalCurrentBaseUnit) UnitRegistry::addBaseUnit<Dimension::Name::ElectricalCurrent>;
%template(addTemperatureBaseUnit) UnitRegistry::addBaseUnit<Dimension::Name::Temperature>;
%template(addAmountBaseUnit) UnitRegistry::addBaseUnit<Dimension::Name::Amount>;
%template(addLuminousIntensityBaseUnit) UnitRegistry::addBaseUnit<Dimension::Name::LuminousIntensity>;
%template(addDimensionlessBaseUnit) UnitRegistry::addBaseUnit<Dimension::Name::Dimensionless>;
%template(makeQuantity) UnitRegistry::makeQuantity<double>;

%include "../src/UnitConvert/Quantity.hpp"
%template(Quantity) Quantity<double>;

class Unit
{
  public:
    %extend {
      Unit(){}
    }
};

