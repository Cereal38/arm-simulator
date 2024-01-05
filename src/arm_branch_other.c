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
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"
#include <debug.h>
#include <stdlib.h>


int arm_branch(arm_core p, uint32_t ins) {

    // Recuperation de l'adresse de PC
    uint32_t address;
    uint8_t mode = registers_get_mode(p->reg);
    address = registers_read(p->reg, 15, mode);

    // Verification L ( with Link )
    // On met dans LR Le retour à l'instruction suivant PC si L
    if (get_bit(ins,24)){
        // LR registre 14, adress (PC depart)
        registers_write(p->reg, 14, mode, address);
    }

    // recuperation des 24 premiers bits
    int32_t immed = get_bits (ins,23,0);

    // extention du bit de poid fort ( 32 bits )
    if ( get_bit (immed,23) ) {
        immed = immed | (0x3F << 24) ;
    }

    // decalage pour former 32 bits
    immed = (immed << 2);


    // Mettre PC au bon endroit pour le branchement
    registers_write(p->reg, 15, mode, address + immed);
    
} 

int arm_coprocessor_others_swi(arm_core p, uint32_t ins) {
    if (get_bit(ins, 24)) {
        return SOFTWARE_INTERRUPT;
    }
    return UNDEFINED_INSTRUCTION;
}

int arm_miscellaneous(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}
