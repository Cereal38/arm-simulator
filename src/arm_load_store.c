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
    //Les positions
    uint8_t posOpCode = 20;
    uint8_t posRd = 12;
    uint8_t posRn = 16;
    uint8_t posL = 22;

    //Extractions
    uint8_t opCode = (ins >> posOpCode) & 0b1111;
    uint8_t rd = (ins >> posRd) & 0b1111;
    uint8_t rn = (ins >> posRn) & 0b1111;
    uint8_t l = (ins >> posL) & 0b1; // savoir si load or store

    uint32_t address;

    switch (l) {
        // Load instructions
        case 1:
            switch (opCode) {
                case 0b0101: // LDR
                    if (rn >= 16) {
                        return UNDEFINED_INSTRUCTION;
                    }
                    address = arm_read_register(p, rn);
                    uint32_t dataToLoad;
                    if (memory_read_word(p->mem, address, &dataToLoad, 1) != 0) {
                        return UNDEFINED_INSTRUCTION;
                    }
                    arm_write_register(p, rd, dataToLoad);
                    return 0;

                case 0b0100: // LDRB
                    if (rn >= 16) {
                        return UNDEFINED_INSTRUCTION;
                    }
                    address = arm_read_register(p, rn);
                    uint8_t dataToLoadByte;
                    if (memory_read_byte(p->mem, address, &dataToLoadByte) != 0) {
                        return UNDEFINED_INSTRUCTION;
                    }
                    arm_write_register(p, rd, (uint32_t)dataToLoadByte);
                    return 0;

                case 0b1000: // LDRH
                    if (rn >= 16) {
                        return UNDEFINED_INSTRUCTION;
                    }
                    address = arm_read_register(p, rn);
                    uint16_t dataToLoadHalfword;
                    if (memory_read_half(p->mem, address, &dataToLoadHalfword, 1) != 0) {
                        return UNDEFINED_INSTRUCTION;
                    }
                    arm_write_register(p, rd, (uint32_t)dataToLoadHalfword);
                    return 0;

                case 0b1001: // LDRSH
                    if (rn >= 16) {
                        return UNDEFINED_INSTRUCTION;
                    }
                    address = arm_read_register(p, rn);
                    int16_t dataToLoadSignedHalfword;
                    if (memory_read_half(p->mem, address, (uint16_t *)&dataToLoadSignedHalfword, 1) != 0) {
                        return UNDEFINED_INSTRUCTION;
                    }
                    arm_write_register(p, rd, (uint32_t)dataToLoadSignedHalfword);
                    return 0;

                default:
                    return UNDEFINED_INSTRUCTION;
            }

        // Store instructions
        case 0:
            switch (opCode) {
                case 0b0110: // STR
                    if (rn >= 16) {
                        return UNDEFINED_INSTRUCTION;
                    }
                    address = arm_read_register(p, rn);
                    uint32_t storedData = arm_read_register(p, rd);
                    if (memory_write_word(p->mem, address, storedData, 1) != 0) {
                        return UNDEFINED_INSTRUCTION;
                    }
                    return 0;

                case 0b0111: // STRB
                    if (rn >= 16) {
                        return UNDEFINED_INSTRUCTION;
                    }
                    address = arm_read_register(p, rn);
                    uint8_t storedDataByte = arm_read_register(p, rd) & 0xFF;
                    if (memory_write_byte(p->mem, address, storedDataByte) != 0) {
                        return UNDEFINED_INSTRUCTION;
                    }
                    return 0;

                case 0b1000: // STRH
                    if (rn >= 16) {
                        return UNDEFINED_INSTRUCTION;
                    }
                    address = arm_read_register(p, rn);
                    uint16_t storedDataHalfword = arm_read_register(p, rd) & 0xFFFF;
                    if (memory_write_half(p->mem, address, storedDataHalfword, 1) != 0) {
                        return UNDEFINED_INSTRUCTION;
                    }
                    return 0;

                default:
                    return UNDEFINED_INSTRUCTION;
            }

        default:
            return UNDEFINED_INSTRUCTION;
    }
    //ne dois jamais etre la
    return UNDEFINED_INSTRUCTION;
}






int number_registers(uint16_t register_list){
    int compteur;
    for (int i = 0; i < 16; i++){
        if((register_list & (1 << i)) != 0)
            compteur++;
    }
    return compteur;
}

//Manque les cas avec le bit S. Manuel page 482.
//LDM et STM info plus générale page 134.
//LDM(1) (apparement c'est lui qu'il faut faire) page 186 du manuel.
//STM(1) page 339 du Manuel.
int arm_load_store_multiple(arm_core p, uint32_t ins) {
    uint8_t posP = 24;
    uint8_t posU = 23;
    uint8_t posS = 22;
    uint8_t posW = 21;
    uint8_t posL = 20;
    
    uint8_t P = (ins >> posP) & 0b1;
    uint8_t U = (ins >> posU) & 0b1;
    uint8_t S = (ins >> posS) & 0b1;
    uint8_t W = (ins >> posW) & 0b1;
    uint8_t L = (ins >> posL) & 0b1;

    uint8_t posRn = 16;//19 à 16
    uint8_t rn = (ins >> posRn) & 0b1111;
    uint16_t register_list = ins & 0b1111;//15 à 0

    uint32_t address = arm_read_register(p, rn);

    int nbr_register_list = number_registers(register_list);

    if(nbr_register_list == 0){
        fprintf(stderr, "<arm_load_store.c> Erreur aucun bit n'est set dans la liste de registres.\n");
        return UNDEFINED_INSTRUCTION;
    }

    if(S != 0){
        fprintf(stderr, "<arm_load_store.c> Erreur le bit S n'est pas à 0.\n");
        return UNDEFINED_INSTRUCTION;
    }
    if(L){//LDM(1)
        for (int i = 0; i < 15; i++){
            if((register_list & (1 << i))!=0){
                if(P == 1){
                    if(U == 0)//decremente BEFORE
                        address -= 4;
                    else//incremente BEFORE
                        address += 4;
                }
                uint32_t Ri = arm_read_register(p, address);
                arm_write_register(p, i, Ri);
                if(P == 0){
                    if(U == 0)//decremente AFTER
                        address -= 4;
                    else//incremente AFTER
                        address += 4;
                }
            }
        }
        if((register_list & (1 << 15))!=0){//si PC est set dans register_list
            uint32_t value = arm_read_register(p, address);
            arm_write_register(p, 15, value);//15 pour pc
            //uint8_t T = value & 0b1;//bit thumb? qu'est ce que c'est que ce truc??
            address += 4;
            // fprintf(stderr, "<arm_load_store.c> Non implémenté : le bit 15 (PC) est set.\n");
            // return UNDEFINED_INSTRUCTION;
            //  des choses incompréhensibles
            // TODO à vérifier ici voilà
        }
    }
    else{//STM(1)
        for (int i = 0; i < 16; i++){
            if((register_list & (1 << i))!=0){
                if(P == 1){
                    if(U == 0)//decremente BEFORE
                        address -= 4;
                    else//incremente BEFORE
                        address += 4;
                }
                uint32_t Ri = arm_read_register(p, i);
                arm_write_register(p, address, Ri);
                if(P == 0){
                    if(U == 0)//decremente AFTER
                        address -= 4;
                    else//incremente AFTER
                        address += 4;
                }
            }
        }
    }
    if (W == 1){
        if(U == 0)
            address = address - 4 * nbr_register_list;
        else
            address = address + 4 * nbr_register_list;
    }
    return 0;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}
