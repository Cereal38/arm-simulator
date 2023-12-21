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
int verif_cond(uint32_t instruction, registers r) {

  // Extraire les bits de condition (bits 28-31)
  uint8_t condition_bits = (uint8_t) get_bits( instruction, 31 , 28);
  // récupération de l'état des flags
  uint32_t cpsr = registers_read_cpsr(r);

  switch (condition_bits) {
    case 0x0: // EQ (Z == 1)
      if ((cpsr & Z) != 0) { return 1; }
      return 0;
    case 0x1: // NE (Z == 0)
      if ((cpsr & Z) == 0) { return 1; }
      return 0;
    case 0x2: // CS/HS (C == 1)
      if ((cpsr & C) != 0) { return 1; }
      return 0;
    case 0x3: // CC/LO (C == 0)
      if ((cpsr & C) == 0) { return 1; }
      return 0;
    case 0x4: // MI (N == 1)
      if ((cpsr & N) != 0) { return 1; }
      return 0;
    case 0x5: // PL (N == 0)
      if ((cpsr & N) == 0) { return 1; }
      return 0;
    case 0x6: // VS (V == 1)
      if ((cpsr & V) != 0) { return 1; }
      return 0;
    case 0x7: // VC (V == 0)
      if ((cpsr & V) == 0) { return 1; }
      return 0;
    case 0x8: // HI (C == 1 && Z == 0)
      if (((cpsr & C) != 0) && ((cpsr & Z) == 0)) { return 1; }
      return 0;
    case 0x9: // LS (C == 0 || Z == 1)
      if (((cpsr & C) == 0) || ((cpsr & Z) != 0)) { return 1; }
      return 0;
    case 0xA: // GE (N == V)
      if ((cpsr & N) == (cpsr & V)) { return 1; }
      return 0;
    case 0xB: // LT (N != V)
      if ((cpsr & N) != (cpsr & V)) { return 1; }
      return 0;
    case 0xC: // GT (Z == 0 && N == V)
      if (((cpsr & Z) == 0) && ((cpsr & N) == (cpsr & V))) { return 1; }
      return 0;
    case 0xD: // LE (Z == 1 || N != V)
      if (((cpsr & Z) != 0) || ((cpsr & N) != (cpsr & V))) { return 1; }
      return 0;
    case 0xE: // AL (Always, toujours vrai)
      return 1;
    case 0xF: // - Voir condition code 
          // a completer ? 
      return 1;
    default:
      fprintf(stderr, "Condition inconnue : %d\n", condition_bits);
      return -1;
  }
}

int bits_a_0(arm_core p, uint32_t instruction){
  // Extractions des bits qui nous interesse pour distinguer les cas

  int result = 0;
  if ( get_bit(instruction, 4) & get_bit(instruction,7)){
    // multiplies 
    // Extra load/stores
    // A REVOIR !!!
    result = arm_load_store_multiple(p,instruction);
  }
  else{
    if ( (get_bits(instruction, 24 ,23) == 0b10) & ~get_bit(instruction, 20)){
      result = arm_miscellaneous(p, instruction);
    }
    else {
      result = arm_data_processing_shift(p, instruction);
    }
  }
   return result; 
}

int bits_a_1(arm_core p, uint32_t instruction){
  if( (get_bits (instruction , 24 , 23)) == 0b10){
    if(get_bits(instruction, 21, 20) == 0b10) {
      return arm_data_processing_immediate_msr (p ,instruction) ;
    }
    if (get_bits(instruction, 21, 20) == 0b00){
      return UNDEFINED_INSTRUCTION;
    }
  }
  return arm_data_processing_immediate_msr (p , instruction);
}

int bits_a_3(arm_core p, uint32_t instruction){
  if( (get_bits(instruction, 24 , 20) == 0b11111 ) && (get_bits(instruction, 7 , 4) == 0b1111 ) ) {
    // Load/store immediate offset
    return UNDEFINED_INSTRUCTION;
  }
  if (get_bit (instruction , 4)){
    // Media instructions
    return UNDEFINED_INSTRUCTION;
  }
  // Load/store register offset
  return arm_load_store(p , instruction);
}

int bits_a_7 (arm_core p, uint32_t instruction) {
  if( get_bit(instruction, 24)) {
    return SOFTWARE_INTERRUPT;
  }
  if( get_bit(instruction, 4)){
    // coprocessor register transfers
    return arm_coprocessor_others_swi(p , instruction);
  }
  // coprocessor registers transfers
  return arm_coprocessor_others_swi(p , instruction);
}

static int arm_execute_instruction(arm_core p)
{
  uint32_t instruction;
  int resultat = arm_fetch(p, &instruction);

  if (resultat)
  {
    // gestion interruption fetch
    return PREFETCH_ABORT;
  }

  // Verfification de la condition
  int cond = verif_cond(instruction, p->reg);

  if (cond == 0) {
    fprintf(stderr, "Condition non satisfaite \n");
    /// A MODIFIER ? 
    return UNDEFINED_INSTRUCTION;
  }
  if( cond == -1 ){
    fprintf(stderr, "Condition non existante \n");
    // Gestion interruption conditon non existante
    return UNDEFINED_INSTRUCTION;
  }
  // Extraction des 3 prochains bits 27 - 25 
  uint8_t code = (uint8_t)(((instruction << 3) >> 25) & 0b0111);

  switch (code)
  {
  case 0b000: // Data processing immediate shift
    resultat = bits_a_0(p, instruction);
    break;
  case 0b001: // Data processing immediate
    resultat = bits_a_1( p, instruction);
    break;
  case 0b010: // Load/store immediate offset
    resultat = arm_load_store( p,  instruction);
    break;
  case 0b011: // Load/store register offset
    resultat = bits_a_3( p, instruction );
    break;
  case 0b100: // Load/store multiple
    resultat = arm_load_store_multiple(p,instruction);
    break;
  case 0b101: // Branch and branch with link
    resultat = arm_branch(p , instruction);
    break;
  case 0b110: // Coprocessor load/store and double register transfers
    resultat = arm_coprocessor_load_store( p, instruction);
    break;
  case 0b111: // Cop data process / Cop register transfers / Software interrupt
    resultat = bits_a_7( p, instruction);
    break;
  default: // ne dois jamais arriver
    fprintf(stderr, "<arm_execute_instruction> Erreur default dans switch\n");
    return UNDEFINED_INSTRUCTION;
  }

  // Incrémente le PC
  uint32_t mode = registers_get_mode(p->reg);
  registers_write(p->reg, 0xF, mode, p->reg->registers[15] + 4);

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
