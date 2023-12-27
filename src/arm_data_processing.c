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
#include "registers.h"
#include "debug.h"

int get_rn(uint32_t ins)
{
  return get_bits(ins, 19, 16);
}

int get_rd(uint32_t ins)
{
  return get_bits(ins, 15, 12);
}

int get_s(uint32_t ins)
{
  return get_bits(ins, 20, 20);
}

int get_i(uint32_t ins)
{
  return get_bits(ins, 25, 25);
}

uint32_t rotate_right(uint32_t value, uint8_t rotate)
{
  return (value >> rotate) | (value << (32 - rotate));
}

// TODO: VERIFY THIS:
// If the I bit is 0 and both bit[7] and bit[4] of shifter_operand are 1, the instruction is not ADD.
// Instead, see Extending the instruction set on page A3-32 to determine which instruction it is.
// TODO: Check bit 25 (register or immediate value)
// TODO: Write shift right function
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
  uint8_t i_code = get_i(ins);
  uint8_t mode = registers_get_mode(p->reg);
  uint32_t rn = registers_read(p->reg, rn_code, mode);
  uint32_t rd;

  // Shifter operand
  // Immediate value
  if (i_code == 1)
  {
    uint8_t immed_8 = get_bits(ins, 7, 0);
    uint8_t rotate_imm = get_bits(ins, 11, 8);
    uint32_t shifter_operand = rotate_right(immed_8, rotate_imm * 2);
    rd = rn + shifter_operand;
    if (s_code == 1)
    {
      // Edit N, Z, C, V flags
      registers_write_N(p->reg, get_bits(rd, 31, 31));
      registers_write_Z(p->reg, (rd == 0) ? 1 : 0);
      registers_write_C(p->reg, (rd < rn) ? 1 : 0);
      registers_write_V(p->reg, (get_bits(rn, 31, 31) == get_bits(shifter_operand, 31, 31) && get_bits(rd, 31, 31) != get_bits(rn, 31, 31)) ? 1 : 0);
    }
  }
  // Register value
  else
  {
    uint8_t rm_code = get_bits(ins, 3, 0);
    uint32_t rm = registers_read(p->reg, rm_code, mode);
    rd = rn + rm;
    if (s_code == 1)
    {
      // Edit N, Z, C, V flags
      registers_write_N(p->reg, get_bits(rd, 31, 31));
      registers_write_Z(p->reg, (rd == 0) ? 1 : 0);
      registers_write_C(p->reg, (rd < rn) ? 1 : 0);
      registers_write_V(p->reg, (get_bits(rn, 31, 31) == get_bits(rm, 31, 31) && get_bits(rd, 31, 31) != get_bits(rn, 31, 31)) ? 1 : 0);
    }
  }

  // Set Rd value
  registers_write(p->reg, rd_code, mode, rd);

  // Set CPSR if needed
  if (s_code == 1 && rd_code == 15)
  {
    if (registers_current_mode_has_spsr(p->reg))
    {
      registers_write_cpsr(p->reg, registers_read_spsr(p->reg, mode));
    }
  }
}

/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins)
{
  return UNDEFINED_INSTRUCTION;
}

// TODO: Merge add in it
int arm_data_processing_immediate_msr(arm_core p, uint32_t ins)
{
  return UNDEFINED_INSTRUCTION;
}
