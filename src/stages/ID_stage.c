#include <stdio.h>
#include <string.h>
#include "pipeline.h"
#include "ID_stage.h"
#include "registerFile.h"

uint32_t inUseArray[] = {-1,-1}; 

void dequeueInUse(){
    inUseArray[0] = inUseArray[1];
    inUseArray[1] = -1;
}

void enqueueInUse(uint32_t R){
    if(inUseArray[0]==-1){
        inUseArray[0] = R;
    } else{
        inUseArray[1] = R;
    }
}

bool inUseContains(uint32_t R){
    return inUseArray[0] == R || inUseArray[1] == R;
}

void ID_stage_decode(IF_ID_Buffer* in, ID_EX_Buffer* out, gpr* R){
    
    out->regA_updated = false;
    out->regB_updated = false;

    uint32_t instruction = in->instruction;
    
    // Decode everything first
    uint8_t opcode = (instruction >> 28) & 0xF;
    uint8_t R1 = (instruction >> 23) & 0x1F;
    uint8_t R2 = (instruction >> 18) & 0x1F;
    uint8_t R3 = (instruction >> 13) & 0x1F;
    uint16_t shamt = instruction & 0x1FFF;
    uint32_t imm = instruction & 0x0003FFFF; 
    uint32_t address = instruction & 0x0FFFFFFF;

    out->op = opcode;
    printf("opcode: %d \n", opcode);

    if(opcode == 4 || opcode == 7 || opcode == 11){
        out->reg_write = false;
    }else{
        out->reg_write = true;
    }

    // Assign the appropriate fields based on opcode
    switch (opcode) {
        case 0: // ADD
        case 1: // SUB
        case 2: // MUL
        case 5: // AND
            printf("destination register: R%d \n", R3);
            printf("operand 1 register: R%d \n", R1);
            printf("operand register 1 value: %d \n", R[R1].data);
            printf("operand 2 register: R%d \n", R2);
            printf("operand 2 regiseter value: %d \n", R[R2].data);

            out->dest_reg = R3;
            enqueueInUse(R3); //we will dequeue after WB in pipeline /.,mnb/.,mnbvcxzasdfghjkl;poiuytrewq

            out->regA_value = R[R1].data;
            out->rs1 = R1;
            if(inUseContains(R1)){
                out->regA_updated = true;
            }

            out->regB_value = R[R2].data;
            out->rs2 = R2;
            if(inUseContains(R2)){
                out->regB_updated = true;
            }

            out->SHAMT = shamt;
            printf("shamt: %d \n", shamt);
            break;

        case 8: // LSL
        case 9: // LSR
            printf("destination register: R%d \n", R1);
            printf("operand 1 register: R%d \n", R2);
            printf("operand register 1 value: %d \n", R[R2].data);
            printf("operand 2 register: R%d \n", R3);
            printf("operand 2 regiseter value: %d \n", R[R3].data);

            out->dest_reg = R1;
            enqueueInUse(R1); //we will dequeue after WB in pipeline /.,mnb/.,mnbvcxzasdfghjkl;poiuytrewq

            out->regA_value = R[R2].data;
            out->rs1 = R2;
            if(inUseContains(R2)){
                out->regA_updated = true;
            }

            out->regB_value = R[R3].data;
            out->rs2 = R3;
            if(inUseContains(R3)){
                out->regB_updated = true;
            }

            out->SHAMT = shamt;
            printf("shamt: %d \n", shamt);
            break;

        case 3: // MOVI
        case 4: // JEQ
        case 6: // ORI
        case 10: // MOVR
        case 11: // MOVM
            printf("destination register: R%d \n", R1);
            printf("operand 1 register: R%d \n", R2);
            printf("operand register 1 value: %d \n", R[R2].data);

            if(opcode == 4){
                out->dest_reg = R[R1].data;
            } else {
                out->dest_reg = R1;
            }
            enqueueInUse(R1); //we will dequeue after WB in pipeline /.,mnb/.,mnbvcxzasdfghjkl;poiuytrewq

            out->regA_value = R[R2].data;
            out->rs1 = R2;
            out->rs2 = 0;
            if(inUseContains(R2)){
                out->regA_updated = true;
            }

            out->immediate = imm;
            break;

        case 7: // JMP
            out->address = address;
            break;

        default:
            break;
    }

    printf("inUseArray: [%d, %d]\n", inUseArray[0], inUseArray[1]);
}