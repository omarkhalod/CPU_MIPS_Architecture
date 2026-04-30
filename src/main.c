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
    // shamt: 0000000000111 
    uint32_t instruction1 = 0b00000000100010000110000000000111;

    // opcode: 0000
    // R4: 00100
    // R1: 00001
    // R5: 00101
    // shamt: 0000000000111 
    uint32_t instruction2 = 0b00000010000001001010000000000111;

    // opcode: 0001
    // R1: 00001
    // R10: 01010
    // R4: 00100
    // shamt: 0000000000111 
    uint32_t instruction3 = 0b00010000101010001000000000000111;

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
    if_id->instruction = instruction1;
    R[2].data = 10;
    R[3].data = 20;
    R[5].data = 5;
    R[10].data = 100;

    id_ex->regA_updated = false;
    id_ex->regB_updated = false;

    ex_mem->alu_result = 0;

    printf("--- Executing Instruction 1: ADD R1 R2 R3 ---\n");
    ID_stage_decode(if_id, id_ex, R);
    EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);
    printf("ALU Result 1 (R1): %d\n\n", ex_mem->alu_result);

    printf("--- Executing Instruction 2: ADD R4 R1 R5 ---\n");
    if_id->instruction = instruction2;
    ID_stage_decode(if_id, id_ex, R);
    printf("ID stage detected hazard for R1? regA_updated = %s\n", id_ex->regA_updated ? "true" : "false");
    EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);
    printf("ALU Result 2 (R4 = R1 + R5): %d\n", ex_mem->alu_result);

    dequeueInUse();

    printf("\n--- Executing Instruction 3: SUB R1 R10 R4 ---\n");
    if_id->instruction = instruction3;
    ID_stage_decode(if_id, id_ex, R);
    printf("ID stage detected hazard for R4? regB_updated = %s\n", id_ex->regB_updated ? "true" : "false");
    EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);
    printf("ALU Result 3 (R1 = R10 - R4): %d\n", ex_mem->alu_result);

    return 0;

}
