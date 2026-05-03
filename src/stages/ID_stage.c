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

void ID_execute(IF_ID_Buffer* in, ID_EX_Buffer* out, gpr* R){
    
    uint32_t instruction = in->instruction;
    // Decode everything at the beginning of the file
    uint8_t opcode = (instruction >> 28) & 0xF;
    uint8_t R1 = (instruction >> 23) & 0x1F;
    uint8_t R2 = (instruction >> 18) & 0x1F;
    uint8_t R3 = (instruction >> 13) & 0x1F;
    uint16_t shamt = instruction & 0x1FFF;
    uint32_t imm = instruction & 0x0003FFFF; 
    uint32_t address = instruction & 0x0FFFFFFF;

    printf("opcode: %d \n", opcode);
    printf("inUseArray: [%d, %d]\n", inUseArray[0], inUseArray[1]);
    printf("shamt: %d \n", shamt);

    out->op = opcode;
    out->SHAMT = shamt;
    out->immediate = imm;
    out->address = address;
    out->reg_write = true;
    out->instructionNumber = in->instructionNumber;

    // Switch logic mapping variables and checking hazards
    switch (opcode) {
        case 0: // OP_ADD
        case 1: // OP_SUB
        case 2: // OP_MUL
        case 5: // OP_AND
            printf("destination register: R%d \n", R3);
            printf("operand 1 register: R%d \n", R1);
            printf("operand register 1 value: %d \n", R[R1].data);
            printf("operand 2 register: R%d \n", R2);
            printf("operand 2 regiseter value: %d \n", R[R2].data);

            out->dest_reg = R3;
            out->regA_value = R[R1].data;
            out->regB_value = R[R2].data;
            if (inUseContains(R1)) out->regA_updated = true;
            if (inUseContains(R2)) out->regB_updated = true;
            enqueueInUse(R3);
            break;
            
        case 3:  // OP_MOVI
        case 6:  // OP_ORI
        case 10: // OP_MOVR
            out->dest_reg = R1;
            out->regA_value = R[R2].data;
            if (inUseContains(R2)) out->regA_updated = true;
            enqueueInUse(R1);
            break;

        case 4: // OP_JEQ
            out->dest_reg = R[R1].data; 
            out->regA_value = R[R2].data;
            if (inUseContains(R2)) out->regA_updated = true;
            out->reg_write = false;
            break;

        case 7: // OP_JMP
            out->reg_write = false;
            break;

        case 8: // OP_LSL
        case 9: // OP_LSR
            printf("destination register: R%d \n", R1);
            printf("operand 1 register: R%d \n", R2);
            printf("operand register 1 value: %d \n", R[R2].data);
            printf("operand 2 register: R%d \n", R3);
            printf("operand 2 regiseter value: %d \n", R[R3].data);
            printf("shamt: %d \n", shamt);
                        
            out->dest_reg = R1;
            out->regA_value = R[R2].data;
            out->regB_value = R[R3].data;
            if (inUseContains(R2)) out->regA_updated = true;
            if (inUseContains(R3)) out->regB_updated = true;
            enqueueInUse(R1);
            break;

        case 11: // OP_MOVM
            out->dest_reg = R1;
            out->regA_value = R[R2].data;
            if (inUseContains(R2)) out->regA_updated = true;
            out->reg_write = false;
            break;

        default:
            out->reg_write = false;
            break;
    }
}