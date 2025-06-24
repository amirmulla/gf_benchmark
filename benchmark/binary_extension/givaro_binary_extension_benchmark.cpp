/**
 * @file givaro_binary_extension_benchmark.cpp
 * @brief Comprehensive benchmark for Givaro's binary extension field
 * implementation Benchmarks GF(2^m) operations using Givaro's GFq
 * implementation
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

// Small fields - Common small extension degrees
const std::vector<uint8_t> SMALL_FIELD_DEGREES = {2, 3, 4, 5, 6, 7, 8};

// Medium fields - Practical sizes for applications
const std::vector<uint8_t> MEDIUM_FIELD_DEGREES = {9,  10, 11, 12,
                                                   13, 14, 15, 16};

// Large fields - Note: Some implementations may be memory intensive
const std::vector<uint8_t> LARGE_FIELD_DEGREES = {17, 18, 19, 20};

//------------------------------------------------------------------------------
// Helper Functions
//------------------------------------------------------------------------------

template <typename ElementType>
std::vector<typename GFq<ElementType>::Element>
GenerateRandomElements(const GFq<ElementType> &field, size_t count,
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

//------------------------------------------------------------------------------
// Small Field Benchmarks (GF(2^m) where m = 2-8)
//------------------------------------------------------------------------------

static void BM_Givaro_GF2X_Small_Addition(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements = GenerateRandomElements(field, 1000);
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

static void BM_Givaro_GF2X_Small_Multiplication(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements = GenerateRandomElements(field, 1000);
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

static void BM_Givaro_GF2X_Small_Division(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements = GenerateRandomElements(field, 1000);
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

static void BM_Givaro_GF2X_Small_Inversion(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements = GenerateRandomElements(field, 1000);
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
// Medium Field Benchmarks (GF(2^m) where m = 9-16)
//------------------------------------------------------------------------------

static void BM_Givaro_GF2X_Medium_Addition(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements = GenerateRandomElements(field, 1000);
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

static void BM_Givaro_GF2X_Medium_Multiplication(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements = GenerateRandomElements(field, 1000);
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

static void BM_Givaro_GF2X_Medium_Division(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements = GenerateRandomElements(field, 1000);
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

static void BM_Givaro_GF2X_Medium_Inversion(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements = GenerateRandomElements(field, 1000);
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
// Large Field Benchmarks (GF(2^m) where m = 17-20)
//------------------------------------------------------------------------------

static void BM_Givaro_GF2X_Large_Addition(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements =
      GenerateRandomElements(field, 500); // Smaller count for large fields
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

static void BM_Givaro_GF2X_Large_Multiplication(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements = GenerateRandomElements(field, 500);
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

static void BM_Givaro_GF2X_Large_Division(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements = GenerateRandomElements(field, 500);
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

static void BM_Givaro_GF2X_Large_Inversion(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));
  GFq<int64_t> field(2, m);

  auto elements = GenerateRandomElements(field, 500);
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
// Field Construction Benchmarks
//------------------------------------------------------------------------------

static void BM_Givaro_GF2X_Field_Construction_Small(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));

  for (auto _ : state) {
    GFq<int64_t> field(2, m);
    benchmark::DoNotOptimize(field);
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldDegree"] = m;
}

static void BM_Givaro_GF2X_Field_Construction_Medium(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));

  for (auto _ : state) {
    GFq<int64_t> field(2, m);
    benchmark::DoNotOptimize(field);
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldDegree"] = m;
}

static void BM_Givaro_GF2X_Field_Construction_Large(benchmark::State &state) {
  uint8_t m = static_cast<uint8_t>(state.range(0));

  for (auto _ : state) {
    GFq<int64_t> field(2, m);
    benchmark::DoNotOptimize(field);
  }

  MemoryUsage mem_end = GetMemoryUsage();
  state.counters["MemoryPeak_KB"] = mem_end.peak_rss_kb;
  state.counters["FieldDegree"] = m;
}

//------------------------------------------------------------------------------
// Benchmark Registration
//------------------------------------------------------------------------------

// Small field benchmarks
BENCHMARK(BM_Givaro_GF2X_Small_Addition)
    ->DenseRange(2, 8, 1)
    ->Unit(benchmark::kNanosecond);
BENCHMARK(BM_Givaro_GF2X_Small_Multiplication)
    ->DenseRange(2, 8, 1)
    ->Unit(benchmark::kNanosecond);
BENCHMARK(BM_Givaro_GF2X_Small_Division)
    ->DenseRange(2, 8, 1)
    ->Unit(benchmark::kNanosecond);
BENCHMARK(BM_Givaro_GF2X_Small_Inversion)
    ->DenseRange(2, 8, 1)
    ->Unit(benchmark::kNanosecond);

// Medium field benchmarks
BENCHMARK(BM_Givaro_GF2X_Medium_Addition)
    ->DenseRange(9, 16, 1)
    ->Unit(benchmark::kNanosecond);
BENCHMARK(BM_Givaro_GF2X_Medium_Multiplication)
    ->DenseRange(9, 16, 1)
    ->Unit(benchmark::kNanosecond);
BENCHMARK(BM_Givaro_GF2X_Medium_Division)
    ->DenseRange(9, 16, 1)
    ->Unit(benchmark::kNanosecond);
BENCHMARK(BM_Givaro_GF2X_Medium_Inversion)
    ->DenseRange(9, 16, 1)
    ->Unit(benchmark::kNanosecond);

// Large field benchmarks
BENCHMARK(BM_Givaro_GF2X_Large_Addition)
    ->DenseRange(17, 20, 1)
    ->Unit(benchmark::kNanosecond);
BENCHMARK(BM_Givaro_GF2X_Large_Multiplication)
    ->DenseRange(17, 20, 1)
    ->Unit(benchmark::kNanosecond);
BENCHMARK(BM_Givaro_GF2X_Large_Division)
    ->DenseRange(17, 20, 1)
    ->Unit(benchmark::kNanosecond);
BENCHMARK(BM_Givaro_GF2X_Large_Inversion)
    ->DenseRange(17, 20, 1)
    ->Unit(benchmark::kNanosecond);

// Field construction benchmarks
BENCHMARK(BM_Givaro_GF2X_Field_Construction_Small)
    ->DenseRange(2, 8, 1)
    ->Unit(benchmark::kNanosecond);
BENCHMARK(BM_Givaro_GF2X_Field_Construction_Medium)
    ->DenseRange(9, 16, 1)
    ->Unit(benchmark::kNanosecond);
BENCHMARK(BM_Givaro_GF2X_Field_Construction_Large)
    ->DenseRange(17, 20, 1)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_MAIN();
