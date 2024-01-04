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

uint32_t rotate_right(uint32_t value, uint8_t rotate)
{
  return (value >> rotate) | (value << (32 - rotate));
}

uint8_t overflow_from(uint32_t rn, uint32_t shifter_operand, int add)
{
  /*
    Returns 1 if the addition or subtraction specified as its parameter caused a 32-bit signed overflow.

    Param rn: The first operand of the addition or subtraction.
    Param shifter_operand: The second operand of the addition or subtraction.
    Param add: 1 if the operation is an addition, 0 if it is a subtraction.
  */
  uint8_t rn_sign = get_bit(rn, 31);
  uint8_t shifter_operand_sign = get_bit(shifter_operand, 31);
  if (add)
  {
    return (rn_sign == shifter_operand_sign) && (rn_sign != get_bit(rn + shifter_operand, 31));
  }
  else
  {
    return (rn_sign != shifter_operand_sign) && (rn_sign != get_bit(rn - shifter_operand, 31));
  }
}

uint8_t carry_from(uint32_t rn, uint32_t shifter_operand)
{
  /*
    Returns 1 if the addition specified as its parameter caused a carry (true result is bigger than 232−1, where
    the operands are treated as unsigned integers), and returns 0 in all other cases.
  */
  return (rn + shifter_operand) < rn;
}

uint8_t borrow_from(uint32_t rn, uint32_t shifter_operand)
{
  /*
    Returns 1 if the subtraction specified as its parameter caused a borrow (the true result is less than 0, where
    the operands are treated as unsigned integers), and returns 0 in all other cases.
  */
  return (rn - shifter_operand) > rn;
}

// TODO: VERIFY THIS:
// If the I bit is 0 and both bit[7] and bit[4] of shifter_operand are 1, the instruction is not ADD.
// Instead, see Extending the instruction set on page A3-32 to determine which instruction it is.
// TODO: Check A5.1.1 and correct register shift + implement immediate shift
int arm_data_processing_immediate(arm_core p, uint32_t ins)
{
  // Check condition
  if (!verif_cond(ins, p->reg))
  {
    return 0;
  }

  uint8_t opcode = get_bits(ins, 24, 21);
  uint8_t rn_code = get_bits(ins, 19, 16);
  uint8_t rd_code = get_bits(ins, 15, 12);
  uint8_t s_code = get_bits(ins, 20, 20);
  uint8_t i_code = get_bits(ins, 25, 25);
  uint8_t mode = registers_get_mode(p->reg);
  uint32_t rn = registers_read(p->reg, rn_code, mode);
  uint32_t rd;
  uint32_t right_value;

  // Shifter operand
  // Immediate value
  if (i_code == 1)
  {
    uint8_t immed_8 = get_bits(ins, 7, 0);
    uint8_t rotate_imm = get_bits(ins, 11, 8);
    right_value = rotate_right(immed_8, rotate_imm * 2);
  }
  // Register value
  else
  {
    uint8_t rm_code = get_bits(ins, 3, 0);
    right_value = registers_read(p->reg, rm_code, mode);
  }

  // Set Rd
  switch (opcode)
  {
  case AND:
    rd = rn & right_value;
    break;
  case EOR:
    rd = rn ^ right_value;
    break;
  case SUB:
    rd = rn + (~right_value + 1);
    break;
  case RSB:
    rd = right_value + (~rn + 1);
    break;
  case ADD:
    rd = rn + right_value;
    break;
  default:
    return UNDEFINED_INSTRUCTION;
  }

  // Edit N, Z, C, V flags
  if (s_code == 1)
  {
    registers_write_N(p->reg, get_bit(rd, 31));
    registers_write_Z(p->reg, (rd == 0) ? 1 : 0);
    switch (opcode)
    {
    case AND:
      // TODO: "C Flag = shifter_carry_out" (p159) ?
      break;
    case EOR:
      // TODO: "C Flag = shifter_carry_out" (p183) ?
      break;
    case SUB:
      registers_write_C(p->reg, !borrow_from(rn, right_value));
      registers_write_V(p->reg, overflow_from(rn, right_value, 0));
      break;
    case RSB:
      registers_write_C(p->reg, !borrow_from(right_value, rn));
      registers_write_V(p->reg, overflow_from(rn, right_value, 0));
      break;
    case ADD:
      registers_write_C(p->reg, carry_from(rn, right_value));
      registers_write_V(p->reg, overflow_from(rn, right_value, 1));
      break;
    default:
      return UNDEFINED_INSTRUCTION;
    }
  }
  registers_write(p->reg, rd_code, mode, rd);

  // Set CPSR if needed
  if (s_code == 1 && rd_code == 15)
  {
    if (registers_current_mode_has_spsr(p->reg))
    {
      registers_write_cpsr(p->reg, registers_read_spsr(p->reg, mode));
    }
  }

  return 1;
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
