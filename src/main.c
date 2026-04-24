#include <stdio.h>
#include <stdlib.h>
#include "pipeline.h"
#include "registerFile.h"
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
    
    if_id->instruction = instruction;
    gpr* R = createRegisters();
    R[2].data = 10;
    R[3].data = 20;

    id_ex->regA_updated = false;
    id_ex->regB_updated = false;

    ex_mem->alu_result = 0;

    ID_stage_decode(if_id, id_ex, R);
    
    EX_execute(id_ex, ex_mem, mem_wb);

    printf("ALU result: %d \n", ex_mem->alu_result);
    
    // addProgramsToMemory();
    // // memory ready

    // //start executing:
    // IF_stage(); //Fetched instruciton 

    return 0;

}

