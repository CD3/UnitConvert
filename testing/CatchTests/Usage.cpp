//#include "catch.hpp"

//#include <UnitConvert.hpp>
//#include <boost/units/systems/si.hpp>


//TEST_CASE("Boost Runtime Units", "[usage]")
//{
  //using namespace boost;
  //using namespace boost::units;

  //BoostUnitRegistry<si::system> ureg;

  //ureg.addUnit(si::joule);
  //ureg.addUnit(si::watt);


  //ureg.addUnit("cal = 4.184 J");
  //ureg.addUnit("hp = 745.7");



  //Quantity<double> q = ureg.makeQuantity<double>(20, "cm");

  //CHECK(q.value() == Approx(20));
  //CHECK(q.to(si::meter).value() == Approx(0.20));
  //CHECK(q.to(si::meter).value() == Approx(0.20));

  ////quantity<si::mm> length = q.to<t::mm>();
  ////CHECK(quantity_cast<double>(length) == Approx(200));

  ////CHECK_THROWS(q.to<t::s>());


  ////q = ureg.makeQuantity<double>(300, "cal");

  ////quantity<t::J> energy = q.to<t::J>();
  ////CHECK(quantity_cast<double>(energy) == Approx(300*4.184));







//}
