#include <sstream>
#include <catch.hpp>
#include <unit_convert/dimensions/basic_dimension.hpp>
#include <unit_convert/io.hpp>

TEST_CASE("basic_dimension<I> tests")
{
  using namespace unit_convert;
  SECTION("Size")
  {
    CHECK(sizeof(basic_dimension<1>) == sizeof(int));
    CHECK(sizeof(basic_dimension<2>) == 2 * sizeof(int));
    CHECK(sizeof(basic_dimension<1, char>) == sizeof(char));
    CHECK(sizeof(basic_dimension<2, char>) == 2 * sizeof(char));
  }

  SECTION("Default exponent type")
  {
    SECTION("2D system")
    {
      basic_dimension<2> length(0);
      basic_dimension<2> time(1);

      SECTION("output operator")
      {
        std::stringstream out;

        out << length;
        CHECK(out.str() == "[A]^1 [B]^0");
        out.str("");

        out << time;
        CHECK(out.str() == "[A]^0 [B]^1");
        out.str("");
      }
      SECTION("equality checks")
      {
        CHECK(length == length);
        CHECK(length == basic_dimension<2>(0));
        CHECK(length != time);
        CHECK(time != basic_dimension<2>(0));
      }

      SECTION("multiply")
      {
        basic_dimension<2> area = length * length;

        CHECK(area[0] == 2);
        CHECK(area[1] == 0);
      }
      SECTION("divide")
      {
        basic_dimension<2> velocity = length / time;

        CHECK(velocity[0] == 1);
        CHECK(velocity[1] == -1);

        auto dim1 = length / length;
        auto dim2 = time / time;

        CHECK(dim1[0] == 0);
        CHECK(dim1[1] == 0);

        CHECK(dim2[0] == 0);
        CHECK(dim2[1] == 0);

        CHECK(dim1 == dim2);
      }
      SECTION("exponent")
      {
        auto area = length^2;

        CHECK(area[0] == 2);
        CHECK(area[1] == 0);

        area ^= 2;

        CHECK(area[0] == 4);
        CHECK(area[1] == 0);
      }

      SECTION("Base vs Derived")
      {
        auto area = length * length;
        auto velocity = length / time;
        CHECK(length.is_base());
        CHECK(time.is_base());
        CHECK(basic_dimension<2>(0).is_base());
        CHECK(basic_dimension<2>(1).is_base());
        CHECK(!area.is_base());
        CHECK(!velocity.is_base());
        CHECK(basic_dimension<2>().is_base());

        CHECK(!length.is_derived());
        CHECK(!time.is_derived());
        CHECK(!basic_dimension<2>(0).is_derived());
        CHECK(!basic_dimension<2>(1).is_derived());
        CHECK(area.is_derived());
        CHECK(velocity.is_derived());
        CHECK(!basic_dimension<2>().is_derived());
      }
    }
  }
  SECTION("user-defined exp types")
  {
    basic_dimension<2, short> length(0);
    basic_dimension<2, short> time(1);

    auto velocity = length / time;

    CHECK(velocity[0] == 1);
    CHECK(velocity[1] == -1);
  }
}
