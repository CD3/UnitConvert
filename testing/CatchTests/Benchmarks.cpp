#include "catch.hpp"
#include "Benchmark.hpp"

#include <UnitConvert.hpp>

TEST_CASE("Building unit from string", "[performance]")
{
  using namespace UnitConvert;

  UnitRegistry ureg;

  ureg.addBaseUnit<Dimension::Name::Length>("m");
  ureg.addBaseUnit<Dimension::Name::Mass>("g");
  ureg.addBaseUnit<Dimension::Name::Time>("s");
  ureg.addBaseUnit<Dimension::Name::Temperature>("K");
  ureg.addBaseUnit<Dimension::Name::Amount>("mol");
  ureg.addBaseUnit<Dimension::Name::Amount>("");

  ureg.addUnit("100 cm = 1 m");
  ureg.addUnit("1 in = 2.54 cm");
  ureg.addUnit("1 ft = 12 in");
  ureg.addUnit("1 J = 1 kg*m^2*s^-2");
  ureg.addUnit("1 W = 1 J/s");
  ureg.addUnit("1 cal = 4.184 J");


  Unit u = ureg.getUnit("");
  std::cout << "\n\n";

  SECTION("Named unit retrieval")
  {
    std::string name = "Named unit retrieval";
    BM::PerformanceBenchmark bm(name);
    BM::Benchmark meter;

    meter.run( [&](){
        u = ureg.getUnit("m");
        } );
    auto result = bm(meter);

    std::cout << name << "\n";
    std::cout << "Measurement:" << meter.get_measurement() << "\n";
    std::cout << "faster than baseline? " << result.is_faster_than_baseline << "\n";
    std::cout << "faster than minimum? " << result.is_faster_than_minimum << "\n";
    std::cout << "speedup over baseline: " << result.speedup_over_baseline.nominal() << "\n";
    std::cout << "speedup over minimum: " << result.speedup_over_minimum.nominal() << "\n";
  }

  SECTION("Named unit building")
  {
    std::string name = "Named unit building";
    BM::PerformanceBenchmark bm(name);
    BM::Benchmark meter;

    meter.run( [&](){
        u = ureg.makeUnit("m");
        } );
    auto result = bm(meter);

    std::cout << name << "\n";
    std::cout << "Measurement:" << meter.get_measurement() << "\n";
    std::cout << "faster than baseline? " << result.is_faster_than_baseline << "\n";
    std::cout << "faster than minimum? " << result.is_faster_than_minimum << "\n";
    std::cout << "speedup over baseline: " << result.speedup_over_baseline.nominal() << "\n";
    std::cout << "speedup over minimum: " << result.speedup_over_minimum.nominal() << "\n";
  }

  SECTION("Named unit with SI prefix building")
  {
    std::string name = "Named unit with SI prefix building";
    BM::PerformanceBenchmark bm(name);
    BM::Benchmark meter;

    meter.run( [&](){
        u = ureg.makeUnit("mm");
        } );
    auto result = bm(meter);

    std::cout << name << "\n";
    std::cout << "Measurement:" << meter.get_measurement() << "\n";
    std::cout << "faster than baseline? " << result.is_faster_than_baseline << "\n";
    std::cout << "faster than minimum? " << result.is_faster_than_minimum << "\n";
    std::cout << "speedup over baseline: " << result.speedup_over_baseline.nominal() << "\n";
    std::cout << "speedup over minimum: " << result.speedup_over_minimum.nominal() << "\n";
  }

  SECTION("Two term derived unit building")
  {
    std::string name = "Two term derived unit building";
    BM::PerformanceBenchmark bm(name);
    BM::Benchmark meter;

    meter.run( [&](){
        u = ureg.makeUnit("m/s");
        } );
    auto result = bm(meter);

    std::cout << name << "\n";
    std::cout << "Measurement:" << meter.get_measurement() << "\n";
    std::cout << "faster than baseline? " << result.is_faster_than_baseline << "\n";
    std::cout << "faster than minimum? " << result.is_faster_than_minimum << "\n";
    std::cout << "speedup over baseline: " << result.speedup_over_baseline.nominal() << "\n";
    std::cout << "speedup over minimum: " << result.speedup_over_minimum.nominal() << "\n";
  }

  SECTION("Many term derived unit building")
  {
    std::string name = "Many term derived unit building";
    BM::PerformanceBenchmark bm(name);
    BM::Benchmark meter;

    meter.run( [&](){
        u = ureg.makeUnit("kg * m^2 / s^2");
        } );
    auto result = bm(meter);

    std::cout << name << "\n";
    std::cout << "Measurement:" << meter.get_measurement() << "\n";
    std::cout << "faster than baseline? " << result.is_faster_than_baseline << "\n";
    std::cout << "faster than minimum? " << result.is_faster_than_minimum << "\n";
    std::cout << "speedup over baseline: " << result.speedup_over_baseline.nominal() << "\n";
    std::cout << "speedup over minimum: " << result.speedup_over_minimum.nominal() << "\n";
  }


}
