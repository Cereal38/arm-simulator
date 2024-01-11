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
  // uint8_t bitW = get_bit(ins, 21);
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
      // man P.474
      int offset = get_bits(ins, 11, 0);
      uint8_t immedH = get_bits(ins, 11, 8);
      uint8_t rm = get_bits(ins, 3, 0);
      offset = (immedH << 4) | rm;

      if (bitP)
      { // val imm
        if (bitU)
        {
          address = arm_read_register(p, rn) + offset;
        }
        else
        {
          address = arm_read_register(p, rn) - offset;
        }
      }
      else
      { // val reg
        if (bitU)
        {
          address = arm_read_register(p, rn) + arm_read_register(p, rm);
        }
        else
        {
          address = arm_read_register(p, rn) - arm_read_register(p, rm);
        }
      }
      uint16_t data;
      data = (uint16_t)registers_read(p->reg, rn, mode);
      arm_write_register(p, rd, data);
    }

    // LDRB
    else if (bitB == 1 && get_bits(ins, 27, 26) == 0b01)
    {
      address = registers_read(p->reg, rn, mode);
      uint8_t data;
      arm_read_byte(p, address, (uint8_t *)&data);
      if (rd == 15)
      {
        // PC = data AND 0xFFFFFFFE
        // T Bit = data[0]
        registers_write(p->reg, 15, mode, data & 0xFFFFFFFE);
        registers_write_T(p->reg, get_bit(data, 0));
      }
      else
      {
        // Rd = data
        // if (rn == 15)
        // {
        //   data += 8;
        //   //  dans le cas ou rn = 15, l'addresse de ma varible  je la connais pas  je sais pas ou est ce
        //   // le compilateur va la mettre
        // }

        registers_write(p->reg, rd, mode, data);
        // on affiche la valeur a l'addrss de rd
        // int memory_read_word(memory mem, uint32_t address, uint32_t *value, uint8_t be)

        uint32_t val = 0;
        memory_read_word(p->mem, data, &val, endianess);
        // printf ("val = %x\n", val);
      }
    }

    // LDR
    else if (bitB == 0 && get_bits(ins, 27, 26) == 0b01)
    {
      // data = Memory[address,4]

      uint32_t data = registers_read(p->reg, rn, mode);

      if (rd == 15)
      {
        // PC = data AND 0xFFFFFFFE
        // T Bit = data[0]
        registers_write(p->reg, 15, mode, data & 0xFFFFFFFE);
        registers_write_T(p->reg, get_bit(data, 0));
      }
      else
      {
        // Rd = data
        // if (rn == 15)
        // {
        //   data += 8;
        //   //  dans le cas ou rn = 15, l'addresse de ma varible  je la connais pas  je sais pas ou est ce
        //   // le compilateur va la mettre
        // }

        // registers_write(p->reg, 15, mode, data & 0xFFFFFFFC);
        registers_write(p->reg, rd, mode, data);
        // on affiche la valeur a l'addrss de rd
        // int memory_read_word(memory mem, uint32_t address, uint32_t *value, uint8_t be)

        uint32_t val = 0;
        memory_read_word(p->mem, data, &val, endianess);
        // printf ("val = %x\n", val);
      }
    }
    break;
  case 0:
    // STRH
    if (bitI == 0 && get_bits(ins, 27, 26) == 0b00)
    {
      // Memory[address,2] = Rd[15:0]
      address = arm_read_register(p, rn);
      uint16_t data = arm_read_register(p, rd) & 0xFFFF;
      arm_write_half(p, address, data & 0xFFFF);
    }

    // STRB
    else if (bitB == 1 && get_bits(ins, 27, 26) == 0b01)
    {
      // Memory[address,1] = Rd[7:0]
      address = arm_read_register(p, rn);
      uint8_t data = arm_read_register(p, rd) & 0xFF;
      arm_write_byte(p, address, data & 0xFF);
    }

    // STR
    else if (bitB == 0 && get_bits(ins, 27, 26) == 0b01)
    {
      // Memory[address,4] = Rd
      address = arm_read_register(p, rn);
      uint32_t data = arm_read_register(p, rd);
      arm_write_word(p, address, data);
    }
    break;
  default:
    fprintf(stderr, "Erreur ! Le bit L n'est pas à 0 ou 1.\n");
    return UNDEFINED_INSTRUCTION;
  }
  return UNDEFINED_INSTRUCTION;
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

// Manque les cas avec le bit S. Manuel page 482.
// LDM et STM info plus générale page 134.
// LDM(1) (apparement c'est lui qu'il faut faire) page 186 du manuel.
// STM(1) page 339 du Manuel.
int arm_load_store_multiple(arm_core p, uint32_t ins)
{
  uint8_t posP = 24;
  uint8_t posU = 23;
  uint8_t posS = 22;
  uint8_t posW = 21;
  uint8_t posL = 20;

  uint8_t P = (ins >> posP) & 0b1;
  uint8_t U = (ins >> posU) & 0b1;
  uint8_t S = (ins >> posS) & 0b1;
  uint8_t W = (ins >> posW) & 0b1;
  uint8_t L = (ins >> posL) & 0b1;

  uint8_t posRn = 16; // 19 à 16
  uint8_t rn = (ins >> posRn) & 0b1111;
  uint16_t register_list = ins & 0b1111; // 15 à 0

  uint32_t address = arm_read_register(p, rn);

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
  if (L)
  { // LDM(1)
    for (int i = 0; i < 15; i++)
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
        uint32_t Ri = arm_read_register(p, address);
        arm_write_register(p, i, Ri);
        if (P == 0)
        {
          if (U == 0) // decremente AFTER
            address -= 4;
          else // incremente AFTER
            address += 4;
        }
      }
    }
    if ((register_list & (1 << 15)) != 0)
    { // si PC est set dans register_list
      uint32_t value = arm_read_register(p, address);
      arm_write_register(p, 15, value); // 15 pour pc
      // uint8_t T = value & 0b1;//bit thumb? qu'est ce que c'est que ce truc??
      address += 4;
      // fprintf(stderr, "<arm_load_store.c> Non implémenté : le bit 15 (PC) est set.\n");
      // return UNDEFINED_INSTRUCTION;
      //  des choses incompréhensibles
      // TODO à vérifier ici voilà
    }
  }
  else
  { // STM(1)
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
        uint32_t Ri = arm_read_register(p, i);
        arm_write_register(p, address, Ri);
        if (P == 0)
        {
          if (U == 0) // decremente AFTER
            address -= 4;
          else // incremente AFTER
            address += 4;
        }
      }
    }
  }
  if (W == 1)
  {
    if (U == 0)
      address = address - 4 * nbr_register_list;
    else
      address = address + 4 * nbr_register_list;
  }
  return 0;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins)
{
  /* Not implemented */
  return UNDEFINED_INSTRUCTION;
}
