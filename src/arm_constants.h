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
#ifndef __ARM_CONSTANTS_H__
#define __ARM_CONSTANTS_H__
#include <stdint.h>

/* ARM Modes */
#define USR 0x10
#define FIQ 0x11
#define IRQ 0x12
#define SVC 0x13
#define ABT 0x17
#define UND 0x1b
#define SYS 0x1f

/* ARM Exceptions (by priority) */
#define RESET 1
#define DATA_ABORT 2
#define FAST_INTERRUPT 3
#define INTERRUPT 4
#define IMPRECISE_ABORT 5 // Unsupported, ARMV6
#define PREFETCH_ABORT 6
#define UNDEFINED_INSTRUCTION 7
#define SOFTWARE_INTERRUPT 8
/* The last one is not realy an exception, but, as we handle software interrupts
 * within the simulator and we decide there to end the simulation, this is a way
 * to tell it to the outside world
 */
#define END_SIMULATION 9

/* Some CPSR bits */
#define N 31
#define Z 30
#define C 29
#define V 28
#define T 5

/* shift operations */
#define LSL 0
#define LSR 1
#define ASR 2
#define ROR 3

/* Conditions */
#define EQ 0b0000
#define NE 0b0001
#define CSHS 0b0010
#define CCLO 0b0011
#define MI 0b0100
#define PL 0b0101
#define VS 0b0110
#define VC 0b0111
#define HI 0b1000
#define LS 0b1001
#define GE 0b1010
#define LT 0b1011
#define GT 0b1100
#define LE 0b1101
#define AL 0b1110

/* Opcodes */
#define AND 0b0000
#define EOR 0b0001
#define SUB 0b0010
#define RSB 0b0011
#define ADD 0b0100
#define ADC 0b0101
#define SBC 0b0110
#define RSC 0b0111
#define TST 0b1000
#define TEQ 0b1001
#define CMP 0b1010
#define CMN 0b1011
#define ORR 0b1100
#define MOV 0b1101
#define BIC 0b1110
#define MVN 0b1111

/* Bit mask constants for msr */
/* We simulate architecture v5T */
#define UnallocMask 0x0FFFFF00
#define UserMask 0xF0000000
#define PrivMask 0x0000000F
#define StateMask 0x00000020

char *arm_get_exception_name(unsigned char exception);
char *arm_get_mode_name(uint8_t mode);
int8_t arm_get_mode_number(char *name);
char *arm_get_register_name(uint8_t reg);

#endif
