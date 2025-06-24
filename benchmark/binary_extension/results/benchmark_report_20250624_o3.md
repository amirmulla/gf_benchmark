# Binary Extension Field Benchmark Report

**Date:** June 24, 2025
**Test System:** Amir's MacBook Pro 2
**Benchmark Library:** Google Benchmark v1.9.4

## System Configuration

- **Host:** Amirs-MacBook-Pro-2.local
- **CPUs:** 10 cores @ 24 MHz
- **CPU Scaling:** Disabled
- **Load Average:** [1.91, 2.23, 2.18]

### Cache Hierarchy
- **L1 Data Cache:** 64 KB
- **L1 Instruction Cache:** 128 KB
- **L2 Unified Cache:** 4 MB (shared between 1 core)

## Test Overview

This benchmark compares the performance of two Galois Field (GF) libraries:
- **Givaro:** Established library for algebraic computations
- **Xgalois:** Custom implementation being evaluated

### Field Orders Tested
- **GF(2^4):** 16 elements
- **GF(2^8):** 256 elements
- **GF(2^12):** 4,096 elements
- **GF(2^16):** 65,536 elements
- **GF(2^20):** 1,048,576 elements

### Operations Benchmarked
1. **Addition**
2. **Multiplication**
3. **Division**
4. **Inversion**

## Performance Results

### Addition Operations

| Field Order | Givaro (ns) | Xgalois (ns) | Speedup | Memory (KB) |
|-------------|-------------|--------------|---------|-------------|
| 2^4 (16)    | 1.53        | 2.52         | 0.61x   | 2.5 / 167   |
| 2^8 (256)   | 1.60        | 2.08         | 0.77x   | 2.7 / 167   |
| 2^12 (4K)   | 1.52        | 1.98         | 0.77x   | 2.9 / 167   |
| 2^16 (65K)  | 1.53        | 2.01         | 0.76x   | 7.6 / 167   |
| 2^20 (1M)   | 1.65        | 2.28         | 0.72x   | 106 / 167   |

**Analysis:** Givaro consistently outperforms Xgalois in addition operations by 24-39%. Givaro's memory usage scales with field size while Xgalois maintains constant memory overhead.

### Multiplication Operations

| Field Order | Givaro (ns) | Xgalois (ns) | Speedup | Memory (KB) |
|-------------|-------------|--------------|---------|-------------|
| 2^4 (16)    | 1.40        | 1.95         | 0.72x   | 106 / 167   |
| 2^8 (256)   | 1.41        | 1.94         | 0.73x   | 106 / 167   |
| 2^12 (4K)   | 1.42        | 1.94         | 0.73x   | 106 / 167   |
| 2^16 (65K)  | 1.41        | 1.94         | 0.73x   | 110 / 167   |
| 2^20 (1M)   | 1.41        | 1.94         | 0.73x   | 159 / 167   |

**Analysis:** Givaro shows consistent ~27% performance advantage across all field sizes. Both libraries show stable performance regardless of field order, indicating efficient algorithms.

### Division Operations

| Field Order | Givaro (ns) | Xgalois (ns) | Speedup | Memory (KB) |
|-------------|-------------|--------------|---------|-------------|
| 2^4 (16)    | 1.29        | 1.94         | 0.66x   | 159 / 167   |
| 2^8 (256)   | 1.29        | 1.95         | 0.66x   | 159 / 167   |
| 2^12 (4K)   | 1.29        | 1.94         | 0.66x   | 159 / 167   |
| 2^16 (65K)  | 1.29        | 1.94         | 0.66x   | 159 / 167   |
| 2^20 (1M)   | 1.29        | 1.94         | 0.66x   | 159 / 167   |

**Analysis:** Givaro demonstrates a significant 34% performance advantage in division operations. The consistent timing across field sizes suggests both implementations use similar algorithmic approaches.

### Inversion Operations

| Field Order | Givaro (ns) | Xgalois (ns) | Speedup | Memory (KB) |
|-------------|-------------|--------------|---------|-------------|
| 2^4 (16)    | 0.70        | 1.28         | 0.55x   | 159 / 167   |
| 2^8 (256)   | 0.69        | 1.30         | 0.53x   | 159 / 167   |
| 2^12 (4K)   | 0.69        | 1.28         | 0.54x   | 159 / 167   |
| 2^16 (65K)  | 0.69        | 1.29         | 0.54x   | 159 / 167   |
| 2^20 (1M)   | 0.69        | 1.28         | 0.54x   | 167 / 167   |

**Analysis:** Givaro excels in inversion operations with a remarkable 46-47% performance advantage. This is the largest performance gap observed across all operations.

## Key Findings

### Performance Summary
1. **Givaro Advantages:**
   - Consistently faster across all operations
   - Particularly strong in inversion (46% faster)
   - Good division performance (34% faster)
   - Efficient memory usage for smaller fields

2. **Xgalois Characteristics:**
   - Consistent memory footprint (~167KB) regardless of field size
   - Stable performance scaling
   - Room for optimization, especially in inversion algorithms

### Memory Usage Patterns
- **Givaro:** Variable memory usage (2.5KB - 167KB) based on field complexity
- **Xgalois:** Consistent ~167KB memory footprint across all field sizes

### Performance Ranking (Best to Worst)
1. **Givaro Inversion:** 0.69-0.70 ns
2. **Givaro Division:** 1.29 ns
3. **Givaro Multiplication:** 1.40-1.42 ns
4. **Givaro Addition:** 1.52-1.65 ns
5. **Xgalois Inversion:** 1.28-1.30 ns
6. **Xgalois Multiplication:** 1.94-1.95 ns
7. **Xgalois Division:** 1.94-1.95 ns
8. **Xgalois Addition:** 1.98-2.52 ns

## Recommendations

### For Xgalois Optimization
1. **Focus on Inversion:** Largest performance gap - investigate Givaro's inversion algorithm
2. **Addition Optimization:** Most variable performance - optimize for smaller fields
3. **Memory Efficiency:** Consider dynamic memory allocation for smaller fields
4. **Algorithm Analysis:** Study Givaro's consistent performance patterns

### For Production Use
- **Choose Givaro** for performance-critical applications
- **Consider Xgalois** if consistent memory usage is more important than speed
- **Benchmark specific use cases** as results may vary with different data patterns

## Test Reliability

- **High iteration counts:** 556M - 2B iterations per test ensure statistical significance
- **Consistent results:** Low variance in timing measurements
- **Controlled environment:** CPU scaling disabled, consistent system load
- **Comprehensive coverage:** All major field operations tested across multiple field sizes

---

*Generated from benchmark data collected on June 24, 2025*
