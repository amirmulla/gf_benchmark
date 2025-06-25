// main.cpp
#include <cstdint>
#include <iostream>

// Function 1: Using if-else with assignment
uint32_t assign_with_if_else(uint32_t &b, uint32_t &c, uint32_t &d) {
  return (b + c) % d;
}

// Function 2: Using ternary operator with assignment
uint32_t assign_with_ternary(uint32_t &b, uint32_t &c, uint32_t &d) {
  return (b + c) > d ? (b + c - d) : (b + c);
}

// Main function to call them (we won't optimize this part for assembly
// comparison)
int main() {
  uint32_t volatile a_val0 = 0;
  uint32_t volatile a_val1 = 0;
  uint32_t b_val = 5;
  uint32_t c_val = 3;
  uint32_t d_val = 11;

  a_val0 = assign_with_if_else(b_val, c_val, d_val);
  a_val1 = assign_with_ternary(b_val, c_val, d_val);

  return 0;
}