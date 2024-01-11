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
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "util.h"
#include "debug.h"

int arm_load_store(arm_core p, uint32_t ins)
{
  // Les positions
  uint8_t bitI = get_bit(ins, 25);
  uint8_t bitP = get_bit(ins, 24);
  uint8_t bitU = get_bit(ins, 23);
  uint8_t bitB = get_bit(ins, 22);
  uint8_t bitW = get_bit(ins, 21);
  uint8_t bitL = get_bit(ins, 20);
  uint8_t rn = get_bits(ins, 19, 16);
  uint8_t rd = get_bits(ins, 15, 12);
  uint8_t endianess = is_big_endian();
  uint8_t mode = registers_get_mode(p->reg);


  uint32_t address;
  

  switch (bitL)
  {
  // Load instructions
  case 1:
    // LDRH

    if (bitI == 0 && get_bits(ins, 27, 26) == 0b00)
    {
      
      uint16_t dataHalf;
      uint32_t *addressRn = registers_get_address(p, rn);
      int res = arm_read_half(p, *addressRn, &dataHalf);
      arm_write_half(p, rd, dataHalf);

    }

    // LDRB
    else if (bitB == 1 && get_bits(ins, 27, 26) == 0b01)
    {

      uint8_t dataByte;
      uint32_t *addressRn = registers_get_address(p, rn);
      printf("addressRn = %ls\n", addressRn);
      int res = arm_read_byte(p, *addressRn ,&dataByte);
      arm_write_byte(p, rd, dataByte);

    }

    // LDR
    else if (bitB == 0 && get_bits(ins, 27, 26) == 0b01)
    {
      // data = Memory[address,4]
      uint32_t dataWord;
      uint32_t *addressRn = registers_get_address(p,rn);
      int res = arm_read_word(p, *addressRn ,&dataWord);



      if (rd == 15)
      {
        // PC = data AND 0xFFFFFFFE
        // T Bit = data[0]
        arm_write_word(p,rd, (dataWord & 0xFFFFFFFE));
        //registers_write(p->reg, 15, mode, dataWord & 0xFFFFFFFE);
        registers_write_T(p->reg, get_bit(dataWord, 0));
      }
      else
      {
        // Rd = data
        //registers_write(p->reg, rd, mode, dataWord);
        arm_write_word(p, rd, dataWord);
      }
    }

  case 0:
    // STRH
    if (bitI == 0 && get_bits(ins, 27, 26) == 0b00)
    {
      // Memory[address,2] = Rd[15:0]
      address = arm_read_register(p, rn);
      uint16_t data = arm_read_register(p, rd) & 0xFFFF;
      if (memory_write_half(p->mem, address, data, endianess) != 0)
      {
        return UNDEFINED_INSTRUCTION;
      }
    }

    // STRB
    else if (bitB == 1 && get_bits(ins, 27, 26) == 0b01)
    {
      // Memory[address,1] = Rd[7:0]
      address = arm_read_register(p, rn);
      uint8_t data = arm_read_register(p, rd) & 0xFF;
      if (memory_write_byte(p->mem, address, data) != 0)
      {
        return UNDEFINED_INSTRUCTION;
      }
    }

    // STR
    else if (bitB == 0 && get_bits(ins, 27, 26) == 0b01)
    {
      // Memory[address,4] = Rd
      address = arm_read_register(p, rn);
      uint32_t data = arm_read_register(p, rd);
      if (memory_write_word(p->mem, address, data, endianess) != 0)
      {
        return UNDEFINED_INSTRUCTION;
      }
    }

  default:
    return UNDEFINED_INSTRUCTION;
  }
}

int number_registers(uint16_t register_list)
{
  int compteur;
  for (int i = 0; i < 16; i++)
  {
    if ((register_list & (1 << i)) != 0)
      compteur++;
  }
  return compteur;
}

// LDM et STM info plus générale page 134.
// LDM(1) (apparement c'est lui qu'il faut faire) page 186 du manuel.
// STM(1) page 339 du Manuel.
int arm_load_store_multiple(arm_core p, uint32_t ins)
{

  uint8_t P = get_bit(ins, 24);
  uint8_t U = get_bit(ins, 23);
  uint8_t S = get_bit(ins, 22);
  uint8_t W = get_bit(ins, 21);
  uint8_t L = get_bit(ins, 20);

  uint8_t rn = get_bits(ins, 19, 16);
  uint16_t register_list = get_bits(ins, 15, 0);
  
  uint8_t endianess = is_big_endian();
  uint8_t mode = registers_get_mode(p->reg);

  uint32_t address = registers_read(p->reg, rn, registers_get_mode(p->reg));

  int nbr_register_list = number_registers(register_list);

  if (nbr_register_list == 0)
  {
    fprintf(stderr, "<arm_load_store.c> Erreur aucun bit n'est set dans la liste de registres.\n");
    return UNDEFINED_INSTRUCTION;
  }

  if (S != 0)
  {
    fprintf(stderr, "<arm_load_store.c> Erreur le bit S n'est pas à 0.\n");
    return UNDEFINED_INSTRUCTION;
  }
  for (int i = 0; i < 16; i++)
  {
    if ((register_list & (1 << i)) != 0)
    {
      if (P == 1)
      {
        if (U == 0) // decremente BEFORE
          address -= 4;
        else // incremente BEFORE
          address += 4;
      }
      if (!L)
      { // STM(1)
        uint32_t data;
        arm_read_word(p, i, &data);
        arm_write_word(p, address, data);
      }
      else
      { // LDM(1)
        uint32_t data;
        arm_read_word(p, address, &data);
        arm_write_word(p, i, data);
      }
      if (P == 0)
      {
        if (U == 0) // decremente AFTER
          address -= 4;
        else // incremente AFTER
          address += 4;
      }
    }
  }
  if (W == 1)
  {
    arm_write_register(p, rn, address); // address sera déjà à la bonne valeur normalement (address +/- 4 * nbr_register_list)
  }
  return 0;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins)
{
  uint8_t posP = 24;
  uint8_t posU = 23;
  uint8_t posW = 21;
  uint8_t posL = 20;

  uint8_t P = (ins >> posP) & 0b1;
  uint8_t U = (ins >> posU) & 0b1;
  uint8_t W = (ins >> posW) & 0b1;
  uint8_t L = (ins >> posL) & 0b1;

  uint8_t posRn = 16; // 19 à 16
  uint8_t rn = (ins >> posRn) & 0b1111;

  uint8_t pos_cp_num = 8;
  uint8_t cp_num = (ins >> pos_cp_num) & 0b1111; // 11 à 8

  uint8_t pos_CRd = 12;
  uint8_t CRd = (ins >> pos_CRd) & 0b1111; // 12 à 15

  uint8_t offset_8 = ins & 0b111111111; // 7 à 0

  uint32_t base = arm_read_register(p, rn);

  if (P == 1 && W == 0)
  { // Immedite offset
    return UNDEFINED_INSTRUCTION;
  }
  if (P == 1 && W == 1)
  { // Immediate pre-indexed
    return UNDEFINED_INSTRUCTION;
  }
  if (P == 0 && W == 1)
  { // Immediate post-indexed
    return UNDEFINED_INSTRUCTION;
  }
  else
  { // Unindexed
    return UNDEFINED_INSTRUCTION;
  }
  return UNDEFINED_INSTRUCTION;
}

