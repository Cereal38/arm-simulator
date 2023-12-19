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
  */
  if (r == NULL)
  {
    fprintf(stderr, "Erreur lors de la lecture du mode");
    exit(EXIT_FAILURE);
  }
  // On regarde les 5 derniers bits de poids faible du registre cpsr (Manuel A2.5)
  // Pour récupérer le mode
  // 0x1f = 0001 1111
  return r->cpsr & 0x1f;
}

static int registers_mode_has_spsr(registers r, uint8_t mode)
{
  if (r == NULL)
  {
    fprintf(stderr, "<registers_mode_has_spsr> Erreur: r est nulle\n");
    exit(EXIT_FAILURE);
  }
  // On regarde si le mode appartient à la liste des modes qui ont un spsr
  return mode == FIQ || mode == IRQ || mode == SVC || mode == ABT || mode == UND;
}

int registers_current_mode_has_spsr(registers r)
{
  if (r == NULL)
  {
    fprintf(stderr, "<registers_current_mode_has_spsr> Erreur: r est nulle\n");
    exit(EXIT_FAILURE);
  }
  return registers_mode_has_spsr(r, registers_get_mode(r));
}

int registers_in_a_privileged_mode(registers r)
{
  if (r == NULL)
  {
    fprintf(stderr, "<registers_in_a_privileged_mode> Erreur: r est nulle\n");
    exit(EXIT_FAILURE);
  }
  // On regarde si le mode appartient à la liste des modes privilégiés
  uint8_t mode = registers_get_mode(r);
  return mode == FIQ || mode == IRQ || mode == SVC || mode == ABT || mode == UND || mode == SYS;
}

uint32_t registers_read(registers r, uint8_t reg, uint8_t mode)
{
  if (r == NULL)
  {
    fprintf(stderr, "<registers_read> Erreur: r est nulle\n");
    exit(EXIT_FAILURE);
  }
  // On regarde si le registre est un registre unbanked ou le pc (r15)
  if (reg < 8 || reg == 15)
  {
    return r->registers[reg];
  }

  // On regarde si le registre est un registre spécifique à un mode
  switch (mode)
  {
  case FIQ:
    switch (reg)
    {
    case 8:
      return r->r8_fiq;
    case 9:
      return r->r9_fiq;
    case 10:
      return r->r10_fiq;
    case 11:
      return r->r11_fiq;
    case 12:
      return r->r12_fiq;
    case 13:
      return r->r13_fiq;
    case 14:
      return r->r14_fiq;
    }
    break;
  case IRQ:
    switch (reg)
    {
    case 13:
      return r->r13_irq;
    case 14:
      return r->r14_irq;
    }
    break;
  case SVC:
    switch (reg)
    {
    case 13:
      return r->r13_svc;
    case 14:
      return r->r14_svc;
    }
    break;
  case ABT:
    switch (reg)
    {
    case 13:
      return r->r13_abt;
    case 14:
      return r->r14_abt;
    }
    break;
  case UND:
    switch (reg)
    {
    case 13:
      return r->r13_und;
    case 14:
      return r->r14_und;
    }
    break;
  }

  // Le registre n'est pas un registre spécifique à un mode
  return r->registers[reg];
}

uint32_t registers_read_cpsr(registers r)
{
  if (r == NULL)
  {
    fprintf(stderr, "<registers_read_cpsr> Erreur: r est nulle\n");
    exit(EXIT_FAILURE);
  }
  return r->cpsr;
}

uint32_t registers_read_spsr(registers r, uint8_t mode)
{
  if (r == NULL)
  {
    fprintf(stderr, "<registers_read_spsr> Erreur: r est nulle\n");
    exit(EXIT_FAILURE);
  }
  // On regarde si le mode appartient à la liste des modes qui ont un spsr
  if (registers_mode_has_spsr(r, mode))
  {
    switch (mode)
    {
    case FIQ:
      return r->spsr_fiq;
    case IRQ:
      return r->spsr_irq;
    case SVC:
      return r->spsr_svc;
    case ABT:
      return r->spsr_abt;
    case UND:
      return r->spsr_und;
    }
  }

  fprintf(stderr, "Erreur ! Le registre spsr n'existe pas pour le mode %s", arm_get_mode_name(mode));
  exit(EXIT_FAILURE);
}

void registers_write(registers r, uint8_t reg, uint8_t mode, uint32_t value)
{
  if (r == NULL)
  {
    fprintf(stderr, "<registers_write> Erreur: r est nulle\n");
    exit(EXIT_FAILURE);
  }
  // On regarde si le registre est un registre unbanked ou le pc (r15)
  if (reg < 8 || reg == 15)
  {
    r->registers[reg] = value;
    return;
  }

  // On regarde si le registre est un registre spécifique à un mode
  switch (mode)
  {
  case FIQ:
    switch (reg)
    {
    case 8:
      r->r8_fiq = value;
      return;
    case 9:
      r->r9_fiq = value;
      return;
    case 10:
      r->r10_fiq = value;
      return;
    case 11:
      r->r11_fiq = value;
      return;
    case 12:
      r->r12_fiq = value;
      return;
    case 13:
      r->r13_fiq = value;
      return;
    case 14:
      r->r14_fiq = value;
      return;
    }
    break;
  case IRQ:
    switch (reg)
    {
    case 13:
      r->r13_irq = value;
      return;
    case 14:
      r->r14_irq = value;
      return;
    }
    break;
  case SVC:
    switch (reg)
    {
    case 13:
      r->r13_svc = value;
      return;
    case 14:
      r->r14_svc = value;
      return;
    }
    break;
  case ABT:
    switch (reg)
    {
    case 13:
      r->r13_abt = value;
      return;
    case 14:
      r->r14_abt = value;
      return;
    }
    break;
  case UND:
    switch (reg)
    {
    case 13:
      r->r13_und = value;
      return;
    case 14:
      r->r14_und = value;
      return;
    }
    break;
  }

  // Le registre n'est pas un registre spécifique à un mode
  r->registers[reg] = value;
}

void registers_write_cpsr(registers r, uint32_t value)
{
  if (r == NULL)
  {
    fprintf(stderr, "<registers_write_cpsr> Erreur: r est nulle\n");
    exit(EXIT_FAILURE);
  }
  r->cpsr = value;
}

void registers_write_spsr(registers r, uint8_t mode, uint32_t value)
{
  if (r == NULL)
  {
    fprintf(stderr, "<registers_write_spsr> Erreur: r est nulle\n");
    exit(EXIT_FAILURE);
  }
  // On regarde si le mode appartient à la liste des modes qui ont un spsr
  if (registers_mode_has_spsr(r, mode))
  {
    switch (mode)
    {
    case FIQ:
      r->spsr_fiq = value;
      return;
    case IRQ:
      r->spsr_irq = value;
      return;
    case SVC:
      r->spsr_svc = value;
      return;
    case ABT:
      r->spsr_abt = value;
      return;
    case UND:
      r->spsr_und = value;
      return;
    }
  }

  fprintf(stderr, "Erreur ! Le registre spsr n'existe pas pour le mode %s", arm_get_mode_name(mode));
  exit(EXIT_FAILURE);
}
