#include <stdio.h>
#include <stdlib.h>
#include "pipeline.h"
#include "registerFile.h"
#include "memory.h"
#include "stages/ID_stage.h"
#include "stages/EX_stage.h"

int main(int argc, char const *argv[])
{   
    // opcode: 0000
    // R1: 00001
    // R2: 00010
    // R3: 00011
    // shamt: 0000000000111 
    uint32_t instruction = 0b00000000100010000110000000000111;

    IF_ID_Buffer* if_id = malloc(sizeof(IF_ID_Buffer));
    ID_EX_Buffer* id_ex = malloc(sizeof(ID_EX_Buffer));
    EX_MEM_Buffer* ex_mem = malloc(sizeof(EX_MEM_Buffer));
    MEM_WB_Buffer* mem_wb = malloc(sizeof(MEM_WB_Buffer));
    uint32_t* omar_yahia_var_inbtw_mres = malloc(sizeof(uint32_t*));
    (*omar_yahia_var_inbtw_mres) = -1;
    
    memory* memory = createMemory();
    gpr* R = createRegisters();
    gpr* pc = createPC();
    
    //Don't forget to pass memory/gpr to the stages that need them
    
    if_id->instruction = instruction;
    R[2].data = 10;
    R[3].data = 20;

    id_ex->regA_updated = false;
    id_ex->regB_updated = false;

    ex_mem->alu_result = 0;

    ID_stage_decode(if_id, id_ex, R);
    
    EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);

    printf("ALU result: %d \n", ex_mem->alu_result);

    return 0;

}
