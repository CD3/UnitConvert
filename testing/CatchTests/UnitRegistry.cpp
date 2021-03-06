#include <catch.hpp>

#include <sstream>

#include <boost/units/systems/si.hpp>

#include <UnitConvert.hpp>
#include <UnitConvert/GlobalUnitRegistry.hpp>


TEST_CASE("UnitRegisty Tests")
{
  using namespace UnitConvert;
  UnitRegistry ureg;
  CHECK(ureg.size() == 0);

  SECTION("Adding derived units from strings")
  {
    // add base units to the registry
    ureg.addBaseUnit<Dimension::Name::Length>("cm");
    ureg.addBaseUnit<Dimension::Name::Mass>("g");
    ureg.addBaseUnit<Dimension::Name::Time>("s");
    ureg.addBaseUnit<Dimension::Name::Temperature>("K");
    CHECK(ureg.size() == 4);

    // add some derived units
    ureg.addUnit("m = 100 cm");
    ureg.addUnit("1 in = 2.54 cm");
    ureg.addUnit("1 ft = 12 in");
    ureg.addUnit("1 J = 1 kg*m^2*s^-2");
    ureg.addUnit("1 W = 1 J/s");
    ureg.addUnit("1 cal = 4.184 J");
    ureg.addUnit("1 degC = K - 273.15");
    ureg.addUnit("C = K - 273.15");
    ureg.addUnit("delta_K = K");
    ureg.addUnit("delta_C = delta_K");

    Quantity<double> q;

    q = ureg.makeQuantity<double>(24, "cm");
    CHECK(q.to("m").value() == Approx(0.24));
    CHECK(q.to("cm").value() == Approx(24));
    CHECK(q.to("mcm").value() == Approx(24000));
    CHECK(q.to("mm").value() == Approx(240));
    CHECK(q.to_base_units().value() == Approx(24));

    q = ureg.makeQuantity<double>(10, "in");
    CHECK(q.to_base_units().value() == Approx(25.4));

    q = ureg.makeQuantity<double>(1, "J");
    // base units are g and cm so...
    // kg m^-2 s^-2   g/kg cm/m cm/m = 1000 100 100 g cm^-2 s^-2 = 10,000,000 g
    // cm^-2 s^-2
    CHECK(q.to_base_units().value() == Approx(1e7));

    q = ureg.makeQuantity<double>(2, "cal");
    CHECK(q.to_base_units().value() == Approx(2 * 4.184 * 1e7));

    q = ureg.makeQuantity<double>("100 degC");
    CHECK(q.to("K").value() == Approx(373.15));
    CHECK(q.to_base_units().value() == Approx(373.15));

    CHECK(q.unit().is_offset());
    CHECK(q.unit().offset() == Approx(-273.15));
    CHECK(!q.to("K").unit().is_offset());
    CHECK(q.to("K").unit().offset() == Approx(0));
    // to_base_units will return a quantity with an offset unit here,
    // but the offset will be zero.
    CHECK(q.to_base_units().unit().is_offset());
    CHECK(q.to_base_units().unit().offset() == Approx(0));

    CHECK(ureg.makeQuantity<double>("20 C").to("K").value() == Approx(293.15));
    CHECK(ureg.makeQuantity<double>("20 1/delta_C").to("1/K").value() ==
          Approx(20));
    CHECK(
        ureg.makeQuantity<double>("1 cal / g / delta_C").to("J/g/K").value() ==
        Approx(4.184));
  }

  SECTION("Loading derived units from stream")
  {
    // add base units to the registry
    ureg.addBaseUnit<Dimension::Name::Length>("cm");
    ureg.addBaseUnit<Dimension::Name::Mass>("g");
    ureg.addBaseUnit<Dimension::Name::Time>("s");

    std::string units =
        "m = 100 cm\n\n N = kg m / s^2\n # comment\nJ = N m\nW = J/s";
    std::stringstream in(units);
    ureg.loadUnits(in);

    CHECK(ureg.makeQuantity<double>(10, "g m^2 / s^3").to("W").value() == 0.01);
  }

  SECTION("Loading units from file")
  {
    ureg.loadUnits("unit_definitions.txt");

    CHECK(ureg.makeQuantity<double>(10, "g m^2 / s^3").to("W").value() == 0.01);
    CHECK(ureg.makeQuantity<double>(5, "m / s^2").to("gravity").value() ==
          Approx(5 / 9.80665));
    CHECK(ureg.makeQuantity<double>(5, "gravity").to("m/s^2").value() ==
          Approx(5 * 9.80665));

    // H2O is a unit for pressure. 1 H2O is 9806.65 Pa
    CHECK(ureg.makeQuantity<double>(5, "H2O").to("Pa/m").value() ==
          Approx(5 * 9806.65));
  }

  SECTION("Adding base units from strings")
  {
    ureg.addUnit("m = [L]");
    ureg.addUnit("g = [M]");
    ureg.addUnit("s = [T]");
    ureg.addUnit("K = [THETA]");
    ureg.addUnit("A = [I]");
    ureg.addUnit("mol = [N]");
    ureg.addUnit("cd = [J]");
    ureg.addUnit("rad = [1]");

    ureg.addUnit("100 cm = m");
    ureg.addUnit("in = 2.54 cm");
    ureg.addUnit("ft = 12 in");
    ureg.addUnit("J = 1 kg*m^2*s^-2");
    ureg.addUnit("W = 1 J/s");
    ureg.addUnit("cal = 4.184 J");

    CHECK(ureg.makeQuantity<double>("2 m").to("cm").value() == Approx(200));
    CHECK(ureg.makeQuantity<double>("2 m").to("in").value() ==
          Approx(200 / 2.54));
  }

  SECTION("Parsing errors")
  {
    CHECK_THROWS(ureg.getUnit("m"));
    CHECK_THROWS(ureg.makeUnit("m"));
    CHECK_THROWS(ureg.getUnit("[L]"));
    CHECK_NOTHROW(ureg.makeUnit("[L]"));
    CHECK_THROWS(ureg.makeQuantity<double>("10"));

  }
}

TEST_CASE("Global Unit Registry Tests")
{
  using namespace UnitConvert;
  SECTION("First Usage")
  {
    UnitRegistry& ureg = getGlobalUnitRegistry();

    CHECK(ureg.makeQuantity<double>("2 m").to("cm").value() == Approx(200));
    CHECK(ureg.makeQuantity<double>("2 J").to("kg cm^2 / s^2").value() ==
          Approx(2. * 100 * 100));
    CHECK(ureg.makeQuantity<float>("0 degC").to("degF").value() == Approx(32));
  }

  SECTION("Second Usage")
  {
    UnitRegistry& ureg = getGlobalUnitRegistry();

    CHECK(ureg.makeQuantity<double>("2 m").to("cm").value() == Approx(200));
    CHECK(ureg.makeQuantity<double>("2 J").to("kg cm^2 / s^2").value() ==
          Approx(2. * 100 * 100));
    CHECK(ureg.makeQuantity<float>("0 degC").to("degF").value() == Approx(32));

  }

  SECTION("Obscure conversions")
  {
    UnitRegistry& ureg = getGlobalUnitRegistry();

    auto q = ureg.makeQuantity<float>("2 pound");
    CHECK(q.to("kg").value() == Approx(0.90718474));
    CHECK(q.to("electron_mass").value() == Approx(9.95879467317e+29));
    CHECK(q.to("carat").value() == Approx(4535.9237));
    CHECK(q.to("metric_ton").value() == Approx(0.00090718474));
    CHECK(q.to("bag").value() == Approx(0.0212765957447));
    CHECK(q.to("grain").value() == Approx(14000.0));
    CHECK(q.to("oz").value() == Approx(32));
    CHECK(q.to("short_ton").value() == Approx(0.001));

  }

}

TEST_CASE("UnitRegisty Adding Unit Tests")
{
  using namespace UnitConvert;
  UnitRegistry ureg;
  CHECK(ureg.size() == 0);

  ureg.addUnit("m = [L]");
  ureg.addUnit("cm = 0.01 m");

  CHECK(ureg.makeQuantity<double>("2 m").to("cm").value() == Approx(200));

  CHECK_THROWS(ureg.addUnit("cm = 10 m"));
  CHECK(ureg.makeQuantity<double>("2 m").to("cm").value() == Approx(200));

  ureg.existing_unit_policy = UnitRegistry::EXISTING_UNIT_POLICY::Warn;
  ureg.addUnit("cm = 10 m");
  CHECK(ureg.makeQuantity<double>("2 m").to("cm").value() == Approx(200));

  ureg.existing_unit_policy = UnitRegistry::EXISTING_UNIT_POLICY::Ignore;
  ureg.addUnit("cm = 10 m");
  CHECK(ureg.makeQuantity<double>("2 m").to("cm").value() == Approx(200));

  ureg.existing_unit_policy = UnitRegistry::EXISTING_UNIT_POLICY::Overwrite;
  ureg.addUnit("cm = 10 m");
  CHECK(ureg.makeQuantity<double>("2 m").to("cm").value() == Approx(0.2));






}
