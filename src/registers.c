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
#include "util.h"
#include <stdlib.h>

registers registers_create()
{
  registers registers = malloc(sizeof(struct registers_data));
  error_if_null(registers);
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
  error_if_null(r);
  // On regarde les 5 derniers bits de poids faible du registre cpsr (Manuel A2.5)
  // Pour récupérer le mode
  // 0x1f = 0001 1111
  return get_bits(r->cpsr, 4, 0);
}

static int registers_mode_has_spsr(registers r, uint8_t mode)
{
  error_if_null(r);
  // On regarde si le mode appartient à la liste des modes qui ont un spsr
  return mode == FIQ || mode == IRQ || mode == SVC || mode == ABT || mode == UND;
}

int registers_current_mode_has_spsr(registers r)
{
  error_if_null(r);
  return registers_mode_has_spsr(r, registers_get_mode(r));
}

int registers_in_a_privileged_mode(registers r)
{
  error_if_null(r);
  // On regarde si le mode appartient à la liste des modes privilégiés
  uint8_t mode = registers_get_mode(r);
  return mode == FIQ || mode == IRQ || mode == SVC || mode == ABT || mode == UND || mode == SYS;
}

uint32_t registers_read(registers r, uint8_t reg, uint8_t mode)
{
  error_if_null(r);
  // On regarde si le registre est un registre unbanked ou le pc (r15)
  if (reg < 8 || reg == 15)
  {
    return r->registers[reg];
  }

  // On regarde si le registre est un registre spécifique à un mode
  switch (mode)
  {
  case FIQ:
    if (reg >= 8 && reg <= 14)
      return r->registers_fiq[reg - 8];
    break;
  case IRQ:
    if (reg == 13 || reg == 14)
      return r->registers_irq[reg - 13];
    break;
  case SVC:
    if (reg == 13 || reg == 14)
      return r->registers_svc[reg - 13];
    break;
  case ABT:
    if (reg == 13 || reg == 14)
      return r->registers_abt[reg - 13];
    break;
  case UND:
    if (reg == 13 || reg == 14)
      return r->registers_und[reg - 13];
    break;
  }

  // Le registre n'est pas un registre spécifique à un mode
  return r->registers[reg];
}

uint32_t registers_read_cpsr(registers r)
{
  error_if_null(r);
  return r->cpsr;
}

uint32_t registers_read_spsr(registers r, uint8_t mode)
{
  error_if_null(r);
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
  error_if_null(r);
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
    if (reg >= 8 && reg <= 14)
    {
      r->registers_fiq[reg - 8] = value;
      return;
    }
    break;
  case IRQ:
    if (reg == 13 || reg == 14)
    {
      r->registers_irq[reg - 13] = value;
      return;
    }
    break;
  case SVC:
    if (reg == 13 || reg == 14)
    {
      r->registers_svc[reg - 13] = value;
      return;
    }
    break;
  case ABT:
    if (reg == 13 || reg == 14)
    {
      r->registers_abt[reg - 13] = value;
      return;
    }
    break;
  case UND:
    if (reg == 13 || reg == 14)
    {
      r->registers_und[reg - 13] = value;
      return;
    }
    break;
  }

  // Le registre n'est pas un registre spécifique à un mode
  r->registers[reg] = value;
}

void registers_write_cpsr(registers r, uint32_t value)
{
  error_if_null(r);
  r->cpsr = value;
}

void registers_write_spsr(registers r, uint8_t mode, uint32_t value)
{
  error_if_null(r);
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

void write_cpsr_bit(registers r, uint8_t bit, uint8_t value)
{
  error_if_null(r);
  r->cpsr &= ~(1 << bit);
  r->cpsr |= value << bit;
}

void registers_write_Z(registers r, uint8_t value)
{
  write_cpsr_bit(r, Z, value);
}

void registers_write_N(registers r, uint8_t value)
{
  write_cpsr_bit(r, N, value);
}

void registers_write_C(registers r, uint8_t value)
{
  write_cpsr_bit(r, C, value);
}

void registers_write_V(registers r, uint8_t value)
{
  write_cpsr_bit(r, V, value);
}

void registers_write_T(registers r, uint8_t value)
{
  write_cpsr_bit(r, T, value);
}

int registers_read_Z(registers r)
{
  return get_bits(r->cpsr, Z, Z);
}

int registers_read_N(registers r)
{
  return get_bits(r->cpsr, N, N);
}

int registers_read_C(registers r)
{
  return get_bits(r->cpsr, C, C);
}

int registers_read_V(registers r)
{
  return get_bits(r->cpsr, V, V);
}

int registers_read_T(registers r)
{
  return get_bits(r->cpsr, T, T);
}

