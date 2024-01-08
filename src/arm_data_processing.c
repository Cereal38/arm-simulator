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
#include <stdio.h>
#include <stdlib.h>

uint32_t rotate_right(uint32_t value, uint8_t rotate)
{
  return (value >> rotate) | (value << (32 - rotate));
}

uint8_t overflow_from(uint32_t rn, uint32_t shifter_operand, int c_flag, int add)
{
  /*
    Returns 1 if the addition or subtraction specified as its parameter caused a 32-bit signed overflow.

    Param rn: The first operand of the addition or subtraction.
    Param shifter_operand: The second operand of the addition or subtraction.
    Param c_flag: The value of the C flag (-1 if the C flag is not considered).
    Param add: 1 if the operation is an addition, 0 if it is a subtraction.
  */
  uint8_t rn_sign = get_bit(rn, 31);
  uint8_t shifter_operand_sign = get_bit(shifter_operand, 31);
  uint8_t c_flag_value;
  if (add)
  {
    c_flag_value = c_flag == -1 ? 0 : c_flag;
    return (rn_sign == shifter_operand_sign) && (rn_sign != get_bit(rn + shifter_operand + c_flag_value, 31));
  }
  else
  {
    c_flag_value = c_flag == -1 ? 1 : c_flag;
    return (rn_sign != shifter_operand_sign) && (rn_sign != get_bit(rn - shifter_operand - !c_flag_value, 31));
  }
}

uint8_t carry_from(uint64_t rn, uint64_t shifter_operand, int c_flag)
{
  /*
    Returns 1 if the addition specified as its parameter caused a carry (true result is bigger than 232−1, where
    the operands are treated as unsigned integers), and returns 0 in all other cases.

    Param rn: The first operand of the addition.
    Param shifter_operand: The second operand of the addition.
    Param c_flag: The value of the C flag (-1 if the C flag is not considered).
  */
  uint64_t c_flag_value = c_flag == -1 ? 0 : c_flag;
  return (rn + shifter_operand + c_flag_value) > 0xFFFFFFFF;
}

uint8_t borrow_from(uint32_t rn, uint32_t shifter_operand, int c_flag)
{
  /*
    Returns 1 if the subtraction specified as its parameter caused a borrow (the true result is less than 0, where
    the operands are treated as unsigned integers), and returns 0 in all other cases.

    Param rn: The first operand of the subtraction.
    Param shifter_operand: The second operand of the subtraction.
    Param c_flag: The value of the C flag (-1 if the C flag is not considered).
  */
  uint8_t c_flag_value = c_flag == -1 ? 1 : c_flag;
  return rn < shifter_operand + !c_flag_value;
}

uint32_t logical_shift_left(uint32_t value, uint8_t shift)
{
  /*
    Returns the result of a logical shift left operation on the value specified as its parameter.

    Param value: The value to shift.
    Param shift: The number of bits to shift by.
  */
  return value << shift;
}

uint32_t logical_shift_right(uint32_t value, uint8_t shift)
{
  /*
    Returns the result of a logical shift right operation on the value specified as its parameter.

    Param value: The value to shift.
    Param shift: The number of bits to shift by.
  */
  return value >> shift;
}

uint32_t arithmetic_shift_right(uint32_t value, uint8_t shift)
{
  /*
    Performs a right shift, repeatedly inserting the original left-most bit (the sign bit) in the vacated bit positions
    on the left.

    Param value: The value to shift.
    Param shift: The number of bits to shift by.
  */
  uint32_t sign_bit = get_bit(value, 31);
  uint32_t result = value >> shift;
  for (int i = 0; i < shift; i++)
  {
    result = result | (sign_bit << (31 - i));
  }
  return result;
}

int arm_data_processing_immediate(arm_core p, uint32_t ins)
{

  // If all three of the following bits have the values shown, the instruction is not a data-processing instruction,
  // but lies in the arithmetic or Load/Store instruction extension space
  if (!get_bit(ins, 25) && get_bit(ins, 7) && get_bit(ins, 4))
  {
    fprintf(stderr, "Instruction is not a data-processing instruction\n");
    exit(EXIT_FAILURE);
  }

  // ---------- START CHECK CONDITION ----------
  if (!verif_cond(ins, p->reg))
  {
    return 0;
  }
  // ---------- END CHECK CONDITION ------------

  uint8_t opcode = get_bits(ins, 24, 21);
  uint8_t rn_code = get_bits(ins, 19, 16);
  uint8_t rd_code = get_bits(ins, 15, 12);
  uint8_t s_code = get_bits(ins, 20, 20);
  uint8_t i_code = get_bits(ins, 25, 25);
  uint8_t mode = registers_get_mode(p->reg);
  uint32_t rn = registers_read(p->reg, rn_code, mode);
  uint32_t result;
  uint32_t shifter_operand;
  uint8_t shifter_carry_out = 0;

  // ---------- START SHIFT OPERAND ----------
  // 32-bit immediate
  if (i_code == 1)
  {
    uint8_t immed_8 = get_bits(ins, 7, 0);
    uint8_t rotate_imm = get_bits(ins, 11, 8);
    shifter_operand = rotate_right(immed_8, rotate_imm * 2);
    if (rotate_imm == 0)
    {
      shifter_carry_out = registers_read_C(p->reg);
    }
    else
    {
      shifter_carry_out = get_bit(shifter_operand, 31);
    }
  }
  // Immediate shifts
  else if (get_bit(ins, 4) == 0)
  {
    uint8_t shift_imm = get_bits(ins, 11, 7);
    uint8_t shift = get_bits(ins, 6, 5);
    uint8_t rm = get_bits(ins, 3, 0);
    uint32_t rm_value = registers_read(p->reg, rm, mode);
    switch (shift)
    {
    case LSL:
      if (shift_imm == 0)
      {
        shifter_operand = rm_value;
        shifter_carry_out = registers_read_C(p->reg);
      }
      else
      {

        shifter_operand = logical_shift_left(rm_value, shift_imm);
        shifter_carry_out = get_bit(rm_value, 32 - shift_imm);
      }
      break;
    case LSR:
      if (shift_imm == 0)
      {
        shifter_operand = logical_shift_right(rm_value, 32);
        shifter_carry_out = get_bit(rm_value, 31);
      }
      else
      {
        shifter_operand = logical_shift_right(rm_value, shift_imm);
        shifter_carry_out = get_bit(rm_value, shift_imm - 1);
      }
      break;
    case ASR:
      if (shift_imm == 0)
      {
        if (get_bit(rm_value, 31) == 0)
        {
          shifter_operand = 0;
          shifter_carry_out = 0;
        }
        else
        {
          shifter_operand = 0xFFFFFFFF;
          shifter_carry_out = 1;
        }
      }
      else
      {
        shifter_operand = arithmetic_shift_right(rm_value, shift_imm);
        shifter_carry_out = get_bit(rm_value, shift_imm - 1);
      }
      break;
    case ROR:
      if (shift_imm == 0) // RRX
      {
        shifter_operand = (logical_shift_left(registers_read_C(p->reg), 31)) | (logical_shift_right(rm_value, 1));
        shifter_carry_out = get_bit(rm_value, 0);
      }
      else
      {
        shifter_operand = rotate_right(rm_value, shift_imm);
        shifter_carry_out = get_bit(rm_value, shift_imm - 1);
      }
      break;
    default:
      return UNDEFINED_INSTRUCTION;
    }
  }
  // Register shifts
  else
  {
    uint8_t rm = get_bits(ins, 3, 0);
    uint8_t shift = get_bits(ins, 6, 5);
    uint8_t rs = get_bits(ins, 11, 8);
    uint32_t rm_value = registers_read(p->reg, rm, mode);
    uint32_t rs_value = registers_read(p->reg, rs, mode);
    uint8_t rs_value_8bits = get_bits(rs_value, 7, 0);
    uint8_t rs_value_5bits = get_bits(rs_value, 4, 0);
    switch (shift)
    {
    case LSL:
      if (rs_value_8bits == 0)
      {
        shifter_operand = rm_value;
        shifter_carry_out = registers_read_C(p->reg);
      }
      else if (rs_value_8bits < 32)
      {
        shifter_operand = logical_shift_left(rm_value, rs_value_8bits);
        shifter_carry_out = get_bit(rm_value, 32 - rs_value_8bits);
      }
      else if (rs_value_8bits == 32)
      {
        shifter_operand = 0;
        shifter_carry_out = get_bit(rm_value, 0);
      }
      else
      {
        shifter_operand = 0;
        shifter_carry_out = 0;
      }
      break;
    case LSR:
      if (rs_value_8bits == 0)
      {
        shifter_operand = rm_value;
        shifter_carry_out = registers_read_C(p->reg);
      }
      else if (rs_value_8bits < 32)
      {
        shifter_operand = logical_shift_right(rm_value, rs_value_8bits);
        shifter_carry_out = get_bit(rm_value, rs_value_8bits - 1);
      }
      else if (rs_value_8bits == 32)
      {
        shifter_operand = 0;
        shifter_carry_out = get_bit(rm_value, 31);
      }
      else
      {
        shifter_operand = 0;
        shifter_carry_out = 0;
      }
      break;
    case ASR:
      if (rs_value_8bits == 0)
      {
        shifter_operand = rm_value;
        shifter_carry_out = registers_read_C(p->reg);
      }
      else if (rs_value_8bits < 32)
      {
        shifter_operand = arithmetic_shift_right(rm_value, rs_value_8bits);
        shifter_carry_out = get_bit(rm_value, rs_value_8bits - 1);
      }
      else
      {
        if (get_bit(rm_value, 31) == 0)
        {
          shifter_operand = 0;
          shifter_carry_out = get_bit(rm_value, 31);
        }
        else
        {
          shifter_operand = 0xFFFFFFFF;
          shifter_carry_out = get_bit(rm_value, 31);
        }
      }
      break;
    case ROR:
      if (rs_value_8bits == 0)
      {
        shifter_operand = rm_value;
        shifter_carry_out = registers_read_C(p->reg);
      }
      else if (rs_value_5bits == 0)
      {
        shifter_operand = rm_value;
        shifter_carry_out = get_bit(rm_value, 31);
      }
      else
      {
        shifter_operand = rotate_right(rm_value, rs_value_5bits);
        shifter_carry_out = get_bit(rm_value, rs_value_5bits - 1);
      }
      break;
    default:
      return UNDEFINED_INSTRUCTION;
    }
  }
  // ---------- END SHIFT OPERAND ----------

  // ---------- START COMPUTE RESULT ----------
  switch (opcode)
  {
  case AND:
    result = rn & shifter_operand;
    break;
  case EOR:
    result = rn ^ shifter_operand;
    break;
  case SUB:
    result = rn - shifter_operand;
    break;
  case RSB:
    result = shifter_operand - rn;
    break;
  case ADD:
    result = rn + shifter_operand;
    break;
  case ADC:
    result = rn + shifter_operand + registers_read_C(p->reg);
    break;
  case SBC:
    result = rn - shifter_operand - !registers_read_C(p->reg);
    break;
  case RSC:
    result = shifter_operand - rn - !registers_read_C(p->reg);
    break;
  case TST:
    result = rn & shifter_operand;
    break;
  case TEQ:
    result = rn ^ shifter_operand;
    break;
  case CMP:
    result = rn - shifter_operand;
    break;
  case CMN:
    result = rn + shifter_operand;
    break;
  case ORR:
    result = rn | shifter_operand;
    break;
  case MOV:
    result = shifter_operand;
    break;
  case BIC:
    result = rn & ~shifter_operand;
    break;
  case MVN:
    result = ~shifter_operand;
    break;
  default:
    return UNDEFINED_INSTRUCTION;
  }
  // ---------- END COMPUTE RESULT ----------

  // ---------- START WRITE RESULT AND SET FLAGS ----------
  if (s_code == 1)
  {
    registers_write_N(p->reg, get_bit(result, 31));
    registers_write_Z(p->reg, (result == 0) ? 1 : 0);
    switch (opcode)
    {
    case AND:
      registers_write_C(p->reg, shifter_carry_out);
      registers_write(p->reg, rd_code, mode, result);
      break;
    case EOR:
      registers_write_C(p->reg, shifter_carry_out);
      registers_write(p->reg, rd_code, mode, result);
      break;
    case SUB:
      registers_write_C(p->reg, !borrow_from(rn, shifter_operand, -1));
      registers_write_V(p->reg, overflow_from(rn, shifter_operand, -1, 0));
      registers_write(p->reg, rd_code, mode, result);
      break;
    case RSB:
      registers_write_C(p->reg, !borrow_from(shifter_operand, rn, -1));
      registers_write_V(p->reg, overflow_from(rn, shifter_operand, -1, 0));
      registers_write(p->reg, rd_code, mode, result);
      break;
    case ADD:
      registers_write_C(p->reg, carry_from(rn, shifter_operand, -1));
      registers_write_V(p->reg, overflow_from(rn, shifter_operand, -1, 1));
      registers_write(p->reg, rd_code, mode, result);
      break;
    case ADC:
    {
      uint8_t c_flag = registers_read_C(p->reg);
      registers_write_C(p->reg, carry_from(rn, shifter_operand, c_flag));
      registers_write_V(p->reg, overflow_from(rn, shifter_operand, c_flag, 1));
      registers_write(p->reg, rd_code, mode, result);
    }
      break;
    case SBC:
    {
      uint8_t c_flag = registers_read_C(p->reg);
      registers_write_C(p->reg, !borrow_from(rn, shifter_operand, c_flag));
      registers_write_V(p->reg, overflow_from(rn, shifter_operand, c_flag, 0));
      registers_write(p->reg, rd_code, mode, result);
    }
      break;
    case RSC:
    {
      uint8_t c_flag = registers_read_C(p->reg);
      registers_write_C(p->reg, !borrow_from(shifter_operand, rn, c_flag));
      registers_write_V(p->reg, overflow_from(shifter_operand, rn, c_flag, 0));
      registers_write(p->reg, rd_code, mode, result);
    }
      break;
    case TST:
      registers_write_C(p->reg, shifter_carry_out);
      break;
    case TEQ:
      registers_write_C(p->reg, shifter_carry_out);
      break;
    case CMP:
      registers_write_C(p->reg, !borrow_from(rn, shifter_operand, -1));
      registers_write_V(p->reg, overflow_from(rn, shifter_operand, -1, 0));
      break;
    case CMN:
      registers_write_C(p->reg, carry_from(rn, shifter_operand, -1));
      registers_write_V(p->reg, overflow_from(rn, shifter_operand, -1, 1));
      break;
    case ORR:
      registers_write_C(p->reg, shifter_carry_out);
      registers_write(p->reg, rd_code, mode, result);
      break;
    case MOV:
      registers_write_C(p->reg, shifter_carry_out);
      registers_write(p->reg, rd_code, mode, result);
      break;
    case BIC:
      registers_write_C(p->reg, shifter_carry_out);
      registers_write(p->reg, rd_code, mode, result);
      break;
    case MVN:
      registers_write_C(p->reg, shifter_carry_out);
      registers_write(p->reg, rd_code, mode, result);
    default:
      return UNDEFINED_INSTRUCTION;
    }
  }
  // ---------- END WRITE RESULT AND SET FLAGS ----------

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
