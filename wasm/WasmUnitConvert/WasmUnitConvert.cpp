#include <UnitConvert/HighLevelFunctions.hpp>
#include <emscripten/bind.h>



EMSCRIPTEN_BINDINGS(WasmUnitConver)
{
  emscripten::function("UnitConvertString",&UnitConvert::UnitConvertString);
  emscripten::function("GetMagnitudeInUnit",&UnitConvert::GetMagnitudeInUnit);
  emscripten::function("HaveSameDimensions",&UnitConvert::HaveSameDimensions);
  emscripten::function("AddUnitDefinition",&UnitConvert::AddUnitDefinition);
}

