# Binary Extension Field Operations Benchmark Report

**Date:** June 24, 2025
**Host:** Amirs-MacBook-Pro-2.local
**CPU:** 10 cores @ 24 MHz
**Library Version:** Google Benchmark v1.9.4

## Executive Summary

This report compares the performance of three different implementations for binary extension field operations:
- **Givaro**: A high-performance computer algebra library
- **Xgalois**: Custom implementation
- **NTL**: Number Theory Library

The benchmarks test four fundamental operations (Addition, Multiplication, Division, Inversion) across five different field sizes (4, 8, 12, 16, 20 bits).

## Performance Overview

### Key Findings

1. **Addition Operations**: Givaro significantly outperforms other implementations
2. **Multiplication Operations**: Givaro shows the best performance, followed by Xgalois
3. **Division Operations**: Xgalois demonstrates superior performance over Givaro and NTL
4. **Inversion Operations**: Givaro achieves the fastest inversion times

## Detailed Performance Analysis

### Addition Operations

| Field Size | Givaro (ns) | Xgalois (ns) | NTL (ns) | Givaro vs Xgalois | Givaro vs NTL |
|------------|-------------|--------------|----------|-------------------|---------------|
| 4 bits     | 1.57        | 2.73         | 26.24    | **1.7x faster**   | **16.7x faster** |
| 8 bits     | 1.56        | 2.03         | 25.35    | **1.3x faster**   | **16.2x faster** |
| 12 bits    | 1.52        | 1.98         | 26.03    | **1.3x faster**   | **17.1x faster** |
| 16 bits    | 1.68        | 2.34         | 25.70    | **1.4x faster**   | **15.3x faster** |
| 20 bits    | 1.83        | 2.81         | 25.32    | **1.5x faster**   | **13.8x faster** |

**Winner: Givaro** - Consistently fastest across all field sizes

### Multiplication Operations

| Field Size | Givaro (ns) | Xgalois (ns) | NTL (ns) | Givaro vs Xgalois | Givaro vs NTL |
|------------|-------------|--------------|----------|-------------------|---------------|
| 4 bits     | 1.41        | 1.94         | 70.33    | **1.4x faster**   | **49.9x faster** |
| 8 bits     | 1.41        | 1.94         | 94.61    | **1.4x faster**   | **67.1x faster** |
| 12 bits    | 1.41        | 1.94         | 117.76   | **1.4x faster**   | **83.5x faster** |
| 16 bits    | 1.41        | 1.94         | 138.21   | **1.4x faster**   | **98.0x faster** |
| 20 bits    | 1.41        | 1.94         | 159.23   | **1.4x faster**   | **112.9x faster** |

**Winner: Givaro** - Maintains consistent low latency regardless of field size

### Division Operations

| Field Size | Givaro (ns) | Xgalois (ns) | NTL (ns) | Xgalois vs Givaro | Xgalois vs NTL |
|------------|-------------|--------------|----------|-------------------|----------------|
| 4 bits     | 5.19        | 1.93         | 251.08   | **2.7x faster**   | **130.1x faster** |
| 8 bits     | 5.20        | 1.94         | 306.73   | **2.7x faster**   | **158.1x faster** |
| 12 bits    | 5.18        | 1.93         | 381.83   | **2.7x faster**   | **197.8x faster** |
| 16 bits    | 5.08        | 1.94         | 456.15   | **2.6x faster**   | **235.2x faster** |
| 20 bits    | 5.14        | 1.94         | 515.26   | **2.7x faster**   | **265.6x faster** |

**Winner: Xgalois** - Exceptional division performance across all field sizes

### Inversion Operations

| Field Size | Givaro (ns) | Xgalois (ns) | NTL (ns) | Givaro vs Xgalois | Givaro vs NTL |
|------------|-------------|--------------|----------|-------------------|---------------|
| 4 bits     | 0.74        | 1.29         | 160.19   | **1.7x faster**   | **216.4x faster** |
| 8 bits     | 0.74        | 1.29         | 190.47   | **1.7x faster**   | **257.2x faster** |
| 12 bits    | 0.74        | 1.29         | 252.02   | **1.7x faster**   | **340.5x faster** |
| 16 bits    | 0.74        | 1.29         | 298.92   | **1.7x faster**   | **403.9x faster** |
| 20 bits    | 0.74        | 1.29         | 339.32   | **1.7x faster**   | **457.7x faster** |

**Winner: Givaro** - Outstanding sub-nanosecond inversion performance

## Memory Usage Analysis

### Peak Memory Consumption (KB)

| Operation      | Field Size | Givaro | Xgalois | NTL    |
|----------------|------------|--------|---------|--------|
| Addition       | 4 bits     | 3.2    | 134.9   | 138.1  |
| Addition       | 20 bits    | 110.2  | 135.0   | 139.8  |
| Multiplication | 4 bits     | 110.2  | 135.0   | 139.8  |
| Multiplication | 20 bits    | 110.2  | 135.0   | 140.0  |
| Division       | 4 bits     | 110.2  | 135.0   | 140.0  |
| Division       | 20 bits    | 110.2  | 135.5   | 140.0  |
| Inversion      | 4 bits     | 110.2  | 135.5   | 140.0  |
| Inversion      | 20 bits    | 134.9  | 136.3   | 140.0  |

**Memory Winner: Givaro** - Most memory-efficient for smaller field sizes

## Performance Scaling Analysis

### Scaling with Field Size

1. **Givaro**:
   - Addition: Minimal scaling impact (1.57ns → 1.83ns)
   - Multiplication: Excellent scaling (constant ~1.41ns)
   - Division: Good scaling (5.19ns → 5.14ns)
   - Inversion: Excellent scaling (constant ~0.74ns)

2. **Xgalois**:
   - Addition: Moderate scaling (2.73ns → 2.81ns)
   - Multiplication: Excellent scaling (constant ~1.94ns)
   - Division: Excellent scaling (constant ~1.93ns)
   - Inversion: Excellent scaling (constant ~1.29ns)

3. **NTL**:
   - Addition: Good scaling (26.24ns → 25.32ns)
   - Multiplication: Poor scaling (70.33ns → 159.23ns)
   - Division: Very poor scaling (251.08ns → 515.26ns)
   - Inversion: Poor scaling (160.19ns → 339.32ns)

## Recommendations

### Best Use Cases

1. **For Addition-Heavy Workloads**: Choose **Givaro**
   - Up to 17x faster than NTL
   - 1.3-1.7x faster than Xgalois

2. **For Multiplication-Heavy Workloads**: Choose **Givaro**
   - Up to 113x faster than NTL
   - 1.4x faster than Xgalois

3. **For Division-Heavy Workloads**: Choose **Xgalois**
   - 2.7x faster than Givaro
   - Up to 266x faster than NTL

4. **For Inversion-Heavy Workloads**: Choose **Givaro**
   - 1.7x faster than Xgalois
   - Up to 458x faster than NTL

5. **For Memory-Constrained Environments**: Choose **Givaro**
   - Lowest memory footprint for small fields
   - Most efficient memory scaling

### General Recommendations

- **Givaro** is the best all-around choice for most applications
- **Xgalois** should be considered for division-heavy computations
- **NTL** is not recommended for performance-critical applications in this domain

## Technical Notes

- All benchmarks run with single-threaded execution
- Field orders tested: 2^4 (16), 2^8 (256), 2^12 (4096), 2^16 (65536), 2^20 (1048576)
- Times reported in nanoseconds (ns)
- CPU scaling was disabled during benchmarking
- Results are averages over millions of iterations for statistical significance

## Conclusion

The benchmark results clearly demonstrate that **Givaro** provides the best overall performance for binary extension field operations, with **Xgalois** offering superior division performance. **NTL**, while a mature library, significantly underperforms in this specific domain and should be avoided for performance-critical applications involving binary extension fields.

For applications requiring a single library that performs well across all operations, **Givaro** is the recommended choice. For specialized applications with heavy division requirements, a hybrid approach using **Xgalois** for division and **Givaro** for other operations may provide optimal performance.
