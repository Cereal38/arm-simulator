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
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "util.h"
#include "debug.h"

int arm_load_store(arm_core p, uint32_t ins) {
    uint8_t posOpCode = 20;        //24 to 20
    uint8_t posRd = 12 ;          //15 to 12
    uint8_t posRn = 16 ;          //19 to 16

    uint8_t opCode = (ins >> posOpCode) & 0b1111 ;
    uint8_t rd = (ins >> posRd) & 0b1111;
    uint8_t rn = (ins >> posRn) & 0b1111;

    switch(opCode) {
        //LDR
        case 0b0101 : 
            //load into rd the value in rn
            if (rn >= 16){
                return UNDEFINED_INSTRUCTION;
            }
            uint32_t address = arm_read_register(p, rn);
            uint32_t dataToLoad;
            if (arm_read_word(p, address , &dataToLoad) != 0){
                return UNDEFINED_INSTRUCTION;
            }
            arm_write_register(p, rd, dataToLoad);
            return 0;


        //STR
        case 0b0110 :
            if (rn >= 16){
                return UNDEFINED_INSTRUCTION; 
            }
            uint32_t address = arm_read_register(p, rn);
            uint32_t storedData = arm_read_register(p, rd);
            if (arm_read_word(p, address , storedData) != 0){  
                //si ca marche pas
                return UNDEFINED_INSTRUCTION;
            }
            //sinon
            return 0;
        default :
            return UNDEFINED_INSTRUCTION;


    }

    return UNDEFINED_INSTRUCTION;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
    uint8_t posOpCode = 20;//24 a 20
    uint8_t posRn = 16;//19 a 16
    uint8_t opCode = (ins >> posOpCode) & 0b1111;
    uint8_t rn = (ins >> posRn) & 0b1111;
    uint16_t register_list = ins & 0b1111;
    uint32_t address = arm_read_register(p, rn);
    switch (opCode)
    {
    // LDM
    case 0b0100:
        for (int i = 0; i < 16; i++){
            if((register_list & (1 << i)) == 1){
                arm_write_register(p, i, )
            }
        }

        // STM
        case 0b0101:
        default:
            return UNDEFINED_INSTRUCTION;
    }
}
int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}
