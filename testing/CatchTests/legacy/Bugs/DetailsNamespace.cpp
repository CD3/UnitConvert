
#include <boost/units/systems/si.hpp>
// boost::units::detail namespace contains
// a src() function. if our detail namespace
// is placed at the global level, this causes
// collisions when the boost::units namespace is
// "used"
using namespace boost::units;
#include <UnitConvert.hpp>
