#include <catch.hpp>
#include <sstream>

#include <unit_convert/units/basic_unit.hpp>
#include <unit_convert/units/si_unit.hpp>
#include <unit_convert/io.hpp>

TEST_CASE("basic_unit class tests")
{
using namespace unit_convert;
  basic_dimension<3> length(0);
  basic_dimension<3> time(1);
  basic_dimension<3> temperature(2);

  basic_unit<basic_dimension<3>> radian;
  basic_unit<basic_dimension<3>> meter(length);
  basic_unit<basic_dimension<3>> centimeter(0.01, length);
  basic_unit<basic_dimension<3>> second(1, time);
  basic_unit<basic_dimension<3>> celsius(1, 273, temperature);

  SECTION("Member Access")
  {
    CHECK(radian.scale() == Approx(1));
    CHECK(meter.scale() == Approx(1));
    CHECK(second.scale() == Approx(1));
    CHECK(celsius.scale() == Approx(1));

    CHECK(radian.offset() == Approx(0).scale(1));
    CHECK(meter.offset() == Approx(0).scale(1));
    CHECK(second.offset() == Approx(0).scale(1));
    CHECK(celsius.offset() == Approx(273));

    CHECK(radian.dimension()[0] == 0);
    CHECK(radian.dimension()[1] == 0);
    CHECK(radian.dimension()[2] == 0);

    CHECK(meter.dimension()[0] == 1);
    CHECK(meter.dimension()[1] == 0);
    CHECK(meter.dimension()[2] == 0);
    CHECK(meter.dimension() == length);

    CHECK(celsius.dimension() == temperature);

    CHECK(basic_unit<basic_dimension<3>>(1, length).dimension()[0] == 1);
    CHECK(basic_unit<basic_dimension<3>>(1, length).dimension() == length);
  }

  SECTION("Querries")
  {
    CHECK(!radian.is_offset());
    CHECK(!meter.is_offset());
    CHECK(!second.is_offset());
    CHECK(celsius.is_offset());

    CHECK(radian.is_base());
    CHECK(meter.is_base());
    CHECK(second.is_base());
    CHECK(celsius.is_base());

    CHECK(radian.has_same_dimensions_as(radian));
    CHECK(!radian.has_same_dimensions_as(meter));
    CHECK(meter.has_same_dimensions_as(meter));
    CHECK(!meter.has_same_dimensions_as(second));
    CHECK(centimeter.has_same_dimensions_as(meter));
    CHECK(meter.has_same_dimensions_as(centimeter));

    CHECK(radian.has_same_dimensions_as(radian.dimension()));
    CHECK(!radian.has_same_dimensions_as(meter.dimension()));
    CHECK(meter.has_same_dimensions_as(meter.dimension()));
    CHECK(!meter.has_same_dimensions_as(second.dimension()));
    CHECK(centimeter.has_same_dimensions_as(meter.dimension()));
    CHECK(meter.has_same_dimensions_as(centimeter.dimension()));
  }

  SECTION("Multiplication")
  {
    auto my_unit = meter;
    my_unit *= meter;

    CHECK(my_unit.scale() == Approx(1));
    CHECK(my_unit.offset() == Approx(0).scale(1));
    CHECK(!my_unit.is_offset());
    CHECK(!my_unit.is_base());

    auto meter_squared = meter * meter;

    CHECK(meter_squared.scale() == Approx(1));
    CHECK(meter_squared.offset() == Approx(0).scale(1));
    CHECK(!meter_squared.is_offset());
    CHECK(!meter_squared.is_base());

    auto kilometer = 1000 * meter;

    CHECK(kilometer.scale() == Approx(1000));
    CHECK(kilometer.offset() == Approx(0).scale(1));
    CHECK(!kilometer.is_offset());
    CHECK(!kilometer.is_base());

    CHECK_THROWS(celsius * meter);

    auto my_temperature = celsius * 2;
    CHECK(my_temperature.scale() == Approx(2));
    CHECK(my_temperature.offset() == Approx(273.));
    CHECK(my_temperature.is_offset());
    CHECK(!my_temperature.is_base());
  }

  SECTION("Division")
  {
    auto my_unit = meter;
    my_unit /= second;

    CHECK(my_unit.scale() == Approx(1));
    CHECK(my_unit.offset() == Approx(0).scale(1));
    CHECK(!my_unit.is_offset());
    CHECK(!my_unit.is_base());

    auto kilometer = 1000 * meter;
    auto minute = 60 * second;
    auto hour = 60 * minute;

    auto kph = kilometer / hour;

    CHECK(kph.scale() == Approx(1000. / 3600));
    CHECK(kph.offset() == Approx(0).scale(1));
    CHECK(!kph.is_offset());
    CHECK(!kph.is_base());

    auto megameter_per_hour = kph / 1000;

    CHECK(megameter_per_hour.scale() == Approx(1. / 3600));
    CHECK(megameter_per_hour.offset() == Approx(0).scale(1));
    CHECK(!megameter_per_hour.is_offset());
    CHECK(!megameter_per_hour.is_base());

    auto my_temperature = celsius / 2;

    CHECK(my_temperature.scale() == Approx(1. / 2));
    CHECK(my_temperature.offset() == Approx(273.));
    CHECK(my_temperature.is_offset());
    CHECK(!my_temperature.is_base());

    CHECK_THROWS(celsius / meter);
  }

  SECTION("Addition")
  {
    auto my_unit = meter;
    my_unit += 10;

    CHECK(my_unit.scale() == Approx(1));
    CHECK(my_unit.offset() == Approx(10));
    CHECK(my_unit.is_offset());
    CHECK(my_unit.is_base());

    auto meter_over_meter = meter + 1;

    CHECK(meter_over_meter.scale() == Approx(1));
    CHECK(meter_over_meter.offset() == Approx(1));
    CHECK(meter_over_meter.is_offset());
    CHECK(meter_over_meter.is_base());

    auto fahrenheit = (5./9)*celsius - (5./9)*32;

    CHECK(fahrenheit.scale() == Approx(5./9));
    CHECK(fahrenheit.offset() == Approx(273 - (5./9)*32));
    CHECK(fahrenheit.is_offset());
    CHECK(!fahrenheit.is_base());
  }

  SECTION("Subtraction")
  {
    auto my_unit = meter;
    my_unit -= 10;

    CHECK(my_unit.scale() == Approx(1));
    CHECK(my_unit.offset() == Approx(-10));
    CHECK(my_unit.is_offset());
    CHECK(my_unit.is_base());

    auto meter_over_meter = meter - 1;

    CHECK(meter_over_meter.scale() == Approx(1));
    CHECK(meter_over_meter.offset() == Approx(-1));
    CHECK(meter_over_meter.is_offset());
    CHECK(meter_over_meter.is_base());

    auto fahrenheit = 9 * celsius / 5 + 32;
    auto celsius_2= 5 * (fahrenheit-32)/ 9;

    CHECK(celsius_2.scale() == Approx(1));
    CHECK(celsius_2.offset() == Approx(273));
    CHECK(celsius_2.is_offset());
    CHECK(celsius_2.is_base());
  }

  SECTION("Powers")
  {
    auto my_unit = meter;
    my_unit ^= 2;

    CHECK( my_unit == meter*meter );

    my_unit = second^3;

    CHECK( my_unit == second*second*second);

    my_unit = meter^-1;

    CHECK( my_unit == radian/meter );
    CHECK( my_unit == meter/meter/meter );

    my_unit = meter^-2;

    CHECK( my_unit == radian/meter/meter );
    CHECK( my_unit == meter/meter/meter/meter );

    // WARNING: ^ has a lower precident than *!!
    my_unit = (meter^-2)*2;

    CHECK( my_unit.dimension() == (radian/meter/meter).dimension() );
    CHECK( my_unit.scale() == Approx(2) );

  }

  SECTION("ostream")
  {
    std::stringstream out;

    out << meter;
    CHECK( out.str() == "1 [A]^1 [B]^0 [C]^0" );
    out.str("");

    out << centimeter;
    CHECK( out.str() == "0.01 [A]^1 [B]^0 [C]^0" );
    out.str("");

    out << second;
    CHECK( out.str() == "1 [A]^0 [B]^1 [C]^0" );
    out.str("");

  }
  SECTION("Equality")
  {
    CHECK( meter != centimeter );
    CHECK( meter == meter );

    auto fahrenheit = (5./9)*celsius - (5./9)*32;
    CHECK( celsius == celsius );
    CHECK( fahrenheit != celsius );
    
  }
}
TEST_CASE("si_unit class")
{
using namespace unit_convert;
  si_unit<double> radian;
  si_unit<double> degree = M_PI*radian/180;
  si_unit<double> meter(si_dimension::name::Length);
  si_unit<double> second(si_dimension::name::Time);
  si_unit<double> ampere(si_dimension::name::ElectricalCurrent);
  si_unit<double> kelvin(si_dimension::name::Temperature);
  si_unit<double> mol(si_dimension::name::Amount);
  si_unit<double> candela(si_dimension::name::LuminousIntensity);

  CHECK( degree.scale() == Approx(M_PI/180) );
  CHECK( meter.scale() == Approx(1) );
}

TEST_CASE("si_base_unit class")
{
using namespace unit_convert;
  si_unit<double> radian;
  si_base_unit<si_dimension::name::Length,double> meter;
  si_base_unit<si_dimension::name::Time,double> second;
  si_base_unit<si_dimension::name::ElectricalCurrent,double> ampere;
  si_base_unit<si_dimension::name::Temperature,double> kelvin;
  si_base_unit<si_dimension::name::Amount,double> mol;
  si_base_unit<si_dimension::name::LuminousIntensity,double> candela;

  CHECK( radian.is_base() );
  CHECK( meter.is_base() );
  CHECK( second.is_base() );
  CHECK( ampere.is_base() );
  CHECK( kelvin.is_base() );
  CHECK( mol.is_base() );
  CHECK( candela.is_base() );

}
