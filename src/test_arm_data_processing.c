
#include <stdio.h>
#include <assert.h>
#include "arm_data_processing.h"
#include "arm_core.h"
#include "registers.h"
#include "memory.h"
#include "arm_constants.h"

void test_add(arm_core p)
{
  printf("Test : ADD (2 + 3) ... ");
  registers_write(p->reg, 0, USR, 2);
  // Cond -- I ---- S Rn   Rd   Shifter
  // 1110 00 1 0100 0 0000 0001 000000000011
  arm_data_processing_add(p, 0b11100010100000000001000000000011);
  assert(registers_read(p->reg, 1, USR) == 5);
  assert(registers_read_Z(p->reg) == 0);
  assert(registers_read_N(p->reg) == 0);
  assert(registers_read_C(p->reg) == 0);
  assert(registers_read_V(p->reg) == 0);
  printf("OK\n");

  printf("Test : ADD (Unvalid condition) ... ");
  registers_write(p->reg, 0, USR, 2);
  registers_write_Z(p->reg, 0);
  // Cond -- I ---- S Rn   Rd   Shifter
  // 0000 00 1 0100 0 0000 0000 000000000011
  arm_data_processing_add(p, 0b00000010100000000000000000000011);
  assert(registers_read(p->reg, 0, USR) == 2);
  printf("OK\n");
}

int main()
{
  arm_core p = arm_create(registers_create(), memory_create(2048));
  test_add(p);

  memory_destroy(p->mem);
  registers_destroy(p->reg);
  arm_destroy(p);

  return 0;
}