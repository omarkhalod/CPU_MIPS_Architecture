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
#include "stages/IF_stage.h"


void printMemory(memory* mem, int lines) {
    printf("--- Memory Dump (First %d lines) ---\n", lines);
    for (int i = 0; i < lines && i < 2048; i++) {
        printf("MEM[%d]: data=0x%08X, isValid=%s\n", i, mem[i].data, mem[i].isValid ? "true" : "false");
    }
    printf("-------------------------------------\n");
}

int main(int argc, char const *argv[])
{   
    
    IF_ID_Buffer* if_id = calloc(1, sizeof(IF_ID_Buffer));
    ID_EX_Buffer* id_ex = calloc(1, sizeof(ID_EX_Buffer));
    EX_MEM_Buffer* ex_mem = calloc(1, sizeof(EX_MEM_Buffer));
    MEM_WB_Buffer* mem_wb = calloc(1, sizeof(MEM_WB_Buffer));
    uint32_t* omar_yahia_var_inbtw_mres = malloc(sizeof(uint32_t));
    (*omar_yahia_var_inbtw_mres) = -1;
    // char* r_instructions[] = {"ADD", "SUB", "MUL", "AND", "LSL", "LSR"};
    // char* i_instructions[] = {"MOVI", "JEQ", "ORI", "MOVR", "MOVM"};
    // char* j_instructions[] = {"JMP"};    
    
    memory* memory = createMemory();
    // function to read from disk to memory
    gpr* R = createRegisters();
    // function to initialize some register data
    gpr* pc = createPC();

    loadProgramFileToMemory(memory);

    printMemory(memory, 10);
        
    R[1].data = 10;
    R[2].data = 10;
    R[4].data = 30;
    R[5].data = 40;

    id_ex->regA_updated = false;
    id_ex->regB_updated = false;

    ex_mem->alu_result = 0;

    int cycle = 1;
    int instructionNumber = 1;
    bool hasIdStarted = false;
    bool hasExStarted = false;
    bool hasMemStarted = false;
    bool hasWbStarted = false;
    bool skipInstructionFetchOnce = false;
    

    while(memory[pc->data].isValid){ //while memory location is valid
        printf("\n----------------------------------Cycle %d----------------------------------\n", cycle);
        if(cycle%2 == 1){
            if(hasWbStarted){
                printf("=================Writing Back Instruction %d=================\n", mem_wb->instructionNumber);
                WB_execute(mem_wb, R);
                dequeueInUse();
                // hasWbStarted = false; copilot el kateb el 7eta dee, need to check it
            }
            if(hasExStarted){
                printf("=================Executing Instruction %d=================\n", id_ex->instructionNumber);
                EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);
                hasMemStarted = true;
                if(ex_mem->flush){
                    hasIdStarted = false;
                    skipInstructionFetchOnce = true;
                }
            }
            if(hasIdStarted){
                printf("=================Decoding Instruction %d=================\n", if_id->instructionNumber);
                ID_execute(if_id, id_ex, R);
                hasExStarted = true;
            }

            if(skipInstructionFetchOnce){
                skipInstructionFetchOnce = false;
            } else{
                if_id->instructionNumber = instructionNumber;
                printf("=================Fetching Instruction %d=================\n", if_id->instructionNumber);
                IF_stage(memory, pc, if_id);
                instructionNumber++;
                hasIdStarted = true;
            }

        } else{
            if(hasMemStarted){
                printf("=================Memory Access for Instruction %d=================\n", ex_mem->instructionNumber);
                MEM_execute(ex_mem, mem_wb, memory, omar_yahia_var_inbtw_mres);
                hasWbStarted = true;
            } else{
                printf("=================No memory access this cycle.=================\n");
            }
            
            //tel3o lazem nekteb el ID wel EX fel even bas without actually running them
            //skipping unnecessary ID cycle
            //skipping unnecessary EX cycle
        }
        cycle++;
    }

    // Hardcoded remaining cycles after cycle 13 to flush the pipeline
    // Cycle 14 (even)
    printf("\n----------------------------------Cycle %d----------------------------------\n", cycle);
    cycle++;
    if(hasMemStarted){
        printf("=================Memory Access for Instruction %d=================\n", ex_mem->instructionNumber);
        MEM_execute(ex_mem, mem_wb, memory, omar_yahia_var_inbtw_mres);
        hasWbStarted = true;
        // hasMemStarted = false;
    } else {
        printf("=================No memory access this cycle.=================\n");
    }

    // Cycle 15 (odd)
    printf("\n----------------------------------Cycle %d----------------------------------\n", cycle);
    cycle++;
    if(hasWbStarted){
        printf("=================Writing Back Instruction %d=================\n", mem_wb->instructionNumber);
        WB_execute(mem_wb, R);
        dequeueInUse();
        // hasWbStarted = false;
    }
    if(hasExStarted){
        printf("=================Executing Instruction %d=================\n", id_ex->instructionNumber);
        EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);
        hasMemStarted = true;
        if(ex_mem->flush){
            hasIdStarted = false;
            skipInstructionFetchOnce = true;
        }
        // hasExStarted = false;
    }
    if(hasIdStarted){
        printf("=================Decoding Instruction %d=================\n", if_id->instructionNumber);
        ID_execute(if_id, id_ex, R);
        hasExStarted = true;
        hasIdStarted = false;
    }

    // Cycle 16 (even)
    printf("\n----------------------------------Cycle %d----------------------------------\n", cycle);
    cycle++;
    if(hasMemStarted){
        printf("=================Memory Access for Instruction %d=================\n", ex_mem->instructionNumber);
        MEM_execute(ex_mem, mem_wb, memory, omar_yahia_var_inbtw_mres);
        hasWbStarted = true;
        // hasMemStarted = false;
    } else {
        printf("=================No memory access this cycle.=================\n");
    }

    // Cycle 17 (odd)
    printf("\n----------------------------------Cycle %d----------------------------------\n", cycle);
    cycle++;
    if(hasWbStarted){
        printf("=================Writing Back Instruction %d=================\n", mem_wb->instructionNumber);
        WB_execute(mem_wb, R);
        dequeueInUse();
        // hasWbStarted = false;
    }
    if(hasExStarted){
        printf("=================Executing Instruction %d=================\n", id_ex->instructionNumber);
        EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);
        hasMemStarted = true;
        if(ex_mem->flush){
            hasIdStarted = false;
            skipInstructionFetchOnce = true;
        }
        hasExStarted = false;
    }
    if(hasIdStarted){
        printf("=================Decoding Instruction %d=================\n", if_id->instructionNumber);
        ID_execute(if_id, id_ex, R);
        hasExStarted = true;
        hasIdStarted = false;
    }

    // Cycle 18 (even)
    printf("\n----------------------------------Cycle %d----------------------------------\n", cycle);
    cycle++;
    if(hasMemStarted){
        printf("=================Memory Access for Instruction %d=================\n", ex_mem->instructionNumber);
        MEM_execute(ex_mem, mem_wb, memory, omar_yahia_var_inbtw_mres);
        hasWbStarted = true;
        hasMemStarted = false;
    } else {
        printf("=================No memory access this cycle.=================\n");
    }

    // Cycle 19 (odd)
    printf("\n----------------------------------Cycle %d----------------------------------\n", cycle);
    cycle++;
    if(hasWbStarted){
        printf("=================Writing Back Instruction %d=================\n", mem_wb->instructionNumber);
        WB_execute(mem_wb, R);
        dequeueInUse();
        hasWbStarted = false;
    }

    return 0;

}
