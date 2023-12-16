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
  /*
    Registers :
      r0-r10 (General Purpose)
      r11    (FP - Frame Pointer)
      r12    (IP - Intra Procedure Call)
      r13    (SP - Stack Pointer)
      r14    (LR - Link Register)
      r15    (PC - Program Counter)
      cpsr   (CPSR - Current Program Status Register)
      spsr   (SPSR - Saved Program Status Register)
    Tutorial :
      https://www.youtube.com/watch?v=ibh4tadjUaw&ab_channel=techAj
  */
  uint32_t r[16];
  uint32_t cpsr;
  uint32_t spsr;
};

registers registers_create()
{
  registers registers = malloc(sizeof(struct registers_data));

  if (registers == NULL)
  {
    fprintf(stderr, "Erreur lors de l'allocation des registres");
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
    fprintf(stderr, "Erreur lors de la lecture du mode");
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
    Documentation:
      https://developer.arm.com/documentation/dvi0027/b/arm-architecture-v4t/modes-and-exceptions

      All modes other than User are privileged modes. These are used to service hardware interrupts, exceptions, and software interrupts. Each privileged mode has an associated Saved Program Status Register (SPSR). This register is use to save the state of the Current Program Status Register (CPSR) of the task immediately before the exception occurs.
  */

  // On regarde si le mode est différent de USR
  return mode != USR;
}

int registers_current_mode_has_spsr(registers r)
{
  return registers_mode_has_spsr(r, registers_get_mode(r));
}

int registers_in_a_privileged_mode(registers r)
{
  /*
    Documentation:
      https://developer.arm.com/documentation/dvi0027/b/arm-architecture-v4t/modes-and-exceptions

      All modes other than User are privileged modes. These are used to service hardware interrupts, exceptions, and software interrupts. Each privileged mode has an associated Saved Program Status Register (SPSR). This register is use to save the state of the Current Program Status Register (CPSR) of the task immediately before the exception occurs.
  */

  // On regarde si le mode est différent de USR
  return registers_get_mode(r) != USR;
}

uint32_t registers_read(registers r, uint8_t reg, uint8_t mode)
{
  // TODO: Corriger cette fonction
  return r->r[reg];
}

uint32_t registers_read_cpsr(registers r)
{
  return r->cpsr;
}

uint32_t registers_read_spsr(registers r, uint8_t mode)
{
  // On regarde si le mode est différent de USR
  if (registers_mode_has_spsr(r, mode))
  {
    return r->spsr;
  }
  else
  {
    fprintf(stderr, "Erreur lors de la lecture du registre spsr");
    exit(EXIT_FAILURE);
  }
}

void registers_write(registers r, uint8_t reg, uint8_t mode, uint32_t value)
{
  // TODO: Corriger cette fonction
  r->r[reg] = value;
}

void registers_write_cpsr(registers r, uint32_t value)
{
  r->cpsr = value;
}

void registers_write_spsr(registers r, uint8_t mode, uint32_t value)
{
  // On regarde si le mode est différent de USR
  if (registers_mode_has_spsr(r, mode))
  {
    r->spsr = value;
  }
  else
  {
    fprintf(stderr, "Erreur lors de l'écriture du registre spsr");
    exit(EXIT_FAILURE);
  }
}
