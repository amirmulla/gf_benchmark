/**
 * @file binary_extension_benchmark.cpp
 * @brief Performance comparison between Givaro GFq and xgalois GF2XZECH
 * Benchmarks GF(2^m) operations for both implementations
 */

#include <benchmark/benchmark.h>
#include <chrono>
#include <memory>
#include <random>
#include <stdexcept>
#include <sys/resource.h>
#include <unistd.h>
#include <vector>

#include <givaro/gfq.h>
#include <xgalois/field/gf_binary.hpp>

using namespace Givaro;

//------------------------------------------------------------------------------
// Memory Usage Utilities
//------------------------------------------------------------------------------

struct MemoryUsage {
  size_t peak_rss_kb;
  size_t current_rss_kb;

  MemoryUsage() : peak_rss_kb(0), current_rss_kb(0) {}
};

MemoryUsage GetMemoryUsage() {
  MemoryUsage usage;

  // Get peak memory usage
  struct rusage rusage_data;
  if (getrusage(RUSAGE_SELF, &rusage_data) == 0) {
    usage.peak_rss_kb = rusage_data.ru_maxrss / 1024; // Convert to KB on macOS
  }

  // Get current memory usage using ps command (macOS compatible)
  pid_t pid = getpid();
  char command[256];
  snprintf(command, sizeof(command), "ps -o rss= -p %d", pid);

  FILE *pipe = popen(command, "r");
  if (pipe) {
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), pipe)) {
      usage.current_rss_kb = atol(buffer);
    }
    pclose(pipe);
  }

  return usage;
}
//------------------------------------------------------------------------------
// Field Sizes for Testing
//------------------------------------------------------------------------------

// Field degrees to test
const std::vector<uint8_t> FIELD_DEGREES = {4, 8, 12, 16, 20};

//------------------------------------------------------------------------------
// Helper Functions
//------------------------------------------------------------------------------

template <typename ElementType>
std::vector<typename GFq<ElementType>::Element>
GenerateRandomGivaroElements(const GFq<ElementType> &field, size_t count,
                            uint32_t seed = 42) {
  std::mt19937 gen(seed);
  std::uniform_int_distribution<uint32_t> dis(
      1, static_cast<uint32_t>(field.cardinality() - 1));

  std::vector<typename GFq<ElementType>::Element> elements;
  elements.reserve(count);

  for (size_t i = 0; i < count; ++i) {
    typename GFq<ElementType>::Element elem;
    field.init(elem, dis(gen));
    elements.push_back(elem);
  }

  return elements;
}

std::vector<uint32_t> GenerateRandomXgaloisElements(const xg::GF2XZECH &field,
                                                   size_t count,
                                                   uint32_t seed = 42) {
  std::mt19937 gen(seed);
  std::uniform_int_distribution<uint32_t> dis(0, field.Order() - 2); // Multiplicative group size = Order - 1

  std::vector<uint32_t> elements;
  elements.reserve(count);

  for (size_t i = 0; i < count; ++i) {
    elements.push_back(dis(gen));
  }

  return elements;
}

// Helper function to get irreducible polynomial for each field size
std::string GetIrreduciblePoly(uint8_t m) {
  switch (m) {
    case 4: return "x^4 + x + 1";
    case 8: return "x^8 + x^4 + x^3 + x^2 + 1";
    case 12: return "x^12 + x^6 + x^4 + x + 1";
    case 16: return "x^16 + x^12 + x^3 + x + 1";
    case 20: return "x^20 + x^3 + 1";
    default: return ""; // Let the library choose
  }
}

//------------------------------------------------------------------------------
// Givaro GFq Benchmarks
//------------------------------------------------------------------------------

static void BM_Givaro_Addition(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements = GenerateRandomGivaroElements(field, 1000);
  size_t idx = 0;

  for (auto _ : state) {
    GFq<int64_t>::Element result;
    field.add(result, elements[idx % elements.size()],
              elements[(idx + 1) % elements.size()]);
    benchmark::DoNotOptimize(result);
    idx++;
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldOrder"] = field.cardinality();
}

static void BM_Givaro_Multiplication(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements = GenerateRandomGivaroElements(field, 1000);
  size_t idx = 0;

  for (auto _ : state) {
    GFq<int64_t>::Element result;
    field.mul(result, elements[idx % elements.size()],
              elements[(idx + 1) % elements.size()]);
    benchmark::DoNotOptimize(result);
    idx++;
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldOrder"] = field.cardinality();
}

static void BM_Givaro_Division(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements = GenerateRandomGivaroElements(field, 1000);
  size_t idx = 0;

  for (auto _ : state) {
    GFq<int64_t>::Element result;
    field.div(result, elements[idx % elements.size()],
              elements[(idx + 1) % elements.size()]);
    benchmark::DoNotOptimize(result);
    idx++;
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldOrder"] = field.cardinality();
}

static void BM_Givaro_Inversion(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements = GenerateRandomGivaroElements(field, 1000);
  size_t idx = 0;

  for (auto _ : state) {
    GFq<int64_t>::Element result;
    field.inv(result, elements[idx % elements.size()]);
    benchmark::DoNotOptimize(result);
    idx++;
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldOrder"] = field.cardinality();
}

//------------------------------------------------------------------------------
// xgalois GF2XZECH Benchmarks
//------------------------------------------------------------------------------

static void BM_Xgalois_Addition(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  xg::GF2XZECH field(m, "log", GetIrreduciblePoly(m));

  auto elements = GenerateRandomXgaloisElements(field, 1000);
  size_t idx = 0;

  for (auto _ : state) {
    uint32_t result = field.Add(elements[idx % elements.size()],
                               elements[(idx + 1) % elements.size()]);
    benchmark::DoNotOptimize(result);
    idx++;
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldOrder"] = field.Order();
}

static void BM_Xgalois_Multiplication(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  xg::GF2XZECH field(m, "log", GetIrreduciblePoly(m));

  auto elements = GenerateRandomXgaloisElements(field, 1000);
  size_t idx = 0;

  for (auto _ : state) {
    uint32_t result = field.Mul(elements[idx % elements.size()],
                               elements[(idx + 1) % elements.size()]);
    benchmark::DoNotOptimize(result);
    idx++;
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldOrder"] = field.Order();
}

static void BM_Xgalois_Division(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  xg::GF2XZECH field(m, "log", GetIrreduciblePoly(m));

  auto elements = GenerateRandomXgaloisElements(field, 1000);
  size_t idx = 0;

  for (auto _ : state) {
    uint32_t result = field.Div(elements[idx % elements.size()],
                               elements[(idx + 1) % elements.size()]);
    benchmark::DoNotOptimize(result);
    idx++;
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldOrder"] = field.Order();
}

static void BM_Xgalois_Inversion(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  xg::GF2XZECH field(m, "log", GetIrreduciblePoly(m));

  auto elements = GenerateRandomXgaloisElements(field, 1000);
  size_t idx = 0;

  for (auto _ : state) {
    uint32_t result = field.Inv(elements[idx % elements.size()]);
    benchmark::DoNotOptimize(result);
    idx++;
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldOrder"] = field.Order();
}

//------------------------------------------------------------------------------
// Benchmark Registration
//------------------------------------------------------------------------------

// Register benchmarks for all field sizes {4,8,12,16,20}
BENCHMARK(BM_Givaro_Addition)
    ->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(20)
    ->Unit(benchmark::kNanosecond);

BENCHMARK(BM_Givaro_Multiplication)
    ->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(20)
    ->Unit(benchmark::kNanosecond);

BENCHMARK(BM_Givaro_Division)
    ->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(20)
    ->Unit(benchmark::kNanosecond);

BENCHMARK(BM_Givaro_Inversion)
    ->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(20)
    ->Unit(benchmark::kNanosecond);

BENCHMARK(BM_Xgalois_Addition)
    ->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(20)
    ->Unit(benchmark::kNanosecond);

BENCHMARK(BM_Xgalois_Multiplication)
    ->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(20)
    ->Unit(benchmark::kNanosecond);

BENCHMARK(BM_Xgalois_Division)
    ->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(20)
    ->Unit(benchmark::kNanosecond);

BENCHMARK(BM_Xgalois_Inversion)
    ->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(20)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_MAIN();
