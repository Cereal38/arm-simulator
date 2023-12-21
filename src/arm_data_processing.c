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
#include "util.h"
#include "debug.h"

int get_rn(uint32_t ins)
{
  // Return the value of Rn (Bits 19.18.17.16)
  uint8_t posRn = 16; // 19 to 16
  uint8_t rn = (ins >> posRn) & 0b1111;
  return rn;
}

int get_rd(uint32_t ins)
{
  // Return the value of Rd (Bits 15.14.13.12)
  uint8_t posRd = 12; // 15 to 12
  uint8_t rd = (ins >> posRd) & 0b1111;
  return rd;
}

int get_s(uint32_t ins)
{
  // Return the value of S bit (Bit 20)
  uint8_t posS = 20; // 20
  uint8_t s = (ins >> posS) & 0b1;
  return s;
}

// TODO: Utiliser la fonction de arm_instruction.c (Pas sur master à l'heure qu'il est)
int verif_cond(uint32_t instruction, registers r)
{
  // Extraire les bits de condition (bits 28-31)
  uint8_t condition_bits = (uint8_t)((instruction >> 28) & 0xF);
  // récupération de l'état des flags
  uint32_t cpsr = registers_read_cpsr(r);

  switch (condition_bits)
  {
  case 0x0: // EQ (Z == 1)
    if ((cpsr & Z) != 0)
    {
      return 1;
    }
    return 0;
  case 0x1: // NE (Z == 0)
    if ((cpsr & Z) == 0)
    {
      return 1;
    }
    return 0;
  case 0x2: // CS/HS (C == 1)
    if ((cpsr & C) != 0)
    {
      return 1;
    }
    return 0;
  case 0x3: // CC/LO (C == 0)
    if ((cpsr & C) == 0)
    {
      return 1;
    }
    return 0;
  case 0x4: // MI (N == 1)
    if ((cpsr & N) != 0)
    {
      return 1;
    }
    return 0;
  case 0x5: // PL (N == 0)
    if ((cpsr & N) == 0)
    {
      return 1;
    }
    return 0;
  case 0x6: // VS (V == 1)
    if ((cpsr & V) != 0)
    {
      return 1;
    }
    return 0;
  case 0x7: // VC (V == 0)
    if ((cpsr & V) == 0)
    {
      return 1;
    }
    return 0;
  case 0x8: // HI (C == 1 && Z == 0)
    if (((cpsr & C) != 0) && ((cpsr & Z) == 0))
    {
      return 1;
    }
    return 0;
  case 0x9: // LS (C == 0 || Z == 1)
    if (((cpsr & C) == 0) || ((cpsr & Z) != 0))
    {
      return 1;
    }
    return 0;
  case 0xA: // GE (N == V)
    if ((cpsr & N) == (cpsr & V))
    {
      return 1;
    }
    return 0;
  case 0xB: // LT (N != V)
    if ((cpsr & N) != (cpsr & V))
    {
      return 1;
    }
    return 0;
  case 0xC: // GT (Z == 0 && N == V)
    if (((cpsr & Z) == 0) && ((cpsr & N) == (cpsr & V)))
    {
      return 1;
    }
    return 0;
  case 0xD: // LE (Z == 1 || N != V)
    if (((cpsr & Z) != 0) || ((cpsr & N) != (cpsr & V)))
    {
      return 1;
    }
    return 0;
  case 0xE: // AL (Always, toujours vrai)
    return 1;
  default:
    fprintf(stderr, "Condition inconnue : %d\n", condition_bits);
    return -1;
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

int arm_data_processing_add(arm_core p, uint32_t ins)
{

  // Check condition
  if (!verif_cond(ins, p->reg))
  {
    return 0;
  }

  uint8_t rn = get_rn(ins);
  uint8_t rd = get_rd(ins);
  uint8_t s = get_s(ins);

  return UNDEFINED_INSTRUCTION;
}
