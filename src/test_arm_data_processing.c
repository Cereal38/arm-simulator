
#include <stdio.h>
#include <assert.h>
#include "arm_data_processing.h"
#include "arm_core.h"
#include "registers.h"
#include "memory.h"
#include "arm_constants.h"
#include "util.h"

void test_add(arm_core p)
{
  printf("Test : ADD (Immediate value) ... ");
  registers_write(p->reg, 0, USR, 2);
  // add r1, r0, #3
  // Cond -- I ---- S Rn   Rd   Shifter
  // 1110 00 1 0100 1 0000 0001 0000 00000011
  arm_data_processing_immediate(p, 0b11100010100100000001000000000011);
  assert(registers_read(p->reg, 1, USR) == 5);
  assert(registers_read_Z(p->reg) == 0);
  assert(registers_read_N(p->reg) == 0);
  assert(registers_read_C(p->reg) == 0);
  assert(registers_read_V(p->reg) == 0);
  printf("OK\n");

  printf("Test : ADD (Second value from register) ... ");
  registers_write(p->reg, 0, USR, 2);
  registers_write(p->reg, 2, USR, 4);
  registers_write_Z(p->reg, 0);
  // add r1, r0, r2
  // Cond -- I ---- S Rn   Rd   Shifter
  // 1110 00 0 0100 1 0000 0001 0000 0000 0010
  arm_data_processing_immediate(p, 0b11100000100100000001000000000010);
  assert(registers_read(p->reg, 1, USR) == 6);
  printf("OK\n");

  printf("Test : ADD (N+V : 0x7FFFFFFF + 0x1) ... ");
  registers_write(p->reg, 0, USR, 0x7FFFFFFF);
  // add r1, r0, #1
  // Cond -- I ---- S Rn   Rd   Shifter
  // 1110 00 1 0100 1 0000 0001 0000 00000001
  arm_data_processing_immediate(p, 0b11100010100100000001000000000001);
  assert(registers_read(p->reg, 1, USR) == 0x80000000);
  assert(registers_read_Z(p->reg) == 0);
  assert(registers_read_N(p->reg) == 1);
  assert(registers_read_C(p->reg) == 0);
  assert(registers_read_V(p->reg) == 1);
  printf("OK\n");

  printf("Test : ADD (Unvalid condition) ... ");
  registers_write(p->reg, 0, USR, 2);
  registers_write_Z(p->reg, 0);
  // Cond -- I ---- S Rn   Rd   Shifter
  // 0000 00 1 0100 1 0000 0000 0000 00000011
  arm_data_processing_immediate(p, 0b00000010100100000000000000000011);
  assert(registers_read(p->reg, 0, USR) == 2);
  printf("OK\n");

  printf("Test : ADD (Immediate value with rotation) ... ");
  registers_write(p->reg, 0, USR, 2);
  // add r1, r0, #512
  // Cond -- I ---- S Rn   Rd   Shifter
  // 1110 00 1 0100 1 0000 0001 1100 00000010
  arm_data_processing_immediate(p, 0b11100010100100000001110000000010);
  assert(registers_read(p->reg, 1, USR) == 514);
  assert(registers_read_Z(p->reg) == 0);
  assert(registers_read_N(p->reg) == 0);
  assert(registers_read_C(p->reg) == 0);
  assert(registers_read_V(p->reg) == 0);
  printf("OK\n");

  printf("Test : ADD (Result is 0) ... ");
  registers_write(p->reg, 0, USR, 0);
  // add r1, r0, #0
  // Cond -- I ---- S Rn   Rd   Shifter
  // 1110 00 1 0100 1 0000 0001 0000 00000000
  arm_data_processing_immediate(p, 0b11100010100100000001000000000000);
  assert(registers_read(p->reg, 1, USR) == 0);
  assert(registers_read_Z(p->reg) == 1);
  assert(registers_read_N(p->reg) == 0);
  assert(registers_read_C(p->reg) == 0);
  assert(registers_read_V(p->reg) == 0);
  printf("OK\n");
}

void test_sub(arm_core p)
{
  printf("Test : SUB (Positive result) ... ");
  registers_write(p->reg, 0, USR, 4);
  // sub r1, r0, #2
  // Cond -- I ---- S Rn   Rd   Shifter
  // 1110 00 1 0010 1 0000 0001 0000 00000011
  arm_data_processing_immediate(p, 0b11100010010100000001000000000010);
  assert(registers_read(p->reg, 1, USR) == 2);
  assert(registers_read_Z(p->reg) == 0);
  assert(registers_read_N(p->reg) == 0);
  assert(registers_read_C(p->reg) == 1);
  assert(registers_read_V(p->reg) == 0);
  printf("OK\n");

  printf("Test : SUB (Negative result) ... ");
  registers_write(p->reg, 0, USR, 2);
  // sub r1, r0, #4
  // Cond -- I ---- S Rn   Rd   Shifter
  // 1110 00 1 0010 1 0000 0001 0000 00000100
  arm_data_processing_immediate(p, 0b11100010010100000001000000000100);
  assert(registers_read(p->reg, 1, USR) == -2);
  assert(registers_read_Z(p->reg) == 0);
  assert(registers_read_N(p->reg) == 1);
  assert(registers_read_C(p->reg) == 0);
  assert(registers_read_V(p->reg) == 0);
  printf("OK\n");

  printf("Test : SUB (Overflow) ... ");
  registers_write(p->reg, 0, USR, 0x80000000);
  // sub r1, r0, #1
  // Cond -- I ---- S Rn   Rd   Shifter
  // 1110 00 1 0010 1 0000 0001 0000 00000001
  arm_data_processing_immediate(p, 0b11100010010100000001000000000001);
  assert(registers_read(p->reg, 1, USR) == 0x7FFFFFFF);
  assert(registers_read_Z(p->reg) == 0);
  assert(registers_read_N(p->reg) == 0);
  assert(registers_read_C(p->reg) == 1);
  assert(registers_read_V(p->reg) == 1);
  printf("OK\n");

  printf("Test : SUB (Result is 0) ... ");
  registers_write(p->reg, 0, USR, 11);
  // sub r1, r0, #11
  // Cond -- I ---- S Rn   Rd   Shifter
  // 1110 00 1 0010 1 0000 0001 0000 00001011
  arm_data_processing_immediate(p, 0b11100010010100000001000000001011);
  assert(registers_read(p->reg, 1, USR) == 0);
  assert(registers_read_Z(p->reg) == 1);
  assert(registers_read_N(p->reg) == 0);
  assert(registers_read_C(p->reg) == 1);
  assert(registers_read_V(p->reg) == 0);
  printf("OK\n");
}

void test_and(arm_core p)
{
  printf("Test : AND (Immediate value) ... ");
  registers_write(p->reg, 0, USR, 0xE4);
  // and r1, r0, #0x47
  // Cond -- I ---- S Rn   Rd   Shifter
  // 1110 00 1 0000 1 0000 0001 0000 01000111
  // 0xE4 & 0x47 = 11100100 & 01000111 = 01000100 = 0x44
  arm_data_processing_immediate(p, 0b11100010000100000001000001000111);
  assert(registers_read(p->reg, 1, USR) == 0x44);
  assert(registers_read_Z(p->reg) == 0);
  assert(registers_read_N(p->reg) == 0);
  // assert(registers_read_C(p->reg) == 0);
  printf("OK\n");

  printf("Test : AND (Second value from register) ... ");
  registers_write(p->reg, 0, USR, 0xE4);
  registers_write(p->reg, 2, USR, 0x47);
  // and r1, r0, r2
  // Cond -- I ---- S Rn   Rd   Shifter
  // 1110 00 0 0000 1 0000 0001 0000 0000 0010
  arm_data_processing_immediate(p, 0b11100000000100000001000000000010);
  assert(registers_read(p->reg, 1, USR) == 0x44);
  assert(registers_read_Z(p->reg) == 0);
  assert(registers_read_N(p->reg) == 0);
  // assert(registers_read_C(p->reg) == 0);
  printf("OK\n");

  printf("Test : AND (Result is 0) ... ");
  registers_write(p->reg, 0, USR, 0xE4);
  // and r1, r0, #0x11
  // Cond -- I ---- S Rn   Rd   Shifter
  // 1110 00 1 0000 1 0000 0001 0000 00010001
  // 0xE4 & 0x11 = 11100100 & 00010001 = 00000000 = 0x00
  arm_data_processing_immediate(p, 0b11100010000100000001000000010001);
  assert(registers_read(p->reg, 1, USR) == 0x00);
  assert(registers_read_Z(p->reg) == 1);
  assert(registers_read_N(p->reg) == 0);
  // assert(registers_read_C(p->reg) == 0);
  printf("OK\n");
}

int main()
{
  arm_core p = arm_create(registers_create(), memory_create(2048));

  test_add(p);
  test_sub(p);
  test_and(p);

  memory_destroy(p->mem);
  registers_destroy(p->reg);
  arm_destroy(p);

  return 0;
}