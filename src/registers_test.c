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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "registers.h"
#include "arm_constants.h"
#include "util.h"

void print_test(int result)
{
  if (result)
    printf("Test succeded\n");
  else
    printf("TEST FAILED !!\n");
}

void test_registers_create()
{
  printf("Test : Create registers ... ");
  registers r = registers_create();
  assert(r != NULL);
  printf("OK\n");
  registers_destroy(r);
}

void test_register_read_write_cpsr()
{
  registers r = registers_create();
  assert(r != NULL);

  printf("Test : Write/Read cpsr ... ");
  registers_write_cpsr(r, 0x1d3);
  assert(registers_read_cpsr(r) == 0x1d3);
  printf("OK\n");

  registers_destroy(r);
}

void test_registers_get_mode()
{
  registers r = registers_create();
  assert(r != NULL);

  printf("Test : Get mode (USR) ... ");
  // 0x110 = 0000 0001 0001 0000 = Mode USR
  registers_write_cpsr(r, 0x110);
  assert(registers_get_mode(r) == USR);
  printf("OK\n");

  printf("Test : Get mode (FIQ) ... ");
  // 0x11 = 0000 0000 0001 0001 = Mode FIQ
  registers_write_cpsr(r, 0x11);
  assert(registers_get_mode(r) == FIQ);
  printf("OK\n");

  printf("Test : Get mode (IRQ) ... ");
  // 0x12 = 0000 0000 0001 0010 = Mode IRQ
  registers_write_cpsr(r, 0x12);
  assert(registers_get_mode(r) == IRQ);
  printf("OK\n");

  printf("Test : Get mode (SVC) ... ");
  // 0x13 = 0000 0000 0001 0011 = Mode SVC
  registers_write_cpsr(r, 0x13);
  assert(registers_get_mode(r) == SVC);
  printf("OK\n");

  printf("Test : Get mode (ABT) ... ");
  // 0x17 = 0000 0000 0001 0111 = Mode ABT
  registers_write_cpsr(r, 0x17);
  assert(registers_get_mode(r) == ABT);
  printf("OK\n");

  printf("Test : Get mode (UND) ... ");
  // 0x1b = 0000 0000 0001 1011 = Mode UND
  registers_write_cpsr(r, 0x1b);
  assert(registers_get_mode(r) == UND);
  printf("OK\n");

  printf("Test : Get mode (SYS) ... ");
  // 0x1f = 0000 0000 0001 1111 = Mode SYS
  registers_write_cpsr(r, 0x1f);
  assert(registers_get_mode(r) == SYS);
  printf("OK\n");

  registers_destroy(r);
}

void test_registers_current_mode_has_spsr()
{

  registers r = registers_create();
  assert(r != NULL);

  printf("Test : Current mode has spsr (USR) ... ");
  registers_write_cpsr(r, 0x110);
  assert(!registers_current_mode_has_spsr(r));
  printf("OK\n");

  printf("Test : Current mode has spsr (FIQ) ... ");
  registers_write_cpsr(r, 0x11);
  assert(registers_current_mode_has_spsr(r));
  printf("OK\n");

  printf("Test : Current mode has spsr (IRQ) ... ");
  registers_write_cpsr(r, 0x12);
  assert(registers_current_mode_has_spsr(r));
  printf("OK\n");

  printf("Test : Current mode has spsr (SVC) ... ");
  registers_write_cpsr(r, 0x13);
  assert(registers_current_mode_has_spsr(r));
  printf("OK\n");

  printf("Test : Current mode has spsr (ABT) ... ");
  registers_write_cpsr(r, 0x17);
  assert(registers_current_mode_has_spsr(r));
  printf("OK\n");

  printf("Test : Current mode has spsr (UND) ... ");
  registers_write_cpsr(r, 0x1b);
  assert(registers_current_mode_has_spsr(r));
  printf("OK\n");

  printf("Test : Current mode has spsr (SYS) ... ");
  registers_write_cpsr(r, 0x1f);
  assert(!registers_current_mode_has_spsr(r));
  printf("OK\n");

  registers_destroy(r);
}

int main()
{

  test_registers_create();
  test_register_read_write_cpsr();
  test_registers_get_mode();
  test_registers_current_mode_has_spsr();

  return 0;

  registers r;
  uint32_t word_value[15], word_read;
  int i;

  r = registers_create();
  if (r == NULL)
  {
    fprintf(stderr, "Error when creating simulated registers\n");
    exit(1);
  }
  srandom(getpid());
  for (i = 0; i < 15; i++)
  {
    word_value[i] = random();
  }

  // Arm reset
  printf("Reseting registers...\n");
  registers_write_cpsr(r, 0x1d3);
  uint8_t mode = registers_get_mode(r);
  registers_write(r, 15, mode, 0);

  printf("Testing read and write to general registers:\n");
  for (i = 0; i < 15; i++)
  {
    registers_write(r, i, mode, word_value[i]);
  }
  for (i = 0; i < 15; i++)
  {
    word_read = registers_read(r, i, mode);
    printf("- register %d : ", i);
    print_test(word_read == word_value[i]);
  }

  printf("Current mode : ");
  print_test(registers_get_mode(r) == SVC);
  printf("Mode is priviledged : ");
  print_test(registers_in_a_privileged_mode(r));
  printf("Mode has spsr : ");
  print_test(registers_current_mode_has_spsr(r));

  registers_destroy(r);

  return 0;
}
