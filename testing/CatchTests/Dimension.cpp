#include <UnitConvert/Dimension.hpp>
#include <catch.hpp>
#include <sstream>

using namespace UnitConvert::v1;
TEST_CASE("Dimension class tests")
{
  SECTION("Default exponent type")
  {
    SECTION("2D system")
    {
      Dimension<2> length(0);
      Dimension<2> time(1);

      SECTION("output operator")
      {
        std::stringstream out;

        out << length;
        CHECK(out.str() == " [0]^1 [1]^0");
        out.str("");

        out << time;
        CHECK(out.str() == " [0]^0 [1]^1");
        out.str("");
      }
      SECTION("equality checks")
      {
        CHECK(length == length);
        CHECK(length == Dimension<2>(0));
        CHECK(length != time);
        CHECK(time != Dimension<2>(0));
      }

      SECTION("multiply")
      {
        Dimension<2> area = length * length;

        CHECK(area[0] == 2);
        CHECK(area[1] == 0);
      }
      SECTION("divide")
      {
        Dimension<2> velocity= length / time;

        CHECK(velocity[0] == 1);
        CHECK(velocity[1] == -1);

        auto dim1 = length/length;
        auto dim2 = time/time;

        CHECK(dim1[0] == 0);
        CHECK(dim1[1] == 0);

        CHECK(dim2[0] == 0);
        CHECK(dim2[1] == 0);

        CHECK(dim1 == dim2);
      }

      SECTION("Base vs Derived")
      {
        auto area = length*length;
        auto velocity = length/time;
        CHECK( length.is_base() );
        CHECK( time.is_base() );
        CHECK( Dimension<2>(0).is_base() );
        CHECK( Dimension<2>(1).is_base() );
        CHECK(!area.is_base() );
        CHECK(!velocity.is_base() );
        CHECK(!Dimension<2>().is_base() );

        CHECK(!length.is_derived() );
        CHECK(!time.is_derived() );
        CHECK(!Dimension<2>(0).is_derived() );
        CHECK(!Dimension<2>(1).is_derived() );
        CHECK( area.is_derived() );
        CHECK( velocity.is_derived() );
        CHECK( Dimension<2>().is_derived() );
      }
      
    }
  }
  SECTION("user-defined exp types")
  {
    Dimension<2, short> length(0);
    Dimension<2, short> time(1);

    auto velocity = length / time;

    CHECK(velocity[0] == 1);
    CHECK(velocity[1] == -1);
  }
}
