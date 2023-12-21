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
#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "arm_instruction.h"
#include "util.h"
#include "debug.h"

int get_rn(uint32_t ins)
{
  // Return the value of Rn (Bits 19..16)
  return (ins >> 16) & 0b1111;
}

int get_rd(uint32_t ins)
{
  // Return the value of Rd (Bits 15..12)
  return (ins >> 12) & 0b1111;
}

int get_s(uint32_t ins)
{
  // Return the value of S bit (Bit 20)
  return (ins >> 20) & 0b1;
}

int get_shifter_operand(uint32_t ins)
{
  // Return the value of shifter operand (Bits 11..0)
  return (ins & 0xFFF);
}

// TODO: VERIFY THIS:
// If the I bit is 0 and both bit[7] and bit[4] of shifter_operand are 1, the instruction is not ADD.
// Instead, see Extending the instruction set on page A3-32 to determine which instruction it is.
void arm_data_processing_add(arm_core p, uint32_t ins)
{

  // Check condition
  if (!verif_cond(ins, p->reg))
  {
    return;
  }

  uint8_t rn_code = get_rn(ins);
  uint8_t rd_code = get_rd(ins);
  uint8_t s_code = get_s(ins);
  uint32_t shifter_operand = get_shifter_operand(ins);
  uint8_t mode = registers_get_mode(p->reg);

  // Get Rn value
  uint32_t rn = registers_read(p->reg, rn_code, mode);

  // Set Rd value
  uint32_t rd = rn + shifter_operand;
  registers_write(p->reg, rd_code, mode, rd);

  // Set CPSR if needed
  if (s_code == 1 && rd_code == 15)
  {
    if (registers_current_mode_has_spsr(p->reg))
    {
      registers_write_cpsr(p->reg, registers_read_spsr(p->reg, mode));
    }
  }
  else if (s_code == 1)
  {
    // Edit N, Z, C, V flags
    registers_write_N(p->reg, (rd >> 31) & 0b1);
    registers_write_Z(p->reg, (rd == 0) ? 1 : 0);

    // TODO: Verify if this implementation is correct (Carry and Overflow flags)
    // Set the Carry flag based on unsigned overflow
    registers_write_C(p->reg, (rn + shifter_operand) < rn ? 1 : 0);

    // Set the Overflow flag based on signed overflow
    int32_t result = (int32_t)rn + (int32_t)shifter_operand;
    registers_write_V(p->reg, (result < rn) != (result < shifter_operand) ? 1 : 0);
  }
}

/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins)
{
  return UNDEFINED_INSTRUCTION;
}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins)
{
  return UNDEFINED_INSTRUCTION;
}
