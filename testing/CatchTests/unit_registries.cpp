#include <sstream>
#include <iostream>
#include <iterator>

#include <boost/units/systems/si.hpp>

#include <unit_convert/unit_registries/global_unit_registry.hpp>
#include <unit_convert/io.hpp>
#include <unit_convert/quantities/add_registry.hpp>
#include <unit_convert/quantities/basic_quantity.hpp>
#include <unit_convert/unit_registries/basic_unit_registry.hpp>
#include <unit_convert/unit_registries/expression_unit_registry.hpp>
#include <unit_convert/unit_registries/si_expression_unit_registry.hpp>
#include <unit_convert/unit_registries/si_unit_registry.hpp>
#include <unit_convert/units/si_unit.hpp>

#include <catch.hpp>

TEST_CASE("v1 unit registry classes")
{
  using namespace unit_convert;
  SECTION("basic_unit_registry class")
  {
    SECTION("using basic units")
    {
      using dim_type = basic_dimension<3>;
      using unit_type = basic_unit<dim_type, double>;
      basic_unit_registry<unit_type> ureg;
      CHECK(ureg.size() == 0);

      ureg.add_unit("m", basic_dimension<3>(0));
      ureg.add_unit("s", basic_dimension<3>(1));
      ureg.add_unit("K", basic_dimension<3>(2));

      CHECK(ureg.size() == 3);

      ureg.add_unit("cm", ureg.get_unit("m") / 100);
      ureg.add_unit("in", 2.54 * ureg.get_unit("cm"));
      ureg.add_unit("ft", 12 * ureg.get_unit("in"));

      CHECK(ureg.size() == 6);

      ureg.add_unit("min", 60 * ureg.get_unit("s"));
      ureg.add_unit("hr", 60 * ureg.get_unit("min"));
      ureg.add_unit("R", (5. / 9) * ureg.get_unit("K"));

      auto x = basic_quantity<basic_unit<dim_type, double>, double>(
          10, ureg.get_unit("ft"));
      auto t = ureg.make_quantity<double>(10, "hr");
      auto T = ureg.make_quantity(310., "K");

      CHECK(x.to(ureg.get_unit("m")).value() == Approx(3.048));
      CHECK(t.to(ureg.get_unit("s")).value() == Approx(36000));
      CHECK(T.to(ureg.get_unit("R")).value() == Approx(310 * 9. / 5));

      basic_unit_registry<unit_type>::registered_quantity_type<double> y(ureg);
      y = x;

      CHECK(y.to(ureg.get_unit("m")).value() == Approx(3.048));
      CHECK(y.to("m").value() == Approx(3.048));

      auto z = ureg.make_quantity<double>();
      z = x;

      CHECK(z.to(ureg.get_unit("m")).value() == Approx(3.048));
      CHECK(z.to("m").value() == Approx(3.048));

      CHECK_THROWS(ureg.make_quantity(10, "m/s**2"));
    }

    SECTION("SI Unit Registry")
    {
      using namespace unit_convert;
      basic_unit_registry<si_unit<double>> ureg;
      CHECK(ureg.size() == 0);
    }
  }
  SECTION("expression_unit_registry")
  {
    expression_unit_registry<basic_unit<basic_dimension<3>, double>> ureg;

    ureg.add_dimension_symbol("1", basic_dimension<3>());
    ureg.add_dimension_symbol("L", basic_dimension<3>(0));
    ureg.add_dimension_symbol("T", basic_dimension<3>(1));
    ureg.add_dimension_symbol("THETA", basic_dimension<3>(3));

    ureg.add_unit("m = [L]");
    ureg.add_unit("s = [T]");
    ureg.add_unit("K = [THETA]");

    ureg.add_unit("in = 2.54 * cm");
    ureg.add_unit("ft = 12 in");
    ureg.add_unit("mi = 5280 ft");
    ureg.add_unit("min = 60 s");
    ureg.add_unit("hr = 60 min");
    ureg.add_unit("R = 5*K/9");

    auto v = ureg.make_quantity<double>(10, "m/s");
    auto r = ureg.make_quantity<double>(5, "K/min");

    CHECK(v.to("mi/hr").value() == Approx(22.369363));
    CHECK(r.to("R/s").value() == Approx(5 * (9. / 5) / 60));

    v = ureg.make_quantity<double>(10, "mm/ms");
    CHECK(v.to("m/s").value() == Approx(10));
    CHECK(v.to("cm/cs").value() == Approx(10));
    CHECK(v.to("km/ks").value() == Approx(10));

    auto g = ureg.make_quantity<double>(9.80665, "m/s^2");

    CHECK(g.to("ft/s^2").value() == Approx(32.1741));
    CHECK(g.to("ft/min^2").value() == Approx(32.1741 * 60 * 60));
  }
  SECTION("si_unit_registry class")
  {
    SECTION("simple")
    {
      using namespace unit_convert;
      si_unit_registry<double> ureg;
      CHECK(ureg.size() == 7);

      ureg.add_unit("in", 2.54 * ureg.get_unit("m") / 100.);
      ureg.add_unit("ft", 12 * ureg.get_unit("in"));

      ureg.add_unit("min", 60 * ureg.get_unit("s"));
      ureg.add_unit("hr", 60 * ureg.get_unit("min"));
      ureg.add_unit("R", (5. / 9) * ureg.get_unit("K"));

      CHECK_THROWS(ureg.get_unit("cm"));
    }
  }
  SECTION("si_expression_unit_registry")
  {
    si_expression_unit_registry<double> ureg;

    // si base units and prefixed versions are already in the registry
    ureg.add_unit("in", 2.54 * ureg.get_unit("cm"));
    ureg.add_unit("ft", 12 * ureg.get_unit("in"));
    ureg.add_unit("mi", 5280 * ureg.get_unit("ft"));
    ureg.add_unit("min", 60 * ureg.get_unit("s"));
    ureg.add_unit("hr", 60 * ureg.get_unit("min"));
    ureg.add_unit("R", (5. / 9) * ureg.get_unit("K"));

    auto v = ureg.make_quantity<double>(10, "m/s");
    auto r = ureg.make_quantity<double>(5, "K/min");

    CHECK(v.to("mi/hr").value() == Approx(22.369363));
    CHECK(r.to("R/s").value() == Approx(5 * (9. / 5) / 60));

    v = ureg.make_quantity<double>(10, "mm/ms");
    CHECK(v.to("m/s").value() == Approx(10));
    CHECK(v.to("cm/cs").value() == Approx(10));
    CHECK(v.to("km/ks").value() == Approx(10));

    auto g = ureg.make_quantity<double>(9.80665, "m/s^2");

    CHECK(g.to("ft/s^2").value() == Approx(32.1741));
    CHECK(g.to("ft/min^2").value() == Approx(32.1741 * 60 * 60));
  }
}

TEST_CASE("v1 lobal Unit Registry Tests")
{
  SECTION("1.0") 
  {
    using namespace unit_convert;
    SECTION("First Usage")
    {
      si_expression_unit_registry<double>& ureg = get_global_unit_registry();
      auto keys = ureg.get_unit_symbols();
    
      // std::copy( keys.begin(), keys.end(), std::ostream_iterator<std::string>(std::cout,"\n"));

      CHECK(ureg.make_quantity<double>("2 m").to("cm").value() == Approx(200));
      CHECK(ureg.make_quantity<double>("2 J").to("kg cm^2 / s^2").value() ==
            Approx(2. * 100 * 100));
      CHECK(ureg.make_quantity<float>("0 degC").to("degF").value() ==
            Approx(32));
    }

    SECTION("Second Usage")
    {
      auto& ureg = get_global_unit_registry();

      CHECK(ureg.make_quantity<double>("2 m").to("cm").value() == Approx(200));
      CHECK(ureg.make_quantity<double>("2 J").to("kg cm^2 / s^2").value() ==
            Approx(2. * 100 * 100));
      CHECK(ureg.make_quantity<float>("0 degC").to("degF").value() ==
            Approx(32));
    }

    SECTION("Obscure conversions")
    {
      auto& ureg = get_global_unit_registry();

      auto q = ureg.make_quantity<float>("2 pound");
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
}

