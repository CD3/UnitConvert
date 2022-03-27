#pragma once

/** @file UnitConvert.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 09/02/18
  */

#include "UnitConvert/legacy/Dimension.hpp"
#include "UnitConvert/legacy/Unit.hpp"
#include "UnitConvert/legacy/Quantity.hpp"
#include "UnitConvert/legacy/UnitRegistry.hpp"
#include "UnitConvert/legacy/BoostUnitRegistry.hpp"

#ifndef UNITCONVERT_NO_BACKWARD_COMPATIBLE_NAMESPACE
using namespace UnitConvert;
#pragma message ( "WARNING: the UnitConvert library is now under the 'UnitConvert' namespace. To maintain backward compatibility, this namespace is being imported to the global namespace. To disable this, define UNITCONVERT_NO_BACKWARD_COMPATIBLE_NAMESPACE before including 'UnitConvert.hpp' and prefix library objects with UnitConvert::" )
#endif


