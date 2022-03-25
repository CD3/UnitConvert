#include <catch.hpp>
#include <UnitConvert/Quantity.hpp>
#include <UnitConvert/basic_quantity.hpp>
#include <UnitConvert.hpp>

TEST_CASE("Quantity Class Tests")
{
  UnitConvert::BaseUnit<UnitConvert::Dimension::Name::Length>      meter;

  UnitConvert::Unit centimeter = meter / 100;
  UnitConvert::Unit inch       = 2.54 * centimeter;


  UnitConvert::Quantity<double> q1(2,meter);
  UnitConvert::Quantity<double> q2(10,inch);
  UnitConvert::Quantity<double> q3;


  q3 = q1 + q2;
  CHECK( q3.value() == Approx(2 + 10*0.0254) );
  CHECK( q3.unit().scale() == Approx(1) );
  CHECK( q3.unit().dimension() == UnitConvert::Dimension::Name::Length );

  q3 = q1 - q2;
  CHECK( q3.value() == Approx(2 - 10*0.0254) );
  CHECK( q3.unit().scale() == Approx(1) );
  CHECK( q3.unit().dimension() == UnitConvert::Dimension::Name::Length );

  q3 = q1 * q2;
  CHECK( q3.value() == Approx(2 * 10) );
  CHECK( q3.unit().scale() == Approx(2.54/100) );
  CHECK( q3.unit().dimension() == UnitConvert::Dimension(UnitConvert::Dimension::Name::Length)*UnitConvert::Dimension(UnitConvert::Dimension::Name::Length));

  q3 = q1 / q2;
  CHECK( q3.value() == Approx(2. / 10) );
  CHECK( q3.unit().scale() == Approx(100/2.54) );
  // hmmm, this should give dimensionless....
  CHECK( q3.unit().dimension() == UnitConvert::Dimension());

  q3 = 2*q1;
  CHECK( q3.value() == Approx(4) );
  CHECK( q3.unit().scale() == Approx(1) );
  CHECK( q3.unit().dimension() == UnitConvert::Dimension::Name::Length );

  q3 = q1*2;
  CHECK( q3.value() == Approx(4) );
  CHECK( q3.unit().scale() == Approx(1) );
  CHECK( q3.unit().dimension() == UnitConvert::Dimension::Name::Length );

  q3 = q1/2;
  CHECK( q3.value() == Approx(1) );
  CHECK( q3.unit().scale() == Approx(1) );
  CHECK( q3.unit().dimension() == UnitConvert::Dimension::Name::Length );
  


}

TEST_CASE("basic_quantity Class")
{
  using namespace unit_convert;
  basic_dimension<3> L(0);
  basic_dimension<3> T(1);
  basic_dimension<3> THETA(2);
  basic_unit<basic_dimension<3>> meter(L);
  basic_unit<basic_dimension<3>> second(T);
  basic_unit<basic_dimension<3>> kelvin(THETA);

  auto centimeter = meter/100;
  auto kilometer = 1000*meter;
  auto minute = 60*second;
  auto hour = 60*minute;
  auto celsius = kelvin + 273.15;
  auto fahrenheit = (5./9)*celsius - (5./9)*32; // note that F = 9/5 C + 32 is for the *components*
  // auto rankine = (9./5)*kelvin;
  // auto fahrenheit = rankine - 459.67;

  CHECK(celsius.scale() == Approx(1));
  CHECK(celsius.offset() == Approx(273.15));
  CHECK(fahrenheit.scale() == Approx(5./9));
  CHECK(fahrenheit.offset() == Approx(255.37));



  SECTION("Conversions")
  {
    basic_quantity<basic_unit<basic_dimension<3>>> distance(2, kilometer);;
    basic_quantity<basic_unit<basic_dimension<3>>> time(0.5,hour );;
    basic_quantity<basic_unit<basic_dimension<3>>> temperature(100,celsius);;

    CHECK(distance.to(centimeter).value() == Approx(200000));
    CHECK(time.to(minute).value() == Approx(30));
    CHECK(time.to(second).value() == Approx(3600/2));
    CHECK(temperature.to(fahrenheit).value() == Approx(212));

    // temperature = basic_quantity<basic_unit<basic_dimension<3>>>(100,celsius);;
    // CHECK(temperature.to(fahrenheit).value() == Approx(212));

    CHECK_THROWS( distance.to(second) );



  }

}

