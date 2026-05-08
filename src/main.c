#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pipeline.h"
#include "registerFile.h"
#include "memory.h"
#include "stages/ID_stage.h"
#include "stages/EX_stage.h"
#include "stages/MEM_stage.h"
#include "stages/WB_stage.h"

int main(int argc, char const *argv[])
{   
    
    IF_ID_Buffer* if_id = calloc(1, sizeof(IF_ID_Buffer));
    ID_EX_Buffer* id_ex = calloc(1, sizeof(ID_EX_Buffer));
    EX_MEM_Buffer* ex_mem = calloc(1, sizeof(EX_MEM_Buffer));
    MEM_WB_Buffer* mem_wb = calloc(1, sizeof(MEM_WB_Buffer));
    uint32_t* omar_yahia_var_inbtw_mres = malloc(sizeof(uint32_t));
    char* r_instructions = {"ADD", "SUB", "MUL", "AND", "LSL", "LSR"};
    char* i_instructions = {"MOVI", "JEQ", "ORI", "MOVR", "MOVM"};
    char* j_instructions = {"JMP"};
    (*omar_yahia_var_inbtw_mres) = -1;
    
    memory* memory = createMemory();
    // function to read from disk to memory
    gpr* R = createRegisters();
    // function to initialize some register data
    gpr* pc = createPC();
        
    R[1].data = 10;
    R[2].data = 10;
    R[4].data = 30;
    R[6].data = 40;

    id_ex->regA_updated = false;
    id_ex->regB_updated = false;

    ex_mem->alu_result = 0;

    int cycle = 1;
    int instructionNumber = 1;
    bool hasIdStarted = false;
    bool hasExStarted = false;
    bool hasMemStarted = false;
    bool hasWbStarted = false;
    
    while(memory[pc->data].isValid){ //while memory location is valid
        if(cycle%2 == 1){
            if(hasWbStarted){
                WB_execute(mem_wb, R);
            }
            if(hasExStarted){
                EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);
                hasMemStarted = true;
            }
            if(hasIdStarted){
                ID_execute(if_id, id_ex, R);
                hasExStarted = true;
            }

            // IF_execute(); BASSEL LESSA HAYE3MEL DEE
            if_id->instructionNumber = instructionNumber++;
            hasIdStarted = true;
        } else{
            if(hasMemStarted){
                MEM_execute(ex_mem, mem_wb, memory, omar_yahia_var_inbtw_mres);
                hasWbStarted = true;
            }
            //skipping unnecessary ID cycle
            //skipping unnecessary EX cycle
        }
        cycle++;
    }

    // Hardcoded remaining cycles after cycle 13 to flush the pipeline
    // Cycle 14 (even)
    if(hasMemStarted){
        MEM_execute(ex_mem, mem_wb, memory, omar_yahia_var_inbtw_mres);
        hasWbStarted = true;
    }

    // Cycle 15 (odd)
    if(hasWbStarted){
        WB_execute(mem_wb, R);
    }
    if(hasExStarted){
        EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);
        hasMemStarted = true;
    }
    if(hasIdStarted){
        ID_execute(if_id, id_ex, R);
        hasExStarted = true;
    }

    // Cycle 16 (even)
    if(hasMemStarted){
        MEM_execute(ex_mem, mem_wb, memory, omar_yahia_var_inbtw_mres);
        hasWbStarted = true;
    }

    // Cycle 17 (odd)
    if(hasWbStarted){
        WB_execute(mem_wb, R);
    }
    if(hasExStarted){
        EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);
        hasMemStarted = true;
    }
    if(hasIdStarted){
        ID_execute(if_id, id_ex, R);
        hasExStarted = true;
    }

    // Cycle 18 (even)
    if(hasMemStarted){
        MEM_execute(ex_mem, mem_wb, memory, omar_yahia_var_inbtw_mres);
        hasWbStarted = true;
    }

    // Cycle 19 (odd)
    if(hasWbStarted){
        WB_execute(mem_wb, R);
    }

    return 0;

}
