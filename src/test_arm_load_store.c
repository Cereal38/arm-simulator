
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
    //printf("Before execution, Rn = %u\n", registers_read(p->reg, Rn, USR));
    arm_load_store(p, ins);
    //printf("After execution, Rd = %u\n", registers_read(p->reg, Rd, USR));
    // Check result
    assert(registers_read(p->reg, Rd, USR) == expected_Rd);
}

void test_template_load_store_multiple(
    char *name,
    arm_core p,
    uint8_t cond,  // Bits 31-28
    uint8_t code,  // Bits 27-20
    uint8_t Rn,    // Bits 19-16
    uint16_t register_list,//Bits 15-0
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
    //printf("Before execution, Rn = %u\n", registers_read(p->reg, Rn, USR));
    arm_load_store(p, ins);
    //printf("After execution, Rd = %u\n", registers_read(p->reg, Rd, USR));
    // Check result
    assert(registers_read(p->reg, Rd, USR) == expected_Rd);
}

void test_LDM(arm_core p){
    
}

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

int main(){
    arm_core p = arm_create(registers_create(), memory_create(2048));
    test_LDM(p);
    memory_destroy(p->mem);
    registers_destroy(p->reg);
    arm_destroy(p);

    return 0;
}