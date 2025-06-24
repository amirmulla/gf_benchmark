# Binary Extension Field Benchmark Report - Updated

**Date:** June 24, 2025 (11:40 AM)
**Test System:** Amir's MacBook Pro 2
**Benchmark Library:** Google Benchmark v1.9.4

## System Configuration

- **Host:** Amirs-MacBook-Pro-2.local
- **CPUs:** 10 cores @ 24 MHz
- **CPU Scaling:** Disabled
- **Load Average:** [1.59, 1.92, 2.08] (improved from earlier run)

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
| 2^4 (16)    | 28.03       | 23.41        | 1.20x   | 2.7 / 139   |
| 2^8 (256)   | 28.04       | 22.14        | 1.27x   | 2.7 / 139   |
| 2^12 (4K)   | 27.89       | 23.66        | 1.18x   | 3.0 / 139   |
| 2^16 (65K)  | 27.99       | 30.21        | 0.93x   | 7.8 / 139   |
| 2^20 (1M)   | 27.57       | 30.79        | 0.90x   | 98 / 139    |

**Analysis:** **DRAMATIC PERFORMANCE CHANGE!** Unlike the previous run, Xgalois now outperforms Givaro in addition for smaller fields (2^4 to 2^12) by 18-27%. However, Givaro still wins for larger fields (2^16, 2^20).

### Multiplication Operations

| Field Order | Givaro (ns) | Xgalois (ns) | Speedup | Memory (KB) |
|-------------|-------------|--------------|---------|-------------|
| 2^4 (16)    | 16.06       | 9.66         | 1.66x   | 98 / 139    |
| 2^8 (256)   | 16.40       | 9.66         | 1.70x   | 98 / 139    |
| 2^12 (4K)   | 16.08       | 9.88         | 1.63x   | 98 / 139    |
| 2^16 (65K)  | 16.91       | 9.73         | 1.74x   | 98 / 139    |
| 2^20 (1M)   | 16.77       | 10.09        | 1.66x   | 98 / 139    |

**Analysis:** **MAJOR REVERSAL!** Xgalois now dramatically outperforms Givaro in multiplication by 63-74% across all field sizes. This is a complete turnaround from the previous benchmark.

### Division Operations

| Field Order | Givaro (ns) | Xgalois (ns) | Speedup | Memory (KB) |
|-------------|-------------|--------------|---------|-------------|
| 2^4 (16)    | 15.25       | 10.81        | 1.41x   | 98 / 139    |
| 2^8 (256)   | 15.37       | 10.64        | 1.44x   | 98 / 139    |
| 2^12 (4K)   | 15.19       | 10.71        | 1.42x   | 98 / 139    |
| 2^16 (65K)  | 15.10       | 10.61        | 1.42x   | 98 / 139    |
| 2^20 (1M)   | 15.25       | 10.62        | 1.44x   | 131 / 139   |

**Analysis:** **ANOTHER REVERSAL!** Xgalois now outperforms Givaro in division operations by 41-44% across all field sizes. Previously, Givaro had a 34% advantage.

### Inversion Operations

| Field Order | Givaro (ns) | Xgalois (ns) | Speedup | Memory (KB) |
|-------------|-------------|--------------|---------|-------------|
| 2^4 (16)    | 8.82        | 7.04         | 1.25x   | 131 / 139   |
| 2^8 (256)   | 7.61        | 7.02         | 1.08x   | 131 / 139   |
| 2^12 (4K)   | 7.53        | 7.02         | 1.07x   | 131 / 139   |
| 2^16 (65K)  | 7.57        | 7.10         | 1.07x   | 131 / 139   |
| 2^20 (1M)   | 7.51        | 7.17         | 1.05x   | 139 / 139   |

**Analysis:** **COMPLETE TURNAROUND!** Xgalois now outperforms Givaro in inversion operations by 5-25%. The largest advantage is for the smallest field (GF(2^4)), with performance converging for larger fields.

## Dramatic Performance Changes

### Comparison with Previous Run (11:32 AM vs 11:40 AM)

| Operation | Previous Winner | New Winner | Performance Swing |
|-----------|----------------|------------|-------------------|
| Addition (small fields) | Givaro | **Xgalois** | 24-39% → 18-27% advantage |
| Multiplication | Givaro | **Xgalois** | 27% → 63-74% swing |
| Division | Givaro | **Xgalois** | 34% → 41-44% swing |
| Inversion | Givaro | **Xgalois** | 46-47% → 5-25% swing |

### Possible Explanations for Performance Changes

1. **Compiler Optimizations:** Different optimization levels or flags
2. **Code Changes:** Modifications to Xgalois implementation between runs
3. **System State:** Different CPU thermal state, cache warming, or background processes
4. **Memory Layout:** Different memory allocation patterns affecting cache performance
5. **Binary Differences:** Recompilation with different settings

## Updated Performance Ranking (Best to Worst)

1. **Xgalois Inversion:** 7.02-7.17 ns ⭐ **NEW LEADER**
2. **Givaro Inversion:** 7.51-8.82 ns
3. **Xgalois Multiplication:** 9.66-10.09 ns ⭐ **MAJOR IMPROVEMENT**
4. **Xgalois Division:** 10.61-10.81 ns ⭐ **MAJOR IMPROVEMENT**
5. **Givaro Division:** 15.10-15.37 ns
6. **Givaro Multiplication:** 16.06-16.91 ns
7. **Xgalois Addition:** 22.14-30.79 ns
8. **Givaro Addition:** 27.57-28.04 ns

## Key Findings

### Xgalois Advantages (NEW!)
- **Multiplication Leader:** 63-74% faster than Givaro
- **Division Leader:** 41-44% faster than Givaro
- **Inversion Leader:** 5-25% faster than Givaro
- **Addition Leader:** 18-27% faster for small fields (GF(2^4) to GF(2^12))
- **Consistent Memory Usage:** ~139KB across all operations and field sizes

### Givaro Characteristics
- **Addition advantage** for larger fields (GF(2^16), GF(2^20))
- **Variable memory usage** (2.7KB - 139KB) based on operation and field size
- **More predictable** performance characteristics

## Memory Usage Analysis

- **Givaro:** 2.7KB - 139KB (operation and field-size dependent)
- **Xgalois:** ~139KB (consistent across all tests)
- **Memory efficiency winner:** Givaro for smaller fields, similar for larger operations

## Recommendations

### For Production Use
- **Choose Xgalois** for multiplication, division, and inversion operations
- **Choose Givaro** for addition on larger fields if memory efficiency matters
- **Benchmark your specific workload** as performance characteristics may vary significantly

### For Xgalois Development
- **Investigate addition performance** for larger fields (GF(2^16), GF(2^20))
- **Document the changes** that led to this massive performance improvement
- **Consider memory optimization** for smaller field operations

### For Performance Analysis
- **Version control critical:** Track exactly what changed between benchmarks
- **Reproducibility testing:** Verify these results with multiple runs
- **Profiling recommended:** Understand what optimizations drove these improvements

## Test Reliability

- **High iteration counts:** 45M - 199M iterations per test ensure statistical significance
- **Improved system state:** Lower load average (1.59 vs 1.91) may contribute to better performance
- **Consistent methodology:** Same hardware and benchmark framework
- **Dramatic but consistent changes:** All Xgalois operations improved significantly

---

*Generated from benchmark data collected on June 24, 2025 at 11:40 AM*
*⚠️ This represents a dramatic performance reversal from the 11:32 AM benchmark*
