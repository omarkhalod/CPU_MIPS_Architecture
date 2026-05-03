#include <stdio.h>
#include <stdlib.h>
#include "pipeline.h"
#include "registerFile.h"
#include "memory.h"
#include "stages/ID_stage.h"
#include "stages/EX_stage.h"
#include "stages/MEM_stage.h"
#include "stages/WB_stage.h"

int main(int argc, char const *argv[])
{   
    // opcode: 0000
    // R1: 00001
    // R2: 00010
    // R3: 00011
    // shamt: 0000000000000 
    uint32_t instruction1 = 0b00000000100010000110000000000000;

    // opcode: 0000
    // R3: 00011
    // R3: 00100
    // R5: 00101
    // shamt: 0000000000000 
    uint32_t instruction2 = 0b00000001100011001010000000000000;

    // opcode: 0000
    // R5: 00101
    // R6: 00110
    // R10: 01010
    // shamt: 0000000000000 
    uint32_t instruction3 = 0b00000010100110010100000000000000;

    // IF ID EX MEM WB
    //    IF ID EX MEM WB    
    //       IF ID EX MEM WB

    IF_ID_Buffer* if_id = calloc(1, sizeof(IF_ID_Buffer));
    ID_EX_Buffer* id_ex = calloc(1, sizeof(ID_EX_Buffer));
    EX_MEM_Buffer* ex_mem = calloc(1, sizeof(EX_MEM_Buffer));
    MEM_WB_Buffer* mem_wb = calloc(1, sizeof(MEM_WB_Buffer));
    uint32_t* omar_yahia_var_inbtw_mres = malloc(sizeof(uint32_t));
    (*omar_yahia_var_inbtw_mres) = -1;
    
    memory* memory = createMemory();
    gpr* R = createRegisters();
    gpr* pc = createPC();
        
    // clock cycle 1: ID
    R[1].data = 10;
    R[2].data = 10;
    R[4].data = 30;
    R[6].data = 40;

    id_ex->regA_updated = false;
    id_ex->regB_updated = false;

    ex_mem->alu_result = 0;


    printf("--- Executing Instruction 1: ADD R1 R2 R3 ---\n");
    if_id->instruction = instruction1;
    ID_stage_decode(if_id, id_ex, R);
    EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);
    printf("ALU Result 1 (R3 = R1 + R2): %d\n\n", ex_mem->alu_result);

    printf("--- Executing Instruction 2: ADD R3 R4 R5 ---\n");
    if_id->instruction = instruction2;
    ID_stage_decode(if_id, id_ex, R);
    printf("ID stage detected hazard for first R3? regA_updated = %s\n", id_ex->regA_updated ? "true" : "false");
    printf("ID stage detected hazard for second R3? regA_updated = %s\n", id_ex->regB_updated ? "true" : "false");
    EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);
    printf("ALU Result 2 (R5 = R3 + R3): %d\n\n", ex_mem->alu_result);

    // dequeueInUse();

    // printf("--- Executing Instruction 3: ADD R5 R6 R10 ---\n");
    // if_id->instruction = instruction3;
    // ID_stage_decode(if_id, id_ex, R);
    // printf("ID stage detected hazard for R5? regA_updated = %s\n", id_ex->regA_updated ? "true" : "false");
    // EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);
    // printf("ALU Result 3 (R10 = R5 + R6): %d\n", ex_mem->alu_result);

    return 0;

}
