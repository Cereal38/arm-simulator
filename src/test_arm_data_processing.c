
#include <stdio.h>
#include <assert.h>
#include "arm_data_processing.h"
#include "arm_core.h"
#include "registers.h"
#include "memory.h"
#include "arm_constants.h"
#include "util.h"

void test_template(
    char *name,
    arm_core p,
    uint8_t cond,
    uint8_t I,
    uint8_t opcode,
    uint8_t S,
    uint8_t Rn,
    uint8_t Rd,
    uint16_t shifter,
    uint32_t Rn_value,
    uint32_t Rs_value,
    uint32_t Rm_value,
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
  // Set Rs
  if (I == 0 && !get_bit(shifter, 7) && get_bit(shifter, 4))
  {
    registers_write(p->reg, get_bits(shifter, 11, 8), USR, Rs_value);
  }
  // Set Rm
  if (I == 0 && !get_bit(shifter, 4))
  {
    registers_write(p->reg, get_bits(shifter, 3, 0), USR, Rm_value);
  }
  uint32_t ins = (cond << 28) | (I << 25) | (opcode << 21) | (S << 20) | (Rn << 16) | (Rd << 12) | shifter;
  arm_data_processing_immediate(p, ins);
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

void test_add(arm_core p)
{
  test_template(
      "ADD (Immediate value)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      ADD,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      1,              // Rd : r1
      0b000000000011, // Shifter : #3
      2,              // Rn value
      0,              // Rs value
      0,              // Rm value
      5,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      0,              // Expected C flag
      0);             // Expected V flag

  test_template(
      "ADD (Second value from register)",
      p,
      AL,             // Cond
      0,              // I : Register value
      ADD,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      1,              // Rd : r1
      0b000000000010, // Shifter : r2
      2,              // Rn value
      0,              // Rs value
      4,              // Rm value
      6,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      0,              // Expected C flag
      0);             // Expected V flag

  test_template(
      "ADD (N+V : 0x7FFFFFFF + 0x1)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      ADD,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      1,              // Rd : r1
      0b000000000001, // Shifter : #1
      0x7FFFFFFF,     // Rn value
      0,              // Rs value
      0,              // Rm value
      0x80000000,     // Expected Rd value
      0,              // Expected Z flag
      1,              // Expected N flag
      0,              // Expected C flag
      1);             // Expected V flag

  registers_write_Z(p->reg, 0);
  test_template(
      "ADD (Unvalid condition)",
      p,
      EQ,             // Cond
      1,              // I : Immediate value
      ADD,            // ADD
      1,              // S : Set condition codes
      0,              // Rn : r0
      1,              // Rd : r1
      0b000000000011, // Shifter : #3
      2,              // Rn value
      0,              // Rs value
      0,              // Rm value
      0,              // Expected Rd value
      -1,             // Expected Z flag
      -1,             // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag

  test_template(
      "ADD (Immediate value with rotation)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      ADD,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      1,              // Rd : r1
      0b110000000010, // Shifter : #512
      2,              // Rn value
      0,              // Rs value
      0,              // Rm value
      514,            // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      0,              // Expected C flag
      0);             // Expected V flag

  test_template(
      "ADD (Result is 0)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      ADD,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      1,              // Rd : r1
      0b000000000000, // Shifter : #0
      0,              // Rn value
      0,              // Rs value
      0,              // Rm value
      0,              // Expected Rd value
      1,              // Expected Z flag
      0,              // Expected N flag
      0,              // Expected C flag
      0);             // Expected V flag

  // test_template(
  //     "ADD (Immediate shift)",
  //     p,
  //     AL,             // Cond
  //     0,              // I : Register value
  //     ADD,            // Opcode
  //     1,              // S : Set condition codes
  //     0,              // Rn : r0
  //     1,              // Rd : r1
  //     0b000000000010, // Shifter : r2
  //     2,              // Rn value
  //     0,              // Rs value
  //     0,              // Rm value
  //     4,              // Expected Rd value
  //     0,              // Expected Z flag
  //     0,              // Expected N flag
  //     0,              // Expected C flag
  //     0);             // Expected V flag
}

/*
void test_sub(arm_core p)
{
  test_template(
      "SUB (Positive result)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      SUB,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      1,              // Rd : r1
      0b000000000010, // Shifter : #2
      4,              // Rn value
      0,              // Rs value
      2,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      1,              // Expected C flag
      0);             // Expected V flag

  test_template(
      "SUB (Negative result)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      SUB,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      1,              // Rd : r1
      0b000000000100, // Shifter : #4
      2,              // Rn value
      0,              // Rs value
      -2,             // Expected Rd value
      0,              // Expected Z flag
      1,              // Expected N flag
      0,              // Expected C flag
      0);             // Expected V flag

  test_template(
      "SUB (Overflow)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      SUB,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      1,              // Rd : r1
      0b000000000001, // Shifter : #1
      0x80000000,     // Rn value
      0,              // Rs value
      0x7FFFFFFF,     // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      1,              // Expected C flag
      1);             // Expected V flag

  test_template(
      "SUB (Result is 0)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      SUB,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      1,              // Rd : r1
      0b000000001011, // Shifter : #11
      11,             // Rn value
      0,              // Rs value
      0,              // Expected Rd value
      1,              // Expected Z flag
      0,              // Expected N flag
      1,              // Expected C flag
      0);             // Expected V flag
}

void test_and(arm_core p)
{
  // 0xE4 & 0x47 = 11100100 & 01000111 = 01000100 = 0x44
  test_template(
      "AND (Immediate value)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      AND,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      1,              // Rd : r1
      0b000001000111, // Shifter : #0x47
      0xE4,           // Rn value
      0,              // Rs value
      0x44,           // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag

  test_template(
      "AND (Second value from register)",
      p,
      AL,             // Cond
      0,              // I : Register value
      AND,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      1,              // Rd : r1
      0b000000000010, // Shifter : r2
      0xE4,           // Rn value
      0x47,           // Rs value
      0x44,           // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag

  test_template(
      "AND (Result is 0)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      AND,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      1,              // Rd : r1
      0b000000010001, // Shifter : #0x11
      0xE4,           // Rn value
      0,              // Rs value
      0x00,           // Expected Rd value
      1,              // Expected Z flag
      0,              // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag
}

void test_eor(arm_core p)
{
  // 0xE4 ^ 0x47 = 11100100 ^ 01000111 = 10100011 = 0xA3
  test_template(
      "EOR (Immediate value)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      EOR,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      1,              // Rd : r1
      0b000001000111, // Shifter : #0x47
      0xE4,           // Rn value
      0,              // Rs value
      0xA3,           // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag

  test_template(
      "EOR (Second value from register)",
      p,
      AL,             // Cond
      0,              // I : Register value
      EOR,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      1,              // Rd : r1
      0b000000000010, // Shifter : r2
      0xE4,           // Rn value
      0x47,           // Rs value
      0xA3,           // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag
}

void test_rsb(arm_core p)
{
  // 2 - 4 = -2
  test_template(
      "RSB (Immediate value)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      RSB,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000010, // Shifter : 2
      4,              // Rn value
      0,              // Rs value
      -2,             // Expected Rd value
      0,              // Expected Z flag
      1,              // Expected N flag
      0,              // Expected C flag
      0);             // Expected V flag

  // 4 - 2 = 2
  test_template(
      "RSB (No borrow)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      RSB,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000100, // Shifter : 4
      2,              // Rn value
      0,              // Rs value
      2,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      1,              // Expected C flag
      0);             // Expected V flag
}

void test_adc(arm_core p)
{
  // 2 + 4 + 1 = 7
  registers_write_C(p->reg, 1);
  test_template(
      "ADC (Immediate value)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      ADC,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000010, // Shifter : 2
      4,              // Rn value
      0,              // Rs value
      7,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      0,              // Expected C flag
      0);             // Expected V flag

  // 4 + 2 + 0 = 6
  registers_write_C(p->reg, 0);
  test_template(
      "ADC (No carry)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      ADC,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000100, // Shifter : 4
      2,              // Rn value
      0,              // Rs value
      6,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      0,              // Expected C flag
      0);             // Expected V flag

  // 0xFFFFFFFE + 0x1 + 1 = 0
  registers_write_C(p->reg, 1);
  test_template(
      "ADC (Overflow)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      ADC,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000001, // Shifter : 1
      0xFFFFFFFE,     // Rn value
      0,              // Rs value
      0,              // Expected Rd value
      1,              // Expected Z flag
      0,              // Expected N flag
      1,              // Expected C flag
      0);             // Expected V flag
}

void test_sbc(arm_core p)
{
  // 4 - 2 - 1 = 1
  registers_write_C(p->reg, 0);
  test_template(
      "SBC (Immediate value)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      SBC,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000010, // Shifter : 2
      4,              // Rn value
      0,              // Rs value
      1,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      1,              // Expected C flag
      0);             // Expected V flag

  // 4 - 2 - 0 = 2
  registers_write_C(p->reg, 1);
  test_template(
      "SBC (With borrow)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      SBC,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000010, // Shifter : 2
      4,              // Rn value
      0,              // Rs value
      2,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      1,              // Expected C flag
      0);             // Expected V flag

  // 0x80000001 - 0x1 - 1 = 0x7FFFFFFF
  registers_write_C(p->reg, 0);
  test_template(
      "SBC (Overflow)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      SBC,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000001, // Shifter
      0x80000001,     // Rn value
      0,              // Rs value
      0x7FFFFFFF,     // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      1,              // Expected C flag
      1);             // Expected V flag
}

void test_rsc(arm_core p)
{
  // 2 - 4 - 1 = -3
  registers_write_C(p->reg, 0);
  test_template(
      "RSC (Immediate value)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      RSC,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000010, // Shifter : 2
      4,              // Rn value
      0,              // Rs value
      -3,             // Expected Rd value
      0,              // Expected Z flag
      1,              // Expected N flag
      0,              // Expected C flag
      0);             // Expected V flag

  // 4 - 2 - 0 = 2
  registers_write_C(p->reg, 1);
  test_template(
      "RSC (No borrow)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      RSC,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000100, // Shifter : 4
      2,              // Rn value
      0,              // Rs value
      2,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      1,              // Expected C flag
      0);             // Expected V flag
}

void test_tst(arm_core p)
{
  registers_write(p->reg, 0, USR, 0);
  test_template(
      "TST (Immediate value)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      TST,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r0
      0,              // Rd : r0
      0b000000000001, // Shifter : 1
      1,              // Rn value
      0,              // Rs value
      0,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag

  // 0xE4 & 0x1B = 11100100 & 00011011 = 0
  registers_write(p->reg, 0, USR, 0);
  test_template(
      "TST (Result is 0)",
      p,
      AL,             // Cond
      0,              // I : Register value
      TST,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r0
      0,              // Rd : r0
      0b000000000010, // Shifter : r2
      0xE4,           // Rn value
      0x1B,           // Rs value
      0,              // Expected Rd value
      1,              // Expected Z flag
      0,              // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag
}

void test_teq(arm_core p)
{
  registers_write(p->reg, 0, USR, 0);
  test_template(
      "TEQ (Immediate value)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      TEQ,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r0
      0,              // Rd : r0
      0b000000000000, // Shifter : 0
      0xF,            // Rn value
      0,              // Rs value
      0,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag

  registers_write(p->reg, 0, USR, 0);
  test_template(
      "TEQ (Result is 0)",
      p,
      AL,             // Cond
      0,              // I : Register value
      TEQ,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r0
      0,              // Rd : r0
      0b000000000010, // Shifter : r2
      0xE4,           // Rn value
      0xE4,           // Rs value
      0,              // Expected Rd value
      1,              // Expected Z flag
      0,              // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag
}

void test_cmp(arm_core p)
{
  test_template(
      "CMP (Positive result)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      CMP,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000010, // Shifter : 2
      4,              // Rn value
      0,              // Rs value
      0,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      1,              // Expected C flag
      0);             // Expected V flag

  test_template(
      "CMP (Negative result)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      CMP,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000100, // Shifter : 4
      2,              // Rn value
      0,              // Rs value
      0,              // Expected Rd value
      0,              // Expected Z flag
      1,              // Expected N flag
      0,              // Expected C flag
      0);             // Expected V flag

  test_template(
      "CMP (Overflow)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      CMP,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000001, // Shifter : 1
      0x80000000,     // Rn value
      0,              // Rs value
      0,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      1,              // Expected C flag
      1);             // Expected V flag

  test_template(
      "CMP (Minus 0)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      CMP,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000000, // Shifter : 0
      11,             // Rn value
      0,              // Rs value
      0,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      1,              // Expected C flag
      0);             // Expected V flag
}

void test_cmn(arm_core p)
{
  // 4 + 2 = 6
  registers_write_C(p->reg, 1);
  test_template(
      "CMN (Positive result)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      CMN,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000010, // Shifter : 2
      4,              // Rn value
      0,              // Rs value
      0,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      0,              // Expected C flag
      0);             // Expected V flag

  registers_write_C(p->reg, 0);
  test_template(
      "CMN (Add 0)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      CMN,            // Opcode
      1,              // S : Set condition codes
      1,              // Rn : r1
      0,              // Rd : r0
      0b000000000000, // Shifter : 0
      2,              // Rn value
      0,              // Rs value
      0,              // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      0,              // Expected C flag
      0);             // Expected V flag
}

void test_orr(arm_core p)
{
  // 0xE4 | 0x47 = 11100100 | 01000111 = 11100111 = 0xE7
  test_template(
      "ORR (Immediate value)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      ORR,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      2,              // Rd : r2
      0b000001000111, // Shifter : #0x47
      0xE4,           // Rn value
      0,              // Rs value
      0xE7,           // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag

  test_template(
      "ORR (Result is 0)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      ORR,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      2,              // Rd : r2
      0b000000000000, // Shifter : 0
      0,              // Rn value
      0,              // Rs value
      0,              // Expected Rd value
      1,              // Expected Z flag
      0,              // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag
}

void test_mov(arm_core p)
{
  registers_write(p->reg, 2, USR, 0);
  test_template(
      "MOV (Immediate value)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      MOV,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      2,              // Rd : r2
      0b000000001011, // Shifter : 11
      0,              // Rn value
      0,              // Rs value
      11,             // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag

  test_template(
      "MOV (Value from register)",
      p,
      AL,             // Cond
      0,              // I : Register value
      MOV,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      2,              // Rd : r2
      0b000000000010, // Shifter : r2
      0,              // Rn value
      -4,             // Rs value
      -4,             // Expected Rd value
      0,              // Expected Z flag
      1,              // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag
}

void test_bic(arm_core p)
{
  // 0xE4 & ~0x47 = 11100100 & 10111000 = 10100000 = 0xA0
  test_template(
      "BIC (Immediate value)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      BIC,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      2,              // Rd : r2
      0b000001000111, // Shifter : #0x47
      0xE4,           // Rn value
      0,              // Rs value
      0xA0,           // Expected Rd value
      0,              // Expected Z flag
      0,              // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag
}

void test_mvn(arm_core p)
{
  // ~0xE4 = ~0x000000E4 = 0xFFFFFF1B
  test_template(
      "MVN (Immediate value)",
      p,
      AL,             // Cond
      1,              // I : Immediate value
      MVN,            // Opcode
      1,              // S : Set condition codes
      0,              // Rn : r0
      2,              // Rd : r2
      0b000011100100, // Shifter : 0
      0,              // Rn value
      0,              // Rs value
      0xFFFFFF1B,     // Expected Rd value
      0,              // Expected Z flag
      1,              // Expected N flag
      -1,             // Expected C flag
      -1);            // Expected V flag
}
*/

int main()
{
  arm_core p = arm_create(registers_create(), memory_create(2048));

  test_add(p);
  // test_sub(p);
  // test_and(p); // TODO: shifter_carry_out
  // test_eor(p); // TODO: shifter_carry_out
  // test_rsb(p);
  // test_adc(p);
  // test_sbc(p);
  // test_rsc(p);
  // test_tst(p); // TODO: shifter_carry_out
  // test_teq(p); // TODO: shifter_carry_out
  // test_cmp(p);
  // test_cmn(p);
  // test_orr(p); // TODO: shifter_carry_out
  // test_mov(p); // TODO: shifter_carry_out
  // test_bic(p); // TODO: shifter_carry_out
  // test_mvn(p); // TODO: shifter_carry_out

  memory_destroy(p->mem);
  registers_destroy(p->reg);
  arm_destroy(p);

  return 0;
}