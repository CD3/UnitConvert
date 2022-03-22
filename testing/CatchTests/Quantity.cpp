#include <catch.hpp>
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

