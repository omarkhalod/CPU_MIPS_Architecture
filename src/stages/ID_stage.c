#include <stdio.h>
#include <string.h>
#include "pipeline.h"
#include "ID_stage.h"
#include "registerFile.h"

void ID_stage_decode(IF_ID_Buffer* in, ID_EX_Buffer* out, gpr* R){
    uint32_t instruction = in->instruction;
    
    uint8_t opcode = (instruction >> 28) & 0xF;
    out->op = opcode;
    printf("opcode: %d \n", opcode);

    if(opcode == 7){
        out->address = instruction & 0x0FFFFFFF;
        return;
    }

    uint8_t R1 = (instruction >> 23) & 0x1F;
    uint8_t R2 = (instruction >> 18) & 0x1F;
    printf("destination register: R%d \n", R1);
    printf("operand 1 register: R%d \n", R2);


    int R2Value = R[R2].data;
    printf("operand register 1 value: %d \n", R2Value);
    

    out->dest_reg = R1;
    out->regA_value = R2Value;

    if(opcode == 3 || opcode == 4 || opcode == 6 || opcode == 10 || opcode ==11){
        uint32_t imm = instruction & 0x0003FFFF; 
        out->immediate = imm;
        return;
    }

    uint8_t R3 = (instruction >> 13) & 0x1F;
    printf("operand 2 register: R%d \n", R3);

    int R3Value = R[R3].data;
    printf("operand 2 regiseter value: %d \n", R3Value);

    uint16_t shamt = instruction & 0x1FFF;
    printf("shamt: %d \n", shamt);


    out->regB_value = R3Value;
    out->SHAMT = shamt;
}