/**
 * @file binary_extension_benchmark.cpp
 * @brief Performance comparison between Givaro GFq and xgalois GF2X
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
#include <NTL/GF2X.h>
#include <NTL/GF2E.h>

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
std::vector<typename Givaro::GFq<ElementType>::Element>
GenerateRandomGivaroElements(const Givaro::GFq<ElementType> &field, size_t count,
                            uint32_t seed = 42) {
  std::mt19937 gen(seed);
  std::uniform_int_distribution<uint32_t> dis(
      1, static_cast<uint32_t>(field.cardinality() - 1)); // Start from 1 to avoid zero

  std::vector<typename Givaro::GFq<ElementType>::Element> elements;
  elements.reserve(count);

  for (size_t i = 0; i < count; ++i) {
    typename Givaro::GFq<ElementType>::Element elem;
    uint32_t val = dis(gen);
    // Ensure we never get zero
    if (val == 0) val = 1;
    field.init(elem, val);
    elements.push_back(elem);
  }

  return elements;
}

std::vector<uint32_t> GenerateRandomXgaloisElements(const xg::GF2XZECH &field,
                                                   size_t count,
                                                   uint32_t seed = 42) {
  std::mt19937 gen(seed);
  std::uniform_int_distribution<uint32_t> dis(1, field.Order() - 1); // Start from 1 to avoid zero

  std::vector<uint32_t> elements;
  elements.reserve(count);

  for (size_t i = 0; i < count; ++i) {
    uint32_t val = dis(gen);
    // Ensure we never get zero (additional safety check)
    if (val == 0) val = 1;
    elements.push_back(val);
  }

  return elements;
}

std::vector<NTL::GF2E> GenerateRandomNTLElements(size_t count, uint32_t seed = 42) {
  std::mt19937 gen(seed);
  std::uniform_int_distribution<uint32_t> dis(1, (1 << NTL::GF2E::degree()) - 1); // Start from 1 to avoid zero

  std::vector<NTL::GF2E> elements;
  elements.reserve(count);

  for (size_t i = 0; i < count; ++i) {
    NTL::GF2X poly;
    uint32_t val = dis(gen);
    // Ensure we never get zero polynomial
    if (val == 0) val = 1;

    // Convert val to polynomial representation
    for (int j = 0; j < NTL::GF2E::degree(); ++j) {
      if ((val >> j) & 1) {
        NTL::SetCoeff(poly, j, 1);
      }
    }

    NTL::GF2E elem;
    NTL::conv(elem, poly);

    // Double-check that we didn't create zero element
    if (NTL::IsZero(elem)) {
      NTL::SetCoeff(poly, 0, 1); // Set constant term to ensure non-zero
      NTL::conv(elem, poly);
    }

    elements.push_back(elem);
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

// Helper function to get NTL irreducible polynomial for each field size
NTL::GF2X GetNTLIrreduciblePoly(uint8_t m) {
  NTL::GF2X poly;
  switch (m) {
    case 4:
      // x^4 + x + 1
      NTL::SetCoeff(poly, 4); NTL::SetCoeff(poly, 1); NTL::SetCoeff(poly, 0);
      break;
    case 8:
      // x^8 + x^4 + x^3 + x^2 + 1
      NTL::SetCoeff(poly, 8); NTL::SetCoeff(poly, 4); NTL::SetCoeff(poly, 3); NTL::SetCoeff(poly, 2); NTL::SetCoeff(poly, 0);
      break;
    case 12:
      // x^12 + x^6 + x^4 + x + 1
      NTL::SetCoeff(poly, 12); NTL::SetCoeff(poly, 6); NTL::SetCoeff(poly, 4); NTL::SetCoeff(poly, 1); NTL::SetCoeff(poly, 0);
      break;
    case 16:
      // x^16 + x^12 + x^3 + x + 1
      NTL::SetCoeff(poly, 16); NTL::SetCoeff(poly, 12); NTL::SetCoeff(poly, 3); NTL::SetCoeff(poly, 1); NTL::SetCoeff(poly, 0);
      break;
    case 20:
      // x^20 + x^3 + 1
      NTL::SetCoeff(poly, 20); NTL::SetCoeff(poly, 3); NTL::SetCoeff(poly, 0);
      break;
    default:
      // Default irreducible polynomial
      NTL::SetCoeff(poly, m); NTL::SetCoeff(poly, 1); NTL::SetCoeff(poly, 0);
      break;
  }
  return poly;
}

// Helper function to get Givaro polynomial representation
std::vector<int> GetGivaroIrreduciblePoly(uint8_t m) {
  std::vector<int> poly;
  switch (m) {
    case 4:
      // x^4 + x + 1
      poly = {1, 1, 0, 0, 1}; // coefficients from x^0 to x^4
      break;
    case 8:
      // x^8 + x^4 + x^3 + x^2 + 1
      poly = {1, 0, 1, 1, 1, 0, 0, 0, 1}; // coefficients from x^0 to x^8
      break;
    case 12:
      // x^12 + x^6 + x^4 + x + 1
      poly = {1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1}; // coefficients from x^0 to x^12
      break;
    case 16:
      // x^16 + x^12 + x^3 + x + 1
      poly = {1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1}; // coefficients from x^0 to x^16
      break;
    case 20:
      // x^20 + x^3 + 1
      poly = {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}; // coefficients from x^0 to x^20
      break;
    default:
      // Default: x^m + x + 1
      poly.resize(m + 1, 0);
      poly[0] = 1; // constant term
      poly[1] = 1; // x term
      poly[m] = 1; // x^m term
      break;
  }
  return poly;
}

//------------------------------------------------------------------------------
// Givaro GFq Benchmarks
//------------------------------------------------------------------------------

static void BM_Givaro_Addition(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  std::vector<int> poly = GetGivaroIrreduciblePoly(m);
  Givaro::GFq<int64_t> field(2, m, poly);

  auto elements = GenerateRandomGivaroElements(field, 10000);
  size_t idx = 0;

  for (auto _ : state) {
    Givaro::GFq<int64_t>::Element result;
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
  std::vector<int> poly = GetGivaroIrreduciblePoly(m);
  Givaro::GFq<int64_t> field(2, m, poly);

  auto elements = GenerateRandomGivaroElements(field, 10000);
  size_t idx = 0;

  for (auto _ : state) {
    Givaro::GFq<int64_t>::Element result;
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
  std::vector<int> poly = GetGivaroIrreduciblePoly(m);
  Givaro::GFq<int64_t> field(2, m, poly);

  auto elements = GenerateRandomGivaroElements(field, 10000);
  size_t idx = 0;

  for (auto _ : state) {
    Givaro::GFq<int64_t>::Element result;
    auto divisor = elements[(idx + 1) % elements.size()];
    // Additional safety check to ensure divisor is not zero
    if (field.isZero(divisor)) {
      field.init(divisor, 1); // Set to 1 if somehow zero
    }
    field.div(result, elements[idx % elements.size()], divisor);
    benchmark::DoNotOptimize(result);
    idx++;
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldOrder"] = field.cardinality();
}

static void BM_Givaro_Inversion(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  std::vector<int> poly = GetGivaroIrreduciblePoly(m);
  Givaro::GFq<int64_t> field(2, m, poly);

  auto elements = GenerateRandomGivaroElements(field, 10000);
  size_t idx = 0;

  for (auto _ : state) {
    Givaro::GFq<int64_t>::Element result;
    auto elem = elements[idx % elements.size()];
    // Additional safety check to ensure element is not zero
    if (field.isZero(elem)) {
      field.init(elem, 1); // Set to 1 if somehow zero
    }
    field.inv(result, elem);
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

  auto elements = GenerateRandomXgaloisElements(field, 10000);
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

  auto elements = GenerateRandomXgaloisElements(field, 10000);
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

  auto elements = GenerateRandomXgaloisElements(field, 10000);
  size_t idx = 0;

  for (auto _ : state) {
    uint32_t divisor = elements[(idx + 1) % elements.size()];
    // Additional safety check to ensure divisor is not zero
    if (divisor == 0) {
      divisor = 1; // Set to 1 if somehow zero
    }
    uint32_t result = field.Div(elements[idx % elements.size()], divisor);
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

  auto elements = GenerateRandomXgaloisElements(field, 10000);
  size_t idx = 0;

  for (auto _ : state) {
    uint32_t elem = elements[idx % elements.size()];
    // Additional safety check to ensure element is not zero
    if (elem == 0) {
      elem = 1; // Set to 1 if somehow zero
    }
    uint32_t result = field.Inv(elem);
    benchmark::DoNotOptimize(result);
    idx++;
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldOrder"] = field.Order();
}

//------------------------------------------------------------------------------
// NTL GF2E Benchmarks
//------------------------------------------------------------------------------

static void BM_NTL_Addition(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  NTL::GF2X poly = GetNTLIrreduciblePoly(m);
  NTL::GF2E::init(poly);

  auto elements = GenerateRandomNTLElements(10000);
  size_t idx = 0;

  for (auto _ : state) {
    NTL::GF2E result = elements[idx % elements.size()] + elements[(idx + 1) % elements.size()];
    benchmark::DoNotOptimize(result);
    idx++;
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldOrder"] = 1UL << m;
}

static void BM_NTL_Multiplication(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  NTL::GF2X poly = GetNTLIrreduciblePoly(m);
  NTL::GF2E::init(poly);

  auto elements = GenerateRandomNTLElements(10000);
  size_t idx = 0;

  for (auto _ : state) {
    NTL::GF2E result = elements[idx % elements.size()] * elements[(idx + 1) % elements.size()];
    benchmark::DoNotOptimize(result);
    idx++;
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldOrder"] = 1UL << m;
}

static void BM_NTL_Division(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  NTL::GF2X poly = GetNTLIrreduciblePoly(m);
  NTL::GF2E::init(poly);

  auto elements = GenerateRandomNTLElements(10000);
  size_t idx = 0;

  for (auto _ : state) {
    NTL::GF2E divisor = elements[(idx + 1) % elements.size()];
    // Additional safety check to ensure divisor is not zero
    if (NTL::IsZero(divisor)) {
      divisor = NTL::to_GF2E(1); // Set to 1 if somehow zero
    }
    NTL::GF2E result = elements[idx % elements.size()] / divisor;
    benchmark::DoNotOptimize(result);
    idx++;
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldOrder"] = 1UL << m;
}

static void BM_NTL_Inversion(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  NTL::GF2X poly = GetNTLIrreduciblePoly(m);
  NTL::GF2E::init(poly);

  auto elements = GenerateRandomNTLElements(10000);
  size_t idx = 0;

  for (auto _ : state) {
    NTL::GF2E elem = elements[idx % elements.size()];
    // Additional safety check to ensure element is not zero
    if (NTL::IsZero(elem)) {
      elem = NTL::to_GF2E(1); // Set to 1 if somehow zero
    }
    NTL::GF2E result = NTL::inv(elem);
    benchmark::DoNotOptimize(result);
    idx++;
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldOrder"] = 1UL << m;
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

BENCHMARK(BM_NTL_Addition)
    ->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(20)
    ->Unit(benchmark::kNanosecond);

BENCHMARK(BM_NTL_Multiplication)
    ->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(20)
    ->Unit(benchmark::kNanosecond);

BENCHMARK(BM_NTL_Division)
    ->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(20)
    ->Unit(benchmark::kNanosecond);

BENCHMARK(BM_NTL_Inversion)
    ->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(20)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_MAIN();
