#include <catch.hpp>
#include <unit_convert/quantities/basic_quantity.hpp>
#include <unit_convert/quantities/si_quantity.hpp>

TEST_CASE("basic_quantity Class")
{
  using namespace unit_convert;
  basic_dimension<3> L(0);
  basic_dimension<3> T(1);
  basic_dimension<3> THETA(2);
  basic_unit<basic_dimension<3>> meter(L);
  basic_unit<basic_dimension<3>> second(T);
  basic_unit<basic_dimension<3>> kelvin(THETA);

  auto centimeter = meter / 100;
  auto kilometer = 1000 * meter;
  auto inch = 2.54*centimeter;
  auto foot = 12*inch;
  auto minute = 60 * second;
  auto hour = 60 * minute;
  SECTION("Conversions")
  {
    basic_quantity<basic_unit<basic_dimension<3>>> distance(2, kilometer);
    basic_quantity<basic_unit<basic_dimension<3>>> time(0.5, hour);

    CHECK(distance.to(centimeter).value() == Approx(200000));
    CHECK(time.to(minute).value() == Approx(30));
    CHECK(time.to(second).value() == Approx(3600 / 2));

    CHECK_THROWS(distance.to(second));
  }

  SECTION("Temperature Conversions")
  {
    auto celsius = kelvin + 273.15;
    auto fahrenheit = (5. / 9) * celsius - 32;  // note that F = 9/5 C + 32 is for the *components*

    CHECK(celsius.scale() == Approx(1));
    CHECK(celsius.offset() == Approx(273.15));
    CHECK(fahrenheit.scale() == Approx(5. / 9));
    CHECK(fahrenheit.offset() == Approx(255.37));

    basic_quantity<basic_unit<basic_dimension<3>>> temperature(100, celsius);
    CHECK(temperature.to(fahrenheit).value() == Approx(212));

    auto rankine = (5. / 9) * kelvin;
    fahrenheit = rankine + 459.67;

    CHECK(temperature.to(fahrenheit).value() == Approx(212));
  }

  SECTION("Quantity Arithmetic") {

    auto  x1 = make_quantity(10., foot);
    auto  x2 = make_quantity<double>(2, meter);

    auto x3 = x1+x2;

    CHECK( x3.value() == Approx(16.56168) );
    CHECK( x3.unit() == x1.unit() );

    x3 = x2 - x3;

    CHECK( x3.value() == Approx(-3.048) );
    CHECK( x3.unit() == x2.unit() );

  }
}

TEST_CASE("si_quantity Class")
{
  using namespace unit_convert;

  si_unit<double> radian;
  si_base_unit<si_dimension::name::Length, double> meter;
  si_base_unit<si_dimension::name::Time, double> second;
  si_base_unit<si_dimension::name::ElectricalCurrent, double> ampere;
  si_base_unit<si_dimension::name::Temperature, double> kelvin;
  si_base_unit<si_dimension::name::Amount, double> mol;
  si_base_unit<si_dimension::name::LuminousIntensity, double> candela;

  auto centimeter = 0.01 * meter;
  auto inch = 2.54 * centimeter;
  auto foot = 12 * inch;

  auto minute = 60 * second;
  si_unit<double> hour = 60 * minute;

  si_quantity<double> x(10, meter);
  si_quantity<double> t(2, hour);

  CHECK(x.to(foot).value() == Approx(32.8084));
  CHECK(t.to(second).value() == Approx(2 * 3600));

  CHECK(
      std::is_same<decltype(minute), basic_unit<si_dimension, double>>::value);
  CHECK(std::is_same<decltype(hour), si_unit<double>>::value);
}

TEST_CASE("registered_quantity")
{
  // registered_quantity<double> x = "2 m";
  // x.to("ft");
}

