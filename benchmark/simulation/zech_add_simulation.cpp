#include <benchmark/benchmark.h>
#include <chrono>
#include <givaro/gfq.h>
#include <iostream>
#include <random>
#include <vector>

using namespace Givaro;

int main() {
  // Use GF(2^20) as an example field
  constexpr uint8_t m = 8;

  std::cout << "Creating GF(2^" << static_cast<int>(m) << ") using Givaro..."
            << std::endl;

  // Create the field using Givaro's GF(2^m) implementation
  GFq<uint64_t> field(2, m);

  std::cout << "Field order: " << field.cardinality() << std::endl;
  std::cout << "Field characteristic: " << field.characteristic() << std::endl;

  // Generate 1000000 random field elements
  std::mt19937 gen(42); // Fixed seed for reproducibility
  std::uniform_int_distribution<uint64_t> dis(0, field.cardinality() - 1);

  uint64_t num_elements = 1e6;
  std::vector<GFq<uint64_t>::Element> elements;
  elements.reserve(num_elements);

  std::cout << "Generating " << num_elements << " random field elements..."
            << std::endl;
  for (size_t i = 0; i < num_elements; ++i) {
    GFq<uint64_t>::Element elem;
    field.init(elem, dis(gen));
    elements.push_back(elem);
  }

  // Measure addition performance
  std::cout << "Measuring addition of " << num_elements << " random pairs..."
            << std::endl;

  auto start_time = std::chrono::high_resolution_clock::now();

  // Perform additions between adjacent pairs
  for (size_t i = 0; i < elements.size() - 1; ++i) {
    GFq<uint64_t>::Element result;
    field.add(result, elements[i], elements[i + 1]);
    benchmark::DoNotOptimize(result);
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
      end_time - start_time);

  // Calculate performance metrics
  double total_time_ms = duration.count() / 1e6;
  double avg_time_ns =
      static_cast<double>(duration.count()) / (elements.size() - 1);
  double operations_per_second = 1e9 / avg_time_ns;

  // Display results
  std::cout << "\n=== Addition Performance Results ===" << std::endl;
  std::cout << "Total operations: " << (elements.size() - 1) << std::endl;
  std::cout << "Total time: " << total_time_ms << " ms" << std::endl;
  std::cout << "Average time per addition: " << avg_time_ns << " ns"
            << std::endl;
  std::cout << "Operations per second: "
            << static_cast<uint64_t>(operations_per_second) << std::endl;

  // Additional test: measure a batch of additions with the same operands
  std::cout << "\n=== Batch Addition Test ===" << std::endl;
  // Pick two random elements for repeated addition
  std::uniform_int_distribution<size_t> index_dis(0, elements.size() - 1);
  GFq<uint64_t>::Element a = elements[index_dis(gen)];
  GFq<uint64_t>::Element b = elements[index_dis(gen)];

  constexpr uint64_t batch_size = 1000000; // 1 million operations

  start_time = std::chrono::high_resolution_clock::now();

  for (uint64_t i = 0; i < batch_size; ++i) {
    GFq<uint64_t>::Element result;
    field.add(result, a, b);
    // Prevent compiler optimization
    volatile auto dummy = result;
    (void)dummy;
  }

  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time -
                                                                  start_time);

  total_time_ms = duration.count() / 1e6;
  avg_time_ns = static_cast<double>(duration.count()) / batch_size;
  operations_per_second = 1e9 / avg_time_ns;

  std::cout << "Batch operations: " << batch_size << std::endl;
  std::cout << "Total time: " << total_time_ms << " ms" << std::endl;
  std::cout << "Average time per addition: " << avg_time_ns << " ns"
            << std::endl;
  std::cout << "Operations per second: "
            << static_cast<uint64_t>(operations_per_second) << std::endl;

  // Test with different field sizes
  std::cout << "\n=== Performance Comparison Across Field Sizes ==="
            << std::endl;

  for (uint8_t test_m : {4, 6, 8, 10, 12}) {
    GFq<uint64_t> test_field(2, test_m);

    // Generate some test elements
    std::uniform_int_distribution<uint64_t> test_dis(
        0, test_field.cardinality() - 1);
    GFq<uint64_t>::Element test_a, test_b;
    test_field.init(test_a, test_dis(gen));
    test_field.init(test_b, test_dis(gen));

    constexpr uint64_t test_operations = 100000;

    start_time = std::chrono::high_resolution_clock::now();

    for (uint64_t i = 0; i < test_operations; ++i) {
      GFq<uint64_t>::Element result;
      test_field.add(result, test_a, test_b);
      // Prevent compiler optimization
      volatile auto dummy = result;
      (void)dummy;
    }

    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time -
                                                                    start_time);

    avg_time_ns = static_cast<double>(duration.count()) / test_operations;
    operations_per_second = 1e9 / avg_time_ns;

    std::cout << "GF(2^" << static_cast<int>(test_m)
              << ") [Order: " << test_field.cardinality()
              << "]: " << avg_time_ns << " ns/op, "
              << static_cast<uint64_t>(operations_per_second) << " ops/sec"
              << std::endl;
  }
  std::cout << "\nAddition simulation completed successfully!" << std::endl;
  return 0;
}
