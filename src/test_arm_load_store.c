
#include <stdio.h>
#include <assert.h>
#include "arm_core.h"
#include "arm_load_store.h"
#include "registers.h"
#include "memory.h"
#include "arm_constants.h"
#include "util.h"

void test_template_load_store(
    char *name,
    arm_core p,
    uint8_t cond,  // Bits 31-28
    uint8_t code,  // Bits 27-20
    uint8_t Rn,    // Bits 19-16
    uint8_t Rd,    // Bits 15-12
    uint16_t addr, // Bits 11-0
    uint32_t Rn_value,
    uint32_t expected_Rd,
    int8_t expected_Z,
    int8_t expected_N,
    int8_t expected_C,
    int8_t expected_V)
{
  printf("Test : %s ... ", name);
  // Set Rn
  registers_write(p->reg, Rn, USR, Rn_value);
  // Reset Rd
  registers_write(p->reg, Rd, USR, 0);
  // Set instruction
  uint32_t ins = (cond << 28) | (code << 20) | (Rn << 16) | (Rd << 12) | addr;
  // Execute
  arm_load_store(p, ins);
  // Check result
  assert(registers_read(p->reg, Rd, USR) == expected_Rd);
  if (expected_Z != -1)
  {
    assert(registers_read_Z(p->reg) == expected_Z);
  }
  if (expected_N != -1)
  {
    assert(registers_read_N(p->reg) == expected_N);
  }
  if (expected_C != -1)
  {
    assert(registers_read_C(p->reg) == expected_C);
  }
  if (expected_V != -1)
  {
    assert(registers_read_V(p->reg) == expected_V);
  }
  printf("OK\n");
}

int main()
{
  arm_core p = arm_create(registers_create(), memory_create(2048));

  memory_destroy(p->mem);
  registers_destroy(p->reg);
  arm_destroy(p);

  return 0;
}