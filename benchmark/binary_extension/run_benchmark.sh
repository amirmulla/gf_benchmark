#!/bin/bash

# Givaro Binary Extension Field Implementation Benchmark Runner
# Benchmarks Givaro's GF(2^m) implementation across different field sizes

set -e

echo "================================================="
echo "Givaro GF(2^m) Binary Extension Field Benchmark"
echo "================================================="
echo "Benchmarking Givaro's GFq implementation for GF(2^m)"
echo "Testing field operations across small, medium, and large field sizes"
echo "================================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
BENCHMARK_TIME="1.0"
OUTPUT_FORMAT="json"
RESULTS_DIR="results"
BENCHMARK_FILTER=""

# Function to print usage
print_usage() {
    echo -e "${GREEN}Givaro Binary Extension Field Benchmark Runner${NC}"
    echo ""
    echo "Usage: $0 [TEST_TYPE] [OPTIONS]"
    echo ""
    echo -e "${YELLOW}TEST_TYPES:${NC}"
    echo "  1  - All benchmarks (default)"
    echo "  2  - Small field tests only (GF(2^2) to GF(2^8))"
    echo "  3  - Medium field tests only (GF(2^9) to GF(2^16))"
    echo "  4  - Large field tests only (GF(2^17) to GF(2^20))"
    echo "  5  - Addition tests only (all field sizes)"
    echo "  6  - Multiplication tests only (all field sizes)"
    echo "  7  - Division tests only (all field sizes)"
    echo "  8  - Inversion tests only (all field sizes)"
    echo "  9  - Exponentiation tests only (all field sizes)"
    echo "  10 - Field construction tests only (all field sizes)"
    echo "  11 - Quick test (0.1s per benchmark)"
    echo "  12 - Memory usage analysis"
    echo ""
    echo -e "${YELLOW}OPTIONS:${NC}"
    echo "  -t, --time TIME      Set benchmark time per test (default: 1.0s)"
    echo "  -f, --format FORMAT  Output format: console, json, csv (default: csv)"
    echo "  -o, --output DIR     Output directory (default: $RESULTS_DIR)"
    echo "  -h, --help          Show this help message"
    echo ""
    echo -e "${YELLOW}EXAMPLES:${NC}"
    echo "  $0 1                           # Run all benchmarks"
    echo "  $0 2 --time 2.0                # Run small field tests for 2s each"
    echo "  $0 5 --format json             # Run addition tests with JSON output"
    echo "  $0 11                          # Quick test run"
    echo ""
}

# Function to create results directory
create_results_dir() {
    if [ ! -d "$RESULTS_DIR" ]; then
        mkdir -p "$RESULTS_DIR"
        echo -e "${GREEN}Created results directory: $RESULTS_DIR${NC}"
    fi
}

# Function to get system info
get_system_info() {
    echo -e "${BLUE}System Information:${NC}"
    echo "Date: $(date)"
    echo "Host: $(hostname)"
    echo "OS: $(uname -s) $(uname -r)"
    echo "Architecture: $(uname -m)"
    echo "CPU: $(sysctl -n machdep.cpu.brand_string 2>/dev/null || echo 'Unknown')"
    echo "Memory: $(system_profiler SPHardwareDataType | grep 'Memory:' | awk '{print $2, $3}' 2>/dev/null || echo 'Unknown')"
    echo "Compiler: $(c++ --version | head -n1 2>/dev/null || echo 'Unknown')"
    echo ""
}

# Function to build benchmark
build_benchmark() {
    echo -e "${YELLOW}Building benchmark...${NC}"

    # Compile directly with clang++
    echo -e "${YELLOW}Compiling with clang++...${NC}"

    /usr/bin/clang++ \
        -std=c++23 \
        -O3 \
        -DNDEBUG \
        -I/opt/homebrew/include \
        -I/Users/amirmulla/Desktop/gf_benchmark \
        -I/Users/amirmulla/xgalois \
        -L/opt/homebrew/lib \
        binary_extension_benchmark.cpp \
        -o binary_extension_benchmark \
        -lgivaro \
        -lgmp \
        -lntl \
        -lbenchmark \
        -pthread

    if [ ! -f "binary_extension_benchmark" ]; then
        echo -e "${RED}Error: Failed to build benchmark executable${NC}"
        exit 1
    fi

    echo -e "${GREEN}Benchmark built successfully${NC}"
}

# Function to run benchmark with specified parameters
run_benchmark() {
    local test_name="$1"
    local filter="$2"
    local output_file="$3"

    echo -e "${YELLOW}Running: $test_name${NC}"

    local benchmark_args=""

    # Set benchmark time
    benchmark_args="--benchmark_min_time=${BENCHMARK_TIME}s"

    # Set output format
    case "$OUTPUT_FORMAT" in
        "json")
            benchmark_args="$benchmark_args --benchmark_format=json"
            ;;
        "csv")
            benchmark_args="$benchmark_args --benchmark_format=csv"
            ;;
        "console")
            benchmark_args="$benchmark_args --benchmark_format=console"
            ;;
    esac

    # Set filter if provided
    if [ -n "$filter" ]; then
        benchmark_args="$benchmark_args --benchmark_filter=$filter"
    fi

    # Set output file if provided
    if [ -n "$output_file" ]; then
        benchmark_args="$benchmark_args --benchmark_out=$output_file"
    fi

    # Run the benchmark
    echo "Command: ./binary_extension_benchmark $benchmark_args"
    ./binary_extension_benchmark $benchmark_args

    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ $test_name completed successfully${NC}"
        if [ -n "$output_file" ]; then
            echo -e "${GREEN}Results saved to: $output_file${NC}"
        fi
    else
        echo -e "${RED}✗ $test_name failed${NC}"
        return 1
    fi

    echo ""
}

# Parse command line arguments
TEST_TYPE=1
while [[ $# -gt 0 ]]; do
    case $1 in
        -t|--time)
            BENCHMARK_TIME="$2"
            shift 2
            ;;
        -f|--format)
            OUTPUT_FORMAT="$2"
            shift 2
            ;;
        -o|--output)
            RESULTS_DIR="$2"
            shift 2
            ;;
        -h|--help)
            print_usage
            exit 0
            ;;
        [1-9]|1[0-2])
            TEST_TYPE=$1
            shift
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            print_usage
            exit 1
            ;;
    esac
done

# Validate output format
case "$OUTPUT_FORMAT" in
    "console"|"json"|"csv")
        ;;
    *)
        echo -e "${RED}Invalid output format: $OUTPUT_FORMAT${NC}"
        echo "Valid formats: console, json, csv"
        exit 1
        ;;
esac

# Main execution
echo -e "${GREEN}Starting Givaro Binary Extension Field Benchmarks${NC}"
echo "Test Type: $TEST_TYPE"
echo "Benchmark Time: ${BENCHMARK_TIME}s per test"
echo "Output Format: $OUTPUT_FORMAT"
echo "Results Directory: $RESULTS_DIR"
echo ""

get_system_info
create_results_dir
build_benchmark

# Get timestamp for result files
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

# Set output file based on format
if [ "$OUTPUT_FORMAT" != "console" ]; then
    OUTPUT_FILE="$RESULTS_DIR/benchmark_${TIMESTAMP}.$OUTPUT_FORMAT"
else
    OUTPUT_FILE=""
fi

# Run tests based on TEST_TYPE
case $TEST_TYPE in
    1) # All benchmarks
        echo -e "${BLUE}Running all benchmarks...${NC}"
        run_benchmark "All Benchmarks" "" "$OUTPUT_FILE"
        ;;
    2) # Small field tests
        echo -e "${BLUE}Running small field tests (GF(2^2) to GF(2^8))...${NC}"
        run_benchmark "Small Field Tests" "Small" "$OUTPUT_FILE"
        ;;
    3) # Medium field tests
        echo -e "${BLUE}Running medium field tests (GF(2^9) to GF(2^16))...${NC}"
        run_benchmark "Medium Field Tests" "Medium" "$OUTPUT_FILE"
        ;;
    4) # Large field tests
        echo -e "${BLUE}Running large field tests (GF(2^17) to GF(2^20))...${NC}"
        run_benchmark "Large Field Tests" "Large" "$OUTPUT_FILE"
        ;;
    5) # Addition tests
        echo -e "${BLUE}Running addition tests (all field sizes)...${NC}"
        run_benchmark "Addition Tests" "Addition" "$OUTPUT_FILE"
        ;;
    6) # Multiplication tests
        echo -e "${BLUE}Running multiplication tests (all field sizes)...${NC}"
        run_benchmark "Multiplication Tests" "Multiplication" "$OUTPUT_FILE"
        ;;
    7) # Division tests
        echo -e "${BLUE}Running division tests (all field sizes)...${NC}"
        run_benchmark "Division Tests" "Division" "$OUTPUT_FILE"
        ;;
    8) # Inversion tests
        echo -e "${BLUE}Running inversion tests (all field sizes)...${NC}"
        run_benchmark "Inversion Tests" "Inversion" "$OUTPUT_FILE"
        ;;
    9) # Exponentiation tests
        echo -e "${BLUE}Running exponentiation tests (all field sizes)...${NC}"
        run_benchmark "Exponentiation Tests" "Exponentiation" "$OUTPUT_FILE"
        ;;
    10) # Field construction tests
        echo -e "${BLUE}Running field construction tests (all field sizes)...${NC}"
        run_benchmark "Field Construction Tests" "Construction" "$OUTPUT_FILE"
        ;;
    11) # Quick test
        echo -e "${BLUE}Running quick test (0.1s per benchmark)...${NC}"
        BENCHMARK_TIME="0.1"
        run_benchmark "Quick Test" "" "$OUTPUT_FILE"
        ;;
    12) # Memory usage analysis
        echo -e "${BLUE}Running memory usage analysis...${NC}"
        OUTPUT_FORMAT="csv"
        OUTPUT_FILE="$RESULTS_DIR/givaro_memory_analysis_${TIMESTAMP}.csv"
        run_benchmark "Memory Usage Analysis" "" "$OUTPUT_FILE"

        # Generate memory usage report
        if [ -f "$OUTPUT_FILE" ]; then
            echo -e "${YELLOW}Generating memory usage report...${NC}"
            echo "Memory Usage Summary:" > "$RESULTS_DIR/memory_summary_${TIMESTAMP}.txt"
            echo "===================" >> "$RESULTS_DIR/memory_summary_${TIMESTAMP}.txt"
            grep "MemoryPeak_KB" "$OUTPUT_FILE" | head -20 >> "$RESULTS_DIR/memory_summary_${TIMESTAMP}.txt"
            echo -e "${GREEN}Memory summary saved to: $RESULTS_DIR/memory_summary_${TIMESTAMP}.txt${NC}"
        fi
        ;;
    *)
        echo -e "${RED}Invalid test type: $TEST_TYPE${NC}"
        print_usage
        exit 1
        ;;
esac

echo ""
echo -e "${GREEN}=================================================${NC}"
echo -e "${GREEN}Benchmark run completed successfully!${NC}"
echo -e "${GREEN}=================================================${NC}"

if [ -n "$OUTPUT_FILE" ] && [ -f "$OUTPUT_FILE" ]; then
    echo -e "${GREEN}Results saved to: $OUTPUT_FILE${NC}"
    echo -e "${YELLOW}File size: $(ls -lh "$OUTPUT_FILE" | awk '{print $5}')${NC}"
fi

echo ""
echo -e "${BLUE}Quick Performance Summary:${NC}"
if [ -f "$OUTPUT_FILE" ] && [ "$OUTPUT_FORMAT" = "csv" ]; then
    echo "Top 5 fastest operations:"
    tail -n +2 "$OUTPUT_FILE" | sort -t',' -k3 -n | head -5 | while IFS=',' read -r name time cpu iterations real_time time_unit; do
        echo "  $name: $time $time_unit"
    done
fi

echo ""
echo -e "${GREEN}Benchmark analysis complete!${NC}"
