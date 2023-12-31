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
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

/* We implement asr because shifting a signed is non portable in ANSI C */
uint32_t asr(uint32_t value, uint8_t shift)
{
  return (value >> shift) | (get_bit(value, 31) ? ~0 << (32 - shift) : 0);
}

uint32_t ror(uint32_t value, uint8_t rotation)
{
  return (value >> rotation) | (value << (32 - rotation));
}

int is_big_endian()
{
  static uint32_t one = 1;
  return ((*(uint8_t *)&one) == 0);
}

void log_printf(const char *format, ...)
{
  /*
    Print the given formatted string to the file logs.txt
  */
  FILE *f = fopen("logs.txt", "a");

  va_list args;
  va_start(args, format);
  vfprintf(f, format, args);
  va_end(args);

  fclose(f);
}

char *to_binary(uint32_t value, int size)
{
  /*
    Convert the given value to a string representing a tab containing 2 rows
    The first row is the number of the bit
    The second row is the value of the bit
    Example:
    31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|...
     0| 0| 1| 0| 0| 0| 0| 0| 0| 0| 0| 1| 1| 0| 0| 0|...
  */
  char *result = malloc(size * 3 * 2 + 4);

  // Write first row
  for (int i = size - 1; i >= 0; i--)
  {
    sprintf(result + (size - 1 - i) * 3, "%2d|", i);
  }

  sprintf(result + size * 3, "\n");

  // Write second row
  for (int i = size - 1; i >= 0; i--)
  {
    sprintf(result + (size * 3 + 1) + (size - 1 - i) * 3, "%2d|", get_bit(value, i));
  }

  sprintf(result + size * 3 * 2 + 2, "\n");

  return result;
}

void error_if_null(void *ptr)
{
  /*
    If the given pointer is null, print an error message and exit the program
  */
  if (ptr == NULL)
  {
    fprintf(stderr, "Erreur: pointeur null\n");
    exit(EXIT_FAILURE);
  }
}