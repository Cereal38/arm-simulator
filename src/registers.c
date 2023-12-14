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
#include "registers.h"
#include "arm_constants.h"
#include <stdlib.h>

struct registers_data
{
  uint32_t r[16];
  uint32_t cpsr; /* Current Program Status Register */
  uint32_t spsr; /* Saved Program Status Register */
};

registers registers_create()
{
  registers registers = malloc(sizeof(struct registers_data));

  if (registers == NULL)
  {
    ERROR_MSG("Erreur lors de l'allocation des registres");
    exit(EXIT_FAILURE);
  }

  return registers;
}

void registers_destroy(registers r)
{
  if (r != NULL)
  {
    free(r);
  }
}

uint8_t registers_get_mode(registers r)
{
  /*
    Modes :
      USR 0x10 (User)
      FIQ 0x11 (Fast Interrupt Request)
      IRQ 0x12 (Interrupt Request)
      SVC 0x13 (Supervisor)
      ABT 0x17 (Abort)
      UND 0x1b (Undefined)
      SYS 0x1f (System)
    Documentation :
      https://developer.arm.com/documentation/ddi0406/cb/System-Level-Architecture/The-System-Level-Programmers--Model/ARM-processor-modes-and-ARM-core-registers/ARM-processor-modes?lang=en
  */
  if (r == NULL)
  {
    ERROR_MSG("Erreur lors de la lecture du mode");
    exit(EXIT_FAILURE);
  }
  // On regarde les 5 derniers bits de poids faible du registre cpsr
  // Pour récupérer le mode
  // 0x1f = 0001 1111
  return r->cpsr & 0x1f;
}

static int registers_mode_has_spsr(registers r, uint8_t mode)
{
  /*
  Dans l'architecture ARM, le registre SPSR (Saved Program Status Register) est un registre spécial qui sauvegarde l'état du registre CPSR (Current Program Status Register) lorsqu'une exception est déclenchée. Cela permet au processeur de restaurer l'état précédent du CPSR lorsque l'exception est terminée.

  Cependant, le registre SPSR n'est pas toujours présent. Il n'existe que dans certains modes d'exécution du processeur. Plus précisément, il est présent dans les modes suivants : FIQ (Fast Interrupt Request), IRQ (Interrupt Request), SVC (Supervisor), ABT (Abort) et UND (Undefined).

  Dire que le "registre SPSR est présent" signifie que le processeur est actuellement dans l'un de ces modes d'exécution où le registre SPSR existe et peut être utilisé.
  */

  // Les modes qui ont un registre SPSR sont FIQ, IRQ, SVC, ABT et UND
  uint8_t modes_with_spsr[] = {FIQ, IRQ, SVC, ABT, UND};

  // On retourne 1 si le mode est dans le tableau
  for (int i = 0; i < sizeof(modes_with_spsr); i++)
  {
    if (mode == modes_with_spsr[i])
    {
      return 1;
    }
  }

  return 0;
}

int registers_current_mode_has_spsr(registers r)
{
  return registers_mode_has_spsr(r, registers_get_mode(r));
}

int registers_in_a_privileged_mode(registers r)
{
  /* � compl�ter... */
  return 0;
}

uint32_t registers_read(registers r, uint8_t reg, uint8_t mode)
{
  uint32_t value = 0;
  /* � compl�ter... */
  return value;
}

uint32_t registers_read_cpsr(registers r)
{
  uint32_t value = 0;
  /* � compl�ter... */
  return value;
}

uint32_t registers_read_spsr(registers r, uint8_t mode)
{
  uint32_t value = 0;
  /* � compl�ter... */
  return value;
}

void registers_write(registers r, uint8_t reg, uint8_t mode, uint32_t value)
{
  /* � compl�ter... */
}

void registers_write_cpsr(registers r, uint32_t value)
{
  /* � compl�ter... */
}

void registers_write_spsr(registers r, uint8_t mode, uint32_t value)
{
  /* � compl�ter... */
}
