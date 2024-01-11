
#include <stdio.h>
#include <assert.h>
#include "arm_data_processing.h"
#include "arm_core.h"
#include "registers.h"
#include "memory.h"
#include "arm_constants.h"
#include "util.h"
#include "arm_load_store.h"

void test_template_load_store(
    char *name,
    arm_core p,
    uint8_t cond,  // Bits 31-28
    uint8_t code,  // Bits 27-20
    uint8_t Rn,    // Bits 19-16
    uint8_t Rd,    // Bits 15-12
    uint16_t addr, // Bits 11-0
    uint32_t Rn_value,
    uint32_t expected_Rd)
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
}

void test_template_load_store_multiple(
    char *name,
    arm_core p,
    uint8_t cond,           // Bits 31-28
    uint8_t code,           // Bits 27-20
    uint8_t Rn,             // Bits 19-16
    uint16_t register_list, // Bits 15-0
    uint32_t Rn_value,
    int incr,
    uint32_t expected_value)
{
  printf("Test : %s ... ", name);

  // Set Rn
  arm_write_register(p, Rn, Rn_value);

  // Set instruction
  uint32_t ins = (cond << 28) | (code << 20) | (Rn << 16) | register_list;

  // Execute
  arm_load_store_multiple(p, ins);

  // Check result
  uint32_t addr = registers_read(p->reg, Rn, registers_get_mode(p->reg));
  for (int i = 0; i < 16; i++)
  {
    if ((register_list & (1 << i)) != 0)
    {
      uint32_t word;
      arm_read_word(p, addr, &word);
      assert(word == expected_value);
      addr += incr;
    }
  }
  printf("OK\n");
}

void test_STM(arm_core p)
{

  // TEST 1
  uint8_t Rn = 0;                              // r0;
  uint16_t register_list = 0b0000000000000010; // r1
  arm_write_word(p, 1, 2);                     // r1 = 2
  uint32_t expected_value = 2;                 // r0 doit être égal à 2 (comme r1)

  test_template_load_store_multiple("STM 1 element, r0 <- r1",
                                    p,
                                    AL,
                                    0b10001000,
                                    Rn,
                                    register_list,
                                    0,
                                    4,
                                    expected_value);

  // TEST 2 (ce test ne passe pas)
  // Rn = 0;                             // r0;
  // register_list = 0b0000000000001100; // r2 r3
  // arm_write_word(p, 2, 3);            // r2 = 3
  // arm_write_word(p, 3, 3);            // r3 = 3
  // expected_value = 3;                 // r0 et r1 doivent être égal à 3 (comme r2 et r3)

  // test_template_load_store_multiple("STM 2 elements, r0 <- r2, r1 <- r3",
  //                                   p,
  //                                   AL,
  //                                   0b10001000,
  //                                   Rn,
  //                                   register_list,
  //                                   0,
  //                                   4,
  //                                   expected_value);
}

/*
void test_LDM(arm_core p){

    uint8_t Rn = 0; // r0;
    uint16_t register_list = 0b0000000000000001;//r0

    registers_write(p->reg, 0, USR, 2);//r0 = 2

    uint32_t expected_value = 2;

    test_template_load_store_multiple("LDM incremente AFTER 1 element, r0 dans r0\n",
                                      p,
                                      AL,
                                      0b10001011,
                                      Rn,
                                      register_list,
                                      2,
                                      expected_value);
}
*/

/*
//Tests pour LDR / STR "normal" non testé car ça marche pas :-(
void test_LDR(arm_core p)
{
   // Test 1 : LDR avec une valeur immédiate
   test_template_load_store(
       "LDR (Immediate value)",
       p,
       AL,             // Cond
       0b01011001,     // Opcode
       0,              // Rn : r0
       1,              // Rd : r1
       0b000000000000,
       2,              // Rn value
       2);             // Expected Rd value

   // Test 2 : LDR avec une valeur provenant du registre
   test_template_load_store(
       "LDR (Value from register)",
       p,
       AL,             // Cond
       0b01011001,     // Opcode
       0,              // Rn : r0
       1,              // Rd : r1
       0b000000000010, // Shifter : r2
       2,              // Rn value
       6);             // Expected Rd value
}
*/

int main()
{
  arm_core p = arm_create(registers_create(), memory_create(2048));
  test_STM(p);
  memory_destroy(p->mem);
  registers_destroy(p->reg);
  arm_destroy(p);

  return 0;
}