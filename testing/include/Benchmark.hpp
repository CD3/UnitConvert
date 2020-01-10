#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/error_of.hpp>
#include <boost/accumulators/statistics/error_of_mean.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/sum.hpp>
#include <boost/optional.hpp>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>

namespace BM
{
/**
 * A class getting high resolution time durations.
 */
struct high_resolution_timer {
  high_resolution_timer() : m_start_time(take_time_stamp()) {}
  void
      /**
       * restart the timer
       */
      restart()
  {
    m_start_time = take_time_stamp();
  }

  /**
   * return the time, in seconds, since the timer was started.
   */
  double elapsed() const
  {
    return static_cast<double>(elapsed_nanoseconds()) * 1e-9;
  }

  /**
   * return the time, in nanoseconds, since the timer was
   * started.
   */
  std::uint64_t elapsed_nanoseconds() const
  {
    return take_time_stamp() - m_start_time;
  }

 private:
  std::uint64_t m_start_time;
  static std::uint64_t take_time_stamp()
  {
    // need to cast results of high_resolution_clock::now().time_since_epoch()
    // to nanoseconds in case system's minimum time resultion is different.
    // VS2012 for example does not return nanoseconds.
    return std::uint64_t(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count());
  }
};

using namespace boost::accumulators;
/**
 * A class for accumulating timing statistice for benchmarks.
 */
typedef accumulator_set<
    double, stats<tag::count, tag::min, tag::max, tag::sum, tag::mean,
                  tag::variance, tag::error_of<tag::mean>>>
    timer_statistics;

/**
 * A class representing a measurement which consists of a nominal value with
 * uncertainty.
 */
template <typename T>
class Measurement
{
 public:
  T m_nominal;
  T m_uncertainty;

  Measurement() = default;
  Measurement(const Measurement&) = default;
  Measurement(T n, T u) : m_nominal(n), m_uncertainty(u) {}
  T nominal() const { return m_nominal; }
  T uncertainty() const { return m_uncertainty; }
  T upper() const { return m_nominal + m_uncertainty; }
  T lower() const { return m_nominal - m_uncertainty; }
  double relative_uncertainty() const { return m_uncertainty / m_nominal; }
  bool operator==(const Measurement& other) const
  {
    // two measurments are "consistent" if
    //
    // z^\prime = \frac{ |m_1 - m_2| }{ \sqrt{\Delta m_1^2 + \Delta m_2^2} } \ge
    // 2
    //
    T diff = std::abs(this->m_nominal - other.m_nominal);
    T unc = sqrt(this->m_uncertainty * this->m_uncertainty +
                 other.m_uncertainty * other.m_uncertainty);

    // don't want to inadvertently divide by zero.
    return diff <= 2 * unc;
  }

  bool operator<(const Measurement& other) const
  {
    // this measurment is less than the other if
    // 1. the two measurements are not consistent
    // 2. the nominal value of this is less than the other
    return !(*this == other) && this->m_nominal < other.m_nominal;
  }

  bool operator!=(const Measurement& other) const { return !(*this == other); }
  bool operator>(const Measurement& other) const
  {
    // this measurment is greater than the other if
    // 1. the two measurements are not consistent
    // 2. the nominal value of this is greater than the other
    return !(*this == other) && this->m_nominal > other.m_nominal;
  }

  bool operator<=(const Measurement& other) const
  {
    return (*this == other) || (*this < other);
  }

  bool operator>=(const Measurement& other) const
  {
    return (*this == other) || (*this > other);
  }

  template <typename TT>
  friend std::ostream& operator<<(std::ostream& os, const Measurement<TT>& uq)
  {
    os << uq.nominal() << " +/- " << uq.uncertainty();
    return os;
  }
};

struct Benchmark {
  high_resolution_timer m_timer;
  timer_statistics m_accum;
  double m_tolerance;
  size_t m_max_iters;
  size_t m_min_iters;
  size_t m_burn_iters;
  std::uint64_t m_max_runtime;  // max runtime in nanoseconds

  Benchmark()
      : m_tolerance(0.01),  // 1% error
        m_max_iters(100000),
        m_min_iters(10),
        m_burn_iters(10),
        m_max_runtime(100000000000ULL)
  {
  }

  /**
   * Add a duration to the Benchmark statistics
   */
  void add_time(std::uint64_t duration) { m_accum(duration); }
  /**
   * Executes a callable, returns the runtime in nanoseconds, and adds it to the
   * Benchmark statistics.
   */
  template <typename F>
  void record(const F& f)
  {
    m_timer.restart();
    f();
    auto duration = m_timer.elapsed_nanoseconds();
    add_time(duration);
  }

  /**
   * Executes a callable multiple times and accumulates statistics on the
   * runtime. The number of times the executable runs is determined by the
   * variance in run times. The callable will be ran until the relative variance
   * is less than the value stored in m_tolerance.
   */
  template <typename F>
  void run(const F& f)
  {
    high_resolution_timer run_timer;
    // allow algorithm to "burn in"
    reset();
    while (count(m_accum) < m_burn_iters &&
           run_timer.elapsed_nanoseconds() < m_max_runtime) {
      record(f);
    }

    reset();
    run_timer.restart();
    while (count(m_accum) < m_max_iters &&
           run_timer.elapsed_nanoseconds() < m_max_runtime &&
           (error_of<tag::mean>(m_accum) / mean(m_accum) > m_tolerance ||
            count(m_accum) < m_min_iters)) {
      record(f);
    }
    if (error_of<tag::mean>(m_accum) / mean(m_accum) > m_tolerance)
      std::cerr << "WARNING: relative error exceeds tolerance. Timing loop was "
                   "exited early for some reason.\n";
  }

  /**
   * Resets all benchmark statistics.
   */
  void reset() { m_accum = timer_statistics(); }
  /**
   * Returns the runtime measurement (in nanoseconds) compiled by the benchmark.
   * This will be a nominal value with an uncertainty that represents the error
   * of the mean.
   */
  Measurement<double> get_measurement() const
  {
    return Measurement<double>(mean(m_accum), error_of<tag::mean>(m_accum));
  }

  double get_longest_runtime() const { return max(m_accum); }
  double get_shortest_runtime() const { return min(m_accum); }
  double get_average_runtime() const { return mean(m_accum); }
  double get_stddev_runtime() const { return sqrt(variance(m_accum)); }
  double get_total_runtime() const { return sum(m_accum); }
  size_t get_iterations() const { return count(m_accum); }
  /**
   * Print the benchmarks current statistics.
   */
  void print_results(std::ostream& os) const
  {
    os << "iterations / max iterations: " << get_iterations() << " / "
       << m_max_iters << " (" << 100. * get_iterations() / m_max_iters
       << "%) \n";
    os << "total time / max allowed total runtime (ns): " << get_total_runtime()
       << " / " << m_max_runtime << " ("
       << 100 * get_total_runtime() / m_max_runtime << "%)\n";
    os << "min - max (ns): " << get_shortest_runtime() << " - "
       << get_longest_runtime() << "\n";
    os << "MEASUREMENT (ns): " << get_measurement() << "\n";
  }
};

// function to escape the optimizer
static void escape(void* p)
{
#if defined(_MSC_VER)
// TODO: find an implementation for VS
#else
  asm volatile("" : : "g"(p) : "memory");
#endif
}

static void clobber()
{
#if defined(_MSC_VER)
// TODO: find an implementation for VS
#else
  asm volatile("" : : : "memory");
#endif
}

/**
 * A class for comparing benchmarks
 */
class BenchmarkComparison
{
};

/**
 * A class for testing performance improvements.
 */
class PerformanceBenchmark
{
 protected:
  std::string m_Name;
  boost::optional<Measurement<double>> m_Baseline;
  boost::optional<Measurement<double>> m_Minimum;

 public:
  struct Result {
    bool is_faster_than_baseline;
    Measurement<double> speedup_over_baseline;
    bool is_faster_than_minimum;
    Measurement<double> speedup_over_minimum;
  };

  static std::string getStorageFilename(std::string name)
  {
    return "PerformanceBenchmark-" + name + ".txt";
  }

  PerformanceBenchmark(const std::string& Name = "Default") : m_Name(Name)
  {
    std::string store = getStorageFilename(m_Name);

    std::ifstream in;
    in.open(store);
    if (in.is_open()) {
      double n, u;
      in >> n;
      in >> u;
      m_Baseline = Measurement<double>(n, u);
      in >> n;
      in >> u;
      m_Minimum = Measurement<double>(n, u);
    }
  }

  ~PerformanceBenchmark()
  {
    std::string store = getStorageFilename(m_Name);

    try {
      std::ofstream out;
      out.open(store);
      if (out.is_open()) {
        out << m_Baseline->nominal() << "\n";
        out << m_Baseline->uncertainty() << "\n";
        out << m_Minimum->nominal() << "\n";
        out << m_Minimum->uncertainty() << "\n";
      } else {
        std::cout << "ERROR: could not open " << store
                  << " to write benchmark data." << std::endl;
      }
      out.close();
    } catch (...) {
    }
  }

  Result operator()(const Benchmark& bench)
  {
    if (!m_Baseline) {
      m_Baseline = bench.get_measurement();
    }

    if (!m_Minimum) {
      m_Minimum = bench.get_measurement();
    }

    Measurement<double> measurement = bench.get_measurement();

    Result res;

    res.is_faster_than_baseline = measurement < m_Baseline;
    res.is_faster_than_minimum = measurement < m_Minimum;

    double s, dsn, dsd;

    s = m_Baseline->nominal() / measurement.nominal();
    dsn = m_Baseline->upper() / measurement.nominal() - s;
    dsd = m_Baseline->nominal() / measurement.upper() - s;
    res.speedup_over_baseline =
        Measurement<double>(s, sqrt(dsn * dsn + dsd * dsd));

    s = m_Minimum->nominal() / measurement.nominal();
    dsn = m_Minimum->upper() / measurement.nominal() - s;
    dsd = m_Minimum->nominal() / measurement.upper() - s;
    res.speedup_over_minimum =
        Measurement<double>(s, sqrt(dsn * dsn + dsd * dsd));

    if (measurement < m_Minimum.get()) {
      m_Minimum = measurement;
    }

    return res;
  }

  bool have_baseline() { return static_cast<bool>(m_Baseline); }
  Measurement<double> get_baseline()
  {
    if (m_Baseline) {
      return m_Baseline.get();
    } else {
      return Measurement<double>(0, 0);
    }
  }

  bool have_minimum() { return static_cast<bool>(m_Minimum); }
  Measurement<double> get_minimum()
  {
    if (m_Minimum) {
      return m_Minimum.get();
    } else {
      return Measurement<double>(0, 0);
    }
  }
};

}  // namespace BM
