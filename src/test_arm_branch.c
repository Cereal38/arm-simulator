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
#define PC_INIT_VAL 0x00000008 
#define LR_INIT_VAL 0x00000000

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
    assert( pc == expected_PC);

    printf("OK\n");
}

void test_B_inconditionnel (arm_core p) {

    // Cas avec une adresse positive
    test_template_branch(
        " B Inconditionnel (Adresse Positive) ",
        p,
        AL,     // cond
        0,      // bit L
        0x000003,      // signed_immed
        0x1c,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );

    test_template_branch(
        " B Inconditionnel (Adresse Negative) ",
        p,
        AL,     // cond
        0,      // bit L
        0xfffffc,      // signed_immed
        0x0,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );
}

void test_B_conditionnel (arm_core p) {
    // Cas avec une adresse positive et branchement si égal (Z set)
    test_template_branch(
        " B Égal (EQ) ",
        p,
        EQ,     // cond
        0,      // bit L
        0x000003,      // signed_immed
        0x1c,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );

    // Cas avec une adresse positive et branchement si non égal (Z clear)
    test_template_branch(
        " B Non Égal (NE) ",
        p,
        NE,     // cond
        0,      // bit L
        0x000003,      // signed_immed
        0x1c,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );
    // Cas avec une adresse positive et branchement si supérieur ou égal (C set ou Z set)
    test_template_branch(
        " B Supérieur ou Égal (CSHS) ",
        p,
        CSHS,   // cond
        0,      // bit L
        0x000003,      // signed_immed
        0x1c,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );

    // Cas avec une adresse positive et branchement si inférieur (C clear et Z clear)
    test_template_branch(
        " B Inférieur (CCLO) ",
        p,
        CCLO,   // cond
        0,      // bit L
        0x000003,      // signed_immed
        0x1c,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );

    // Cas avec une adresse positive et branchement si négatif (N set)
    test_template_branch(
        " B Négatif (MI) ",
        p,
        MI,     // cond
        0,      // bit L
        0x000003,      // signed_immed
        0x1c,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );
    // Cas avec une adresse positive et branchement si positif ou nul (N clear)
    test_template_branch(
        " B Positif ou Nul (PL) ",
        p,
        PL,     // cond
        0,      // bit L
        0x000003,      // signed_immed
        0x1c,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );
    // Cas avec une adresse positive et branchement si débordement (V set)
    test_template_branch(
        " B Débordement (VS) ",
        p,
        VS,     // cond
        0,      // bit L
        0x000003,      // signed_immed
        0x1c,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );

    // Cas avec une adresse positive et branchement si pas de débordement (V clear)
    test_template_branch(
        " B Pas de Débordement (VC) ",
        p,
        VC,     // cond
        0,      // bit L
        0x000003,      // signed_immed
        0x1c,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );

    // Cas avec une adresse positive et branchement si supérieur (C set et Z clear)
    test_template_branch(
        " B Supérieur (HI) ",
        p,
        HI,     // cond
        0,      // bit L
        0x000003,      // signed_immed
        0x1c,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );

    // Cas avec une adresse positive et branchement si inférieur ou égal (C clear ou Z set)
    test_template_branch(
        " B Inférieur ou Égal (LS) ",
        p,
        LS,     // cond
        0,      // bit L
        0x000003,      // signed_immed
        0x1c,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );

    // Cas avec une adresse positive et branchement si supérieur ou égal (N set et V set ou N clear et V clear)
    test_template_branch(
        " B Supérieur ou Égal (GE) ",
        p,
        GE,     // cond
        0,      // bit L
        0x000003,      // signed_immed
        0x1c,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );

    // Cas avec une adresse positive et branchement si inférieur (N set et V clear ou N clear et V set)
    test_template_branch(
        " B Inférieur (LT) ",
        p,
        LT,     // cond
        0,      // bit L
        0x000003,      // signed_immed
        0x1c,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );

    // Cas avec une adresse positive et branchement si supérieur (N set et V clear et Z clear ou N clear et V set et Z clear)
    test_template_branch(
        " B Supérieur (GT) ",
        p,
        GT,     // cond
        0,      // bit L
        0x000003,      // signed_immed
        0x1c,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );

    // Cas avec une adresse positive et branchement si inférieur ou égal (N set et V set et Z set ou N clear et V clear et Z set)
    test_template_branch(
        " B Inférieur ou Égal (LE) ",
        p,
        LE,     // cond
        0,      // bit L
        0x000003,      // signed_immed
        0x1c,  // expected_PC
        LR_INIT_VAL      // expected_LR
    );

}

void test_BL_inconditionnel(arm_core p) {
    test_template_branch(
        "BL Inconditionnel (Adresse Positive) ",
        p,
        AL,     // cond
        1,      // bit L
        0xea000003,      // signed_immed
        0x1c,  // expected_PC
        0x0c      // expected_LR
    );
    test_template_branch(
        "BL Inconditionnel (Adresse Negative) ",
        p,
        AL,     // cond
        1,      // bit L
        0xfffffc,      // signed_immed
        0x0,  // expected_PC
        PC_INIT_VAL + 4      // expected_LR
    );
    
}

void test_B (arm_core p) {
    test_B_inconditionnel(p);
    test_B_conditionnel(p);
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
