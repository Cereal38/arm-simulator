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
#include <stdlib.h>
#include "memory.h"
#include "util.h"
#include <stdio.h>

struct memory_data
{
  size_t size;
  uint8_t *data;
};

memory memory_create(size_t size)
{

  memory mem = malloc(sizeof(struct memory_data));
  error_if_null(mem);
  mem->size = size;
  mem->data = malloc(size);
  error_if_null(mem->data);

  return mem;
}

size_t memory_get_size(memory mem)
{
  error_if_null(mem);
  return mem->size;
}

void memory_destroy(memory mem)
{
  error_if_null(mem);
  if (mem->data != NULL)
  {
    free(mem->data);
  }
  free(mem);
}

int memory_read_byte(memory mem, uint32_t address, uint8_t *value)
{
  error_if_null(mem);
  if (address > mem->size)
  {
    printf("address = %hx , size= %lx\n", address, mem->size);
    fprintf(stderr, "Erreur lors de l'allocation de la memoire");
    exit(1);
  }
  *value = mem->data[address];

  return 0;
}

int memory_read_half(memory mem, uint32_t address, uint16_t *value, uint8_t be)
{

  uint8_t byte1, byte2;
  error_if_null(mem);
  if (address > mem->size)
  {
    fprintf(stderr, "Erreur lors de l'allocation de la memoire");
    exit(1);
  }

  if (memory_read_byte(mem, address, &byte1) != 0)
  {
    return -1;
  }

  if (memory_read_byte(mem, address + 1, &byte2) != 0)
  {
    return -1;
  }

  if (be)
  {
    // BIG ENDIAN
    *value = (uint16_t)(byte1 << 8) | byte2;
  }
  else
  {
    // LITTLE ENDIAN
    *value = (uint16_t)(byte2 << 8) | byte1;
  }

  return 0;
}

int memory_read_word(memory mem, uint32_t address, uint32_t *value, uint8_t be)
{
  uint8_t byte1, byte2, byte3, byte4;

  error_if_null(mem);
  if (address > mem->size)
  {
    fprintf(stderr, "Erreur lors de l'allocation de la memoire");
    exit(1);
  }

  if (memory_read_byte(mem, address, &byte1) != 0)
  {
    return -1;
  }

  if (memory_read_byte(mem, address + 1, &byte2) != 0)
  {
    return -1;
  }

  if (memory_read_byte(mem, address + 2, &byte3) != 0)
  {
    return -1;
  }

  if (memory_read_byte(mem, address + 3, &byte4) != 0)
  {
    return -1;
  }

  if (be)
  {
    *value = (uint32_t)(byte1 << 24) | (uint32_t)(byte2 << 16) | (uint32_t)(byte3 << 8) | byte4;
  }
  else
  {
    *value = (uint32_t)(byte4 << 24) | (uint32_t)(byte3 << 16) | (uint32_t)(byte2 << 8) | byte1;
  }

  return 0;
}

int memory_write_byte(memory mem, uint32_t address, uint8_t value)
{

  if (mem == NULL || address >= mem->size)
    return -1;
  mem->data[address] = value;
  return 0;
}

int memory_write_half(memory mem, uint32_t address, uint16_t value, uint8_t be)
{
  if (mem == NULL || address + 1 >= mem->size)
    return -1;

  if (be)
  {
    mem->data[address] = ((uint8_t *)&value)[1];     //    (uint8_t)(value >> 8)
    mem->data[address + 1] = ((uint8_t *)&value)[0]; //  (uint8_t)(value << 8)
  }
  else
  {
    mem->data[address] = ((uint8_t *)&value)[0];     //   (uint8_t)(value << 8)
    mem->data[address + 1] = ((uint8_t *)&value)[1]; //  (uint8_t)(value >> 8)
  }
  return 0;
}

int memory_write_word(memory mem, uint32_t address, uint32_t value, uint8_t be)
{
  if (mem == NULL || address + 3 >= mem->size)
    return -1;

  if (be)
  {
    mem->data[address] = ((uint8_t *)&value)[3];
    mem->data[address + 1] = ((uint8_t *)&value)[2];
    mem->data[address + 2] = ((uint8_t *)&value)[1];
    mem->data[address + 3] = ((uint8_t *)&value)[0];
  }
  else
  {
    mem->data[address] = ((uint8_t *)&value)[0];
    mem->data[address + 1] = ((uint8_t *)&value)[1];
    mem->data[address + 2] = ((uint8_t *)&value)[2];
    mem->data[address + 3] = ((uint8_t *)&value)[3];
  }

  return 0;
}
