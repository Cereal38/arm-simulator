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
#include "arm_instruction.h"
#include "arm_exception.h"
#include "arm_data_processing.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"

static int arm_execute_instruction(arm_core p)
{
  uint32_t instruction;
  int resultat = arm_fetch(p, &instruction);

  if (resultat == -1)
  {
    // return arm_exception(p, resultat); // venant de arm step.. pas sur
    // // Doit retrun 1 si il y a une erreur (d'apres fun arm step)
    return 1;
  }
  //  << 24 or >> 21 c'est sur 8 ou 4 bits?
  uint8_t codeInstruction = (uint8_t)((instruction >> 21) & 0b1111);

  switch (codeInstruction)
  {
  case 0b0000: // AND
    printf("Implement AND\n");
    break;
  case 0b0001:
    printf("Implement EOR\n");
    break;
  case 0b0010:
    printf("Implement SUB\n");
    break;
  case 0b0011:
    printf("Implement RSB\n");
    break;
  case 0b0100:
    arm_data_processing_add(p, instruction);
    break;
  case 0b0101:
    printf("Implement ADC\n");
    break;
  case 0b0110:
    printf("Implement SBC\n");
    break;
  case 0b0111:
    printf("Implement RSC\n");
    break;
  case 0b1000:
    printf("Implement TST\n");
    break;
  case 0b1001:
    printf("Implement TEQ\n");
    break;
  case 0b1010:
    printf("Implement CMP\n");
    break;
  case 0b1011:
    printf("Implement CMN\n");
    break;
  case 0b1100:
    printf("Implement ORR\n");
    break;
  case 0b1101:
    printf("Implement MOV\n");
    break;
  case 0b1110:
    printf("Implement BIC\n");
    break;
  case 0b1111:
    printf("Implement MVN\n");
    break;
  default: // ne dois jamais arriver
    fprintf(stderr, "<arm_execute_instruction> Erreur default dans switch\n");
    exit(1);
  }

  return 0;
}

int arm_step(arm_core p)
{
  int result;

  result = arm_execute_instruction(p);
  if (result)
  {
    return arm_exception(p, result);
  }
  return result;
}
