#include <stdio.h>
#include <assert.h>
#include "arm_data_processing.h"
#include "arm_branch_other.h"
#include "arm_core.h"
#include "registers.h"
#include "memory.h"
#include "arm_constants.h"
#include "util.h"

#define CODE_BL 0b101
#define PC_INIT_VAL 0x00000000 
#define LR_INIT_VAL 0x0000000C

void test_template_branch(char *name,
                          arm_core p,
                          uint8_t cond,
                          uint8_t l,
                          uint32_t signed_immed,
                          uint32_t expected_PC,
                          uint32_t expected_LR)
{
    printf("Test : %s ... ", name);

    // Initialiser la valeur de PC
    arm_write_register(p,15,PC_INIT_VAL);
    // Initialiser la veleur de LR
    arm_write_register(p,14,LR_INIT_VAL);


    uint32_t ins = (cond << 28) |  (CODE_BL << 25) | (l << 24) | signed_immed ;
    
    arm_branch(p, ins);

    // verifier que lr contient l'adresse suivante 
    uint32_t lr = arm_read_register(p,14);
    assert( lr == expected_LR );

    uint32_t pc = arm_read_register(p,15);
    // On compare avec PC + 4 car lors de la lecture du registre dans la fonction arm_read_register_internal de arm_core on incremente PC de 4
    assert( pc == expected_PC + 4);

    printf("OK\n");
}

void test_B_inconditionnel (arm_core p) {
    // Cas avec une adresse négative
    test_template_branch(
        " B Inconditionnel (Adresse Négative) ",
        p,
        AL,     // cond
        0,      // bit L
        -5,     // signed_immed
        PC_INIT_VAL - 5,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );

    // Cas avec une adresse positive
    test_template_branch(
        " B Inconditionnel (Adresse Positive) ",
        p,
        AL,     // cond
        0,      // bit L
        5,      // signed_immed
        20,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );

    // Cas avec une adresse proche de la limite positive
    test_template_branch(
        " B Inconditionnel (Adresse Limite Positive) ",
        p,
        AL,     // cond
        0,      // bit L
        0x3FFFFFC,  // signed_immed (proche de la limite positive)
        -4,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );

    // Cas avec une adresse proche de la limite négative
    test_template_branch(
        " B Inconditionnel (Adresse Limite Négative) ",
        p,
        AL,     // cond
        0,      // bit L
        -0x3FFFFFC,  // signed_immed (proche de la limite négative)
        PC_INIT_VAL - 0x3FFFFFC,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );
}


void test_BL_inconditionnel(arm_core p) {
    test_template_branch(
        "BL Inconditionnel avec LR",
        p,
        AL,     // cond
        1,      // bit L
        5,      // signed_immed
        20,     // expected_PC
        PC_INIT_VAL + 4      // expected_LR
    );
}

void test_B (arm_core p) {
    test_B_inconditionnel(p);
}

void test_BL (arm_core p) {
    test_BL_inconditionnel(p);
}

int main()
{
  arm_core p = arm_create(registers_create(), memory_create(2048));
  
  test_B (p);
  test_BL (p);
  
  memory_destroy(p->mem);
  registers_destroy(p->reg);
  arm_destroy(p);

  return 0;
}
