#ifndef UnitConvert_hpp
#define UnitConvert_hpp

/** @file UnitConvert.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 09/02/18
  */

#include "UnitConvert/Dimension.hpp"
#include "UnitConvert/Unit.hpp"
#include "UnitConvert/Quantity.hpp"
#include "UnitConvert/UnitRegistry.hpp"
#include "UnitConvert/BoostUnitRegistry.hpp"

#ifndef UNITCONVERT_NO_BACKWARD_COMPATIBLE_NAMESPACE
using namespace UnitConvert;
#pragma message ( "WARNING: the UnitConvert library is now under the 'UnitConvert' namespace. To maintain backward compatibility, this namespace is being imported to the global namespace. To disable this, define UNITCONVERT_NO_BACKWARD_COMPATIBLE_NAMESPACE before including 'UnitConvert.hpp' and prefix library objects with UnitConvert::" )
#endif


#endif // include protector
