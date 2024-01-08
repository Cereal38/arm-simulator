
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
    if (I == 0)
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

void test_LDR(arm_core p){
    
}

int main(){
    arm_core p = arm_create(registers_create(), memory_create(2048));

    memory_destroy(p->mem);
    registers_destroy(p->reg);
    arm_destroy(p);

    return 0;
}