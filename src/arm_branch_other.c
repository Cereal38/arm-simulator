/*
Armator - simulateur de jeu d'instruction ARMv5T � but p�dagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique G�n�rale GNU publi�e par la Free Software
Foundation (version 2 ou bien toute autre version ult�rieure choisie par vous).
Ce programme est distribu� car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but sp�cifique. Reportez-vous � la
Licence Publique G�n�rale GNU pour plus de d�tails.

Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU en m�me
temps que ce programme ; si ce n'est pas le cas, �crivez � la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
�tats-Unis.

Contact: Guillaume.Huard@imag.fr
	 B�timent IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'H�res
*/
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"
#include <debug.h>
#include <stdlib.h>


int arm_branch(arm_core p, uint32_t ins) {

  // Récupération de l'adresse de PC
  uint32_t address = registers_read(p->reg, 15, registers_get_mode(p->reg));

  // Vérification L ( with Link )
  if (get_bit(ins, 24)) {
    registers_write(p->reg, 14, registers_get_mode(p->reg), address);
  }

  // Récupération des 24 premiers bits
  int32_t immed = get_bits(ins, 23, 0);

  // Sign extension
  if (get_bit(immed, 23)) {
    immed = immed | (0x3F << 24);
  }

  // Décalage pour former 32 bits
  immed = immed << 2;

  // Mise à jour du PC
  registers_write(p->reg, 15, registers_get_mode(p->reg), address + immed - 4);

  // Retour
  return 0;
}
int arm_coprocessor_others_swi(arm_core p, uint32_t ins) {
    if (get_bit(ins, 24)) {
        return SOFTWARE_INTERRUPT;
    }
    return UNDEFINED_INSTRUCTION;
}

// TODO voir si la fonction ROR non definie ailleurs
uint8_t rotateRight8(uint8_t value, int rotateBy) {
    int size = sizeof(uint8_t) * 8; // Taille d'un octet en bits
    rotateBy = rotateBy % size; // Pour s'assurer que le décalage est dans la plage 0-7
    
    // Effectue la rotation vers la droite
    return (value >> rotateBy) | (value << (size - rotateBy));
}

int msr_instruction_commun_code(arm_core p, uint32_t ins, int8_t operand){
    if ((operand & UnallocMask) != 0) {
        // TODO Vérifier UNDEF Ou UNPREDICTABLE
        return UNDEFINED_INSTRUCTION;
    }

    int8_t field_mask = get_bits(ins, 19, 16);
    int8_t byte_mask = ((get_bit(field_mask, 0) == 1) ? 0x000000FF : 0x00000000) |
                       ((get_bit(field_mask, 1) == 1) ? 0x0000FF00 : 0x00000000) |
                       ((get_bit(field_mask, 2) == 1) ? 0x00FF0000 : 0x00000000) |
                       ((get_bit(field_mask, 3) == 1) ? 0xFF000000 : 0x00000000);

    uint32_t mask;  // Declare mask here

    if (!get_bit(ins, 22)) /*R = 0*/ {
        if (registers_in_a_privileged_mode(p->reg)) {
            if ((operand & StateMask) != 0) {
                // TODO Vérifier UNDEF Ou UNPREDICTABLE
                return UNDEFINED_INSTRUCTION;
            } else {
                mask = byte_mask & (UserMask | PrivMask);
            }
        } else {
            mask = byte_mask & UserMask;
        }
        uint32_t CPSR_register = registers_read_cpsr(p->reg);
        CPSR_register = (CPSR_register & (~mask)) | (operand & mask);
        registers_write_cpsr(p->reg, CPSR_register);
    } else /* R == 1 */ {
        if (arm_current_mode_has_spsr(p)) {
            mask = byte_mask & (UserMask | PrivMask | StateMask);
            uint8_t mode = registers_get_mode(p->reg);
            uint32_t SPSR_register = registers_read_spsr(p->reg, mode);
            SPSR_register = (SPSR_register & (~mask)) | (operand & mask);
            registers_write_spsr(p->reg, mode, SPSR_register);
        } else {
            // TODO Vérifier UNDEF Ou UNPREDICTABLE
            return UNDEFINED_INSTRUCTION;
        }
    }
    // TODO Vérifier la valeur de retour
    return 0;
}

int mrs_instruction (arm_core p, uint32_t ins){
    uint8_t rd = get_bits(ins,15,12);
    if (rd == 15) {
        // TODO Verifier UNDEF ou UNPREDICTABLE
        return UNDEFINED_INSTRUCTION;
    }

    if( get_bit(ins,22) ) /* R == 1 */ {
        uint8_t mode = registers_get_mode(p->reg);
        uint32_t SPSR_register = registers_read_spsr(p->reg, mode);
        arm_write_register(p,rd,SPSR_register);
    } else /* R == 0 */ {
        uint32_t CPSR_register = registers_read_cpsr(p->reg);
        arm_write_register(p,rd,CPSR_register);
    }
    return 0;
}
int arm_miscellaneous(arm_core p, uint32_t ins) {

    if (get_bit(ins,21) & (get_bits(ins,7,4)==0)){
        // Cas MSR Register operand
        int8_t operand = get_bits(ins, 3, 0);
        int result = msr_instruction_commun_code(p, ins, operand);
        return result;
    }

    // MRS
    int result = mrs_instruction(p,ins);
    return result;
}
