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
#ifndef __REGISTERS_H__
#define __REGISTERS_H__
#include <stdint.h>
#include <stdio.h>

struct registers_data
{
  /*
    Voir figure A2-1 du manuel (page 43)

    Video: https://youtu.be/msWvVmCZRTI?si=YUxaf2tEL0ttMTFD
  */

  // r0-r7 sont les mêmes pour tous les modes (unbanked registers)
  // pc est le même pour tous les modes (r15)
  uint32_t registers[16];

  // Modes spécifiques de SVC, ABT, UND, IRQ, FIQ
  uint32_t r13_svc, r14_svc, spsr_svc;
  uint32_t r13_abt, r14_abt, spsr_abt;
  uint32_t r13_und, r14_und, spsr_und;
  uint32_t r13_irq, r14_irq, spsr_irq;
  uint32_t r8_fiq, r9_fiq, r10_fiq, r11_fiq, r12_fiq, r13_fiq, r14_fiq, spsr_fiq;

  // cpsr est le même pour tous les modes
  uint32_t cpsr;
};
typedef struct registers_data *registers;

registers registers_create();
void registers_destroy(registers r);

uint8_t registers_get_mode(registers r);
int registers_current_mode_has_spsr(registers r);
int registers_in_a_privileged_mode(registers r);

uint32_t registers_read(registers r, uint8_t reg, uint8_t mode);
uint32_t registers_read_cpsr(registers r);
uint32_t registers_read_spsr(registers r, uint8_t mode);
void registers_write(registers r, uint8_t reg, uint8_t mode, uint32_t value);
void registers_write_cpsr(registers r, uint32_t value);
void registers_write_spsr(registers r, uint8_t mode, uint32_t value);

void registers_write_Z(registers r, uint8_t value);
void registers_write_N(registers r, uint8_t value);
void registers_write_C(registers r, uint8_t value);
void registers_write_V(registers r, uint8_t value);
int registers_read_Z(registers r);
int registers_read_N(registers r);
int registers_read_C(registers r);
int registers_read_V(registers r);

#endif
