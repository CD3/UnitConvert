#include <pybind11/pybind11.h>
#define UNITCONVERT_NO_BACKWARD_COMPATIBLE_NAMESPACE
#include "../UnitConvert.hpp"

PYBIND11_MODULE(pyUnitConvert,m){
  m.doc() = "A C++ library for doing runtime unit conversions.";
  pybind11::class_<UnitConvert::Quantity<double>>(m,"Quantity")
    .def(pybind11::init())
    .def("to",(UnitConvert::Quantity<double>(UnitConvert::Quantity<double>::*)(const std::string&) const)&UnitConvert::Quantity<double>::to)
    .def("value",(const double&(UnitConvert::Quantity<double>::*)() const)&UnitConvert::Quantity<double>::value)
    ;
  pybind11::class_<UnitConvert::UnitRegistry>(m,"UnitRegistry")
    .def(pybind11::init())
    .def("addUnit",(void(UnitConvert::UnitRegistry::*)(std::string))&UnitConvert::UnitRegistry::addUnit)
    .def("makeQuantity",(UnitConvert::Quantity<double> (UnitConvert::UnitRegistry::*)(std::string) const)&UnitConvert::UnitRegistry::makeQuantity)
    ;
}



