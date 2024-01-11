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

// verif_cond prend en parametre l'instruction en cours
// et verifie si sa condition est verifiée
// renvoie 1 si ok 0 sinon
int verif_cond(uint32_t instruction, registers r)
{

  // Extraire les bits de condition (bits 28-31)
  uint8_t condition_bits = (uint8_t)get_bits(instruction, 31, 28);
  // récupération de l'état des flags
  uint32_t cpsr = registers_read_cpsr(r);

  switch (condition_bits)
  {
  case EQ: // Z == 1
    if ((cpsr & (1 << Z)) != 0)
    {
      return 1;
    }
    return 0;
  case NE: // Z == 0
    if ((cpsr & (1 << Z)) == 0)
    {
      return 1;
    }
    return 0;
  case CSHS: // CS/HS (C == 1)
    if ((cpsr & (1 << C)) != 0)
    {
      return 1;
    }
    return 0;
  case CCLO: // CC/LO (C == 0)
    if ((cpsr & (1 << C)) == 0)
    {
      return 1;
    }
    return 0;
  case MI: // N == 1
    if ((cpsr & (1 << N)) != 0)
    {
      return 1;
    }
    return 0;
  case PL: // N == 0
    if ((cpsr & (1 << N)) == 0)
    {
      return 1;
    }
    return 0;
  case VS: // V == 1
    if ((cpsr & (1 << V)) != 0)
    {
      return 1;
    }
    return 0;
  case VC: // V == 0
    if ((cpsr & (1 << V)) == 0)
    {
      return 1;
    }
    return 0;
  case HI: // C == 1 && Z == 0
    if (((cpsr & (1 << C)) != 0) && ((cpsr & (1 << Z)) == 0))
    {
      return 1;
    }
    return 0;
  case LS: // C == 0 || Z == 1
    if (((cpsr & (1 << C)) == 0) || ((cpsr & (1 << Z)) != 0))
    {
      return 1;
    }
    return 0;
  case GE: // N == V
    if ((cpsr & (1 << N)) == (cpsr & (1 << V)))
    {
      return 1;
    }
    return 0;
  case LT: // N != V
    if ((cpsr & (1 << N)) != (cpsr & (1 << V)))
    {
      return 1;
    }
    return 0;
  case GT: // Z == 0 && N == V
    if (((cpsr & (1 << Z)) == 0) && ((cpsr & (1 << N)) == (cpsr & (1 << V))))
    {
      return 1;
    }
    return 0;
  case LE: // Z == 1 || N != V
    if (((cpsr & (1 << Z)) != 0) || ((cpsr & (1 << N)) != (cpsr & (1 << V))))
    {
      return 1;
    }
    return 0;
  case AL: // Toujours vrai
    return 1;
  case 0xF: // - Voir condition code si ARMV5 unconditionnal execution
    return 1;
  default:
    fprintf(stderr, "Condition inconnue : %d\n", condition_bits);
    return -1;
  }
}



static int arm_execute_instruction(arm_core p)
{
  uint32_t instruction;
  int resultat = arm_fetch(p, &instruction);

  log_printf(">>> READ INSTRUCTION :\n%s\n", to_binary(instruction, 32));

  if (resultat)
  {
    // gestion interruption fetch
    return PREFETCH_ABORT;
  }

  // Verfification de la condition
  int cond = verif_cond(instruction, p->reg);

  if (cond == 0)
  {
    fprintf(stderr, "Condition non satisfaite \n");
    // Gestion interruption conditon non satisfaite
    //  TODO 
    return UNDEFINED_INSTRUCTION;
  }
  if (cond == -1)
  {
    fprintf(stderr, "Condition non existante \n");
    // Gestion interruption conditon non existante
    //  TODO
    return UNDEFINED_INSTRUCTION;
  }
  // Extraction des 3 prochains bits 27 - 25
  uint8_t code = (uint8_t)get_bits(instruction, 27, 25);

  switch (code)
  {
  case 0b000: // Data processing immediate shift or register shift
    printf (" case  data processing \n") ;
    if (get_bits(instruction, 24, 20) == 0b10000 || get_bits(instruction, 24, 20) == 0b10010 || get_bits(instruction, 24, 20) == 0b10110 || get_bits(instruction, 24, 20) == 0b10100 ) 
    {
      resultat = arm_miscellaneous(p, instruction);
    }
    else if (get_bit(instruction, 4) & get_bit(instruction, 7))
    {
            // case offset 
      resultat = arm_load_store (p, instruction);
    }
    else 
    {
      resultat = arm_data_processing_immediate(p, instruction);
    }
 
    break;
  case 0b001: // Data processing immediate or move immediate to status register


      if ((get_bits(instruction, 24, 23)) == 0b10)
      {
        if (get_bits(instruction, 21, 20) == 0b10)
        {
          resultat = arm_data_processing_immediate_msr(p, instruction);
        }
        else
         resultat = arm_data_processing_immediate(p, instruction);

        if (get_bits(instruction, 21, 20) == 0b00)
        {
          resultat = UNDEFINED_INSTRUCTION;
        }
        else
        {
          resultat = arm_data_processing_immediate(p, instruction);
        }
      }
      else
      {
        resultat = arm_data_processing_immediate(p, instruction);
      } 

    break;
  case 0b010: // Load/store immediate offset

    resultat = arm_load_store(p, instruction);
    break;

  case 0b011: // Load/store register offset 



      if ((get_bits(instruction, 24, 20) == 0b11111) && (get_bits(instruction, 7, 4) == 0b1111))
      {
        resultat = UNDEFINED_INSTRUCTION;
      }
      if (get_bit(instruction, 4))
      {
        // Media instructions 
        // TODO
        resultat =  UNDEFINED_INSTRUCTION;
      }
      // Load/store register offset
      resultat =  arm_load_store(p, instruction);

    break;
  case 0b100: // Load/store multiple

    resultat = arm_load_store_multiple(p, instruction);
    break;
  case 0b101: // Branch and branch with link

    resultat = arm_branch(p, instruction);

    break;
  case 0b110: // Coprocessor load/store and double register transfers

    resultat = arm_coprocessor_load_store(p, instruction);

    break;
  case 0b111: // Cop data process / Cop register transfers / Software interrupt
    
    
  if (get_bit(instruction, 24))
  {
    resultat = SOFTWARE_INTERRUPT;
  }
  if (get_bit(instruction, 4))
  {
    // coprocessor register transfers
    resultat = arm_coprocessor_others_swi(p, instruction);
  }
  // coprocessor Data processing
    resultat = 0 ;

    break;
  default: // ne dois jamais arriver
    fprintf(stderr, "<arm_execute_instruction> Erreur default dans switch\n");
    return UNDEFINED_INSTRUCTION;
  }

  return resultat;
}

int arm_step(arm_core p)
{
  int result;
  printf ("step\n");
  result = arm_execute_instruction(p);
  //  on affiche la valeur des registres R1 et R2
  printf("R0 = %d\n", arm_read_register(p, 0) );
  printf ("R1 = %d\n", arm_read_register(p, 1));
  printf ("R2 = %d\n", arm_read_register(p, 2));
  printf ("R3 = %d\n", arm_read_register(p, 3));
  printf ("pc = %d\n", arm_read_register(p, 15));
  printf ("cpsr = %d\n", arm_read_register(p, 16));
  printf ("lr = %d\n", arm_read_register(p, 14));
  printf ("===============================\n") ;

  if (result )
  {
    return arm_exception(p, result);
  }
  return result;
}
