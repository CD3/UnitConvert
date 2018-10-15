#include <catch.hpp>
#include <UnitConvert.hpp>

#include <boost/units/systems/si.hpp>

TEST_CASE("UnitRegistry Example")
{

UnitRegistry ureg;

// add base units to the registry
ureg.addBaseUnit<Dimension::Name::Length>("m");
ureg.addBaseUnit<Dimension::Name::Mass>("kg");
ureg.addBaseUnit<Dimension::Name::Time>("s");
ureg.addBaseUnit<Dimension::Name::Temperature>("K");
ureg.addBaseUnit<Dimension::Name::Amount>("mol");

// add some derived units to the registry
ureg.addUnit("100 cm = 1 m");
ureg.addUnit("1 in = 2.54 cm");
ureg.addUnit("1 ft = 12 in");
ureg.addUnit("1 J = 1 kg*m^2*s^-2");
ureg.addUnit("1 W = 1 J/s");
ureg.addUnit("1 cal = 4.184 J");

Quantity<double> q = ureg.makeQuantity<double>(200, "cm");

// q.value() returns the numerical value of the quantity
CHECK(q.value() == Approx(200));
CHECK(q.to("m").value() == Approx(2));
// millimeter was not defined, but we can use SI-prefixed versions of any unit in the registry
CHECK(q.to("mm").value() == Approx(2000));
// converting to a unit with different dimensions will throw an error
CHECK_THROWS(q.to(ureg.getUnit("s")));




// quantities  can be assigned to new dimensions
q = ureg.makeQuantity<double>(0.25, "s");
CHECK(q.value() == Approx(0.25));
CHECK(q.to("ms").value() == Approx(250));
// conversions have to match the quantities current dimensions.
CHECK_THROWS(q.to(ureg.getUnit("m")));



q = ureg.makeQuantity<double>(100, "cal");
CHECK(q.value() == Approx(100));
CHECK(q.to("J").value() == Approx(100 * 4.184));
// again, we can use SI prefixes
CHECK(q.to("mJ").value() == Approx(100 * 4.184*1000));
// we can convert to arbitrary combinations of defined dimensions
CHECK(q.to("kg*m^2*s^-2").value() == Approx(100 * 4.184));
CHECK(q.to("kg*m^2*ms^-2").value() == Approx(100 * 4.184 / 1000 / 1000));
CHECK(q.to("W*s").value() == Approx(100 * 4.184));
CHECK(q.to("kW*s").value() == Approx(100 * 4.184/1000));

CHECK_THROWS(q.to("m"));



// The unit registry can create Boost.Units quantities.
// So we can easily convert to a unit in one of the Boost.Unit
// systems and then create a Boost.Unit quantity.
q = ureg.makeQuantity<double>(100, "ft");
boost::units::quantity<boost::units::si::length> L = q.to<boost::units::si::length>();
CHECK(boost::units::quantity_cast<double>(L) == Approx(30.48));

}
