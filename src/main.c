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

#include <inttypes.h>

// Dump simulator state as JSON lines to sim_state.jsonl for GUI consumption
void dump_state(FILE* out, int cycle, IF_ID_Buffer* if_id, ID_EX_Buffer* id_ex, EX_MEM_Buffer* ex_mem, MEM_WB_Buffer* mem_wb, gpr* R, gpr* pc, memory* mem) {
    if (!out) return;
    const int memory_size = 2048;
    fprintf(out, "{");
    fprintf(out, "\"cycle\":%d,", cycle);

    // Registers
    fprintf(out, "\"registers\":[");
    for (int i = 0; i < 32; i++) {
        fprintf(out, "%d", R[i].data);
        if (i < 31) fprintf(out, ",");
    }
    fprintf(out, "],");

    // PC
    fprintf(out, "\"pc\":%d,", pc->data);

    // IF/ID
    fprintf(out, "\"IF_ID\":{\"instruction\":%u,\"instructionNumber\":%d},", if_id->instruction, if_id->instructionNumber);

    // ID/EX
    fprintf(out, "\"ID_EX\":{\"op\":%u,\"SHAMT\":%u,\"regA_value\":%u,\"regB_value\":%u,\"immediate\":%d,\"address\":%u,\"regA_updated\":%s,\"regB_updated\":%s,\"reg_write\":%s,\"instructionNumber\":%d},",
        id_ex->op, id_ex->SHAMT, id_ex->regA_value, id_ex->regB_value, id_ex->immediate, id_ex->address,
        id_ex->regA_updated ? "true" : "false",
        id_ex->regB_updated ? "true" : "false",
        id_ex->reg_write ? "true" : "false",
        id_ex->instructionNumber
    );

    // EX/MEM
    fprintf(out, "\"EX_MEM\":{\"alu_result\":%d,\"dest_reg\":%d,\"mem_read\":%d,\"mem_write\":%d,\"reg_write\":%s,\"flush\":%s,\"instructionNumber\":%d},",
        ex_mem->alu_result, ex_mem->dest_reg, ex_mem->mem_read, ex_mem->mem_write,
        ex_mem->reg_write ? "true" : "false",
        ex_mem->flush ? "true" : "false",
        ex_mem->instructionNumber
    );

    // MEM/WB
    fprintf(out, "\"MEM_WB\":{\"data\":%d,\"dest_reg\":%d,\"reg_write\":%s,\"instructionNumber\":%d}",
        mem_wb->data, mem_wb->dest_reg, mem_wb->reg_write ? "true" : "false", mem_wb->instructionNumber
    );

    // Include the full memory image so the GUI can show any written address.
    fprintf(out, ",\"memory_snapshot\":[");
    for (int i = 0; i < memory_size; i++) {
        fprintf(out, "{\"addr\":%d,\"data\":%u,\"valid\":%s}", i, mem[i].data, mem[i].isValid ? "true" : "false");
        if (i < memory_size - 1) fprintf(out, ",");
    }
    fprintf(out, "]}");
    fprintf(out, "\n");
    fflush(out);
}


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

    memory* memory = createMemory();
    gpr* R = createRegisters();
    gpr* pc = createPC();

    loadProgramFileToMemory(memory);

    // Open state output file for GUI (JSON lines)
    FILE* state_file = fopen("sim_state.jsonl", "w");
    if (!state_file) {
        fprintf(stderr, "Warning: could not open sim_state.jsonl for writing. GUI state will not be produced.\n");
    }

    // printMemory(memory, 2048);
        
    R[1].data = 10;
    R[2].data = 10;
    R[4].data = 30;
    R[5].data = 40;

    memory[1033].data = 100; // R2 (10) + 1023 = 1033
    memory[1033].isValid = true;

    id_ex->regA_updated = false;
    id_ex->regB_updated = false;

    ex_mem->alu_result = 0;
    prevAluResultValid = false;

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
                printf("- WB Stage Input: dest_reg=R%d, data=%d, reg_write=%s\n", mem_wb->dest_reg, mem_wb->data, mem_wb->reg_write ? "true" : "false");
                WB_execute(mem_wb, R);
                dequeueInUse();
                // hasWbStarted = false; copilot el kateb el 7eta dee, need to check it
            }
            if(hasExStarted){
                printf("=================Executing Instruction %d=================\n", id_ex->instructionNumber);
                printf("- EX Stage Input: op=%d, dest_reg=R%d, regA_value=%d, regB_value=%d, immediate=%d, address=%d, SHAMT=%d\n", id_ex->op, id_ex->dest_reg, id_ex->regA_value, id_ex->regB_value, id_ex->immediate, id_ex->address, id_ex->SHAMT);
                EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);
                hasMemStarted = true;
                if(ex_mem->flush){
                    hasIdStarted = false;
                    skipInstructionFetchOnce = true;
                }
            } else {
                hasMemStarted = false;
                prevAluResultValid = false;
            }
            if(hasIdStarted){
                    printf("=================Decoding Instruction %d=================\n", if_id->instructionNumber);
                    printf("- ID Stage Input: instruction=0x%08X\n", if_id->instruction);
                    ID_execute(if_id, id_ex, R);
                    hasExStarted = true;
            } else {
                hasExStarted = false;
            }

            if(skipInstructionFetchOnce){
                skipInstructionFetchOnce = false;
            } else{
                if_id->instructionNumber = instructionNumber;
                printf("=================Fetching Instruction %d=================\n", if_id->instructionNumber);
                printf("- IF Stage Input: PC=%d\n", pc->data);
                IF_stage(memory, pc, if_id);
                instructionNumber++;
                hasIdStarted = true;
            }

        } else{
            if(hasMemStarted){
                printf("=================Memory Access for Instruction %d=================\n", ex_mem->instructionNumber);
                printf("- MEM Stage Input: alu_result=%d, dest_reg=R%d, mem_read=%d, mem_write=%d, reg_write=%s\n", ex_mem->alu_result, ex_mem->dest_reg, ex_mem->mem_read, ex_mem->mem_write, ex_mem->reg_write ? "true" : "false");
                MEM_execute(ex_mem, mem_wb, memory, omar_yahia_var_inbtw_mres);
                hasWbStarted = true;
            } else{
                printf("=================No memory access this cycle.=================\n");
            }
            
            //tel3o lazem nekteb el ID wel EX fel even bas without actually running them
            //skipping unnecessary ID cycle
            //skipping unnecessary EX cycle
        }
        // dump state for this cycle before incrementing
        if (state_file) dump_state(state_file, cycle, if_id, id_ex, ex_mem, mem_wb, R, pc, memory);
        cycle++;
    }

    // Hardcoded remaining cycles after cycle 13 to flush the pipeline
    // Cycle 14 (even)
    printf("\n----------------------------------Cycle %d----------------------------------\n", cycle);
    cycle++;
    if(hasMemStarted){
        printf("=================Memory Access for Instruction %d=================\n", ex_mem->instructionNumber);
        printf("- MEM Stage Input: alu_result=%d, dest_reg=R%d, mem_read=%d, mem_write=%d, reg_write=%s\n", ex_mem->alu_result, ex_mem->dest_reg, ex_mem->mem_read, ex_mem->mem_write, ex_mem->reg_write ? "true" : "false");
        MEM_execute(ex_mem, mem_wb, memory, omar_yahia_var_inbtw_mres);
        hasWbStarted = true;
        // hasMemStarted = false;
    } else {
        printf("=================No memory access this cycle.=================\n");
    }
    if (state_file) dump_state(state_file, cycle-1, if_id, id_ex, ex_mem, mem_wb, R, pc, memory);

    // Cycle 15 (odd)
    printf("\n----------------------------------Cycle %d----------------------------------\n", cycle);
    cycle++;
    if(hasWbStarted){
        printf("=================Writing Back Instruction %d=================\n", mem_wb->instructionNumber);
        printf("- WB Stage Input: dest_reg=R%d, data=%d, reg_write=%s\n", mem_wb->dest_reg, mem_wb->data, mem_wb->reg_write ? "true" : "false");
        WB_execute(mem_wb, R);
        dequeueInUse();
        // hasWbStarted = false;
    }
    if(hasExStarted){
        printf("=================Executing Instruction %d=================\n", id_ex->instructionNumber);
        printf("- EX Stage Input: op=%d, dest_reg=R%d, regA_value=%d, regB_value=%d, immediate=%d, address=%d, SHAMT=%d\n", id_ex->op, id_ex->dest_reg, id_ex->regA_value, id_ex->regB_value, id_ex->immediate, id_ex->address, id_ex->SHAMT);
        EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);
        hasMemStarted = true;
        if(ex_mem->flush){
            hasIdStarted = false;
            skipInstructionFetchOnce = true;
        }
        // hasExStarted = false;
        } else {
            prevAluResultValid = false;
    }
    if(hasIdStarted){
        printf("=================Decoding Instruction %d=================\n", if_id->instructionNumber);
        printf("- ID Stage Input: instruction=0x%08X\n", if_id->instruction);
        ID_execute(if_id, id_ex, R);
        hasExStarted = true;
        hasIdStarted = false;
    }
    if (state_file) dump_state(state_file, cycle-1, if_id, id_ex, ex_mem, mem_wb, R, pc, memory);

    // Cycle 16 (even)
    printf("\n----------------------------------Cycle %d----------------------------------\n", cycle);
    cycle++;
    if(hasMemStarted){
        printf("=================Memory Access for Instruction %d=================\n", ex_mem->instructionNumber);
        printf("- MEM Stage Input: alu_result=%d, dest_reg=R%d, mem_read=%d, mem_write=%d, reg_write=%s\n", ex_mem->alu_result, ex_mem->dest_reg, ex_mem->mem_read, ex_mem->mem_write, ex_mem->reg_write ? "true" : "false");
        MEM_execute(ex_mem, mem_wb, memory, omar_yahia_var_inbtw_mres);
        hasWbStarted = true;
        // hasMemStarted = false;
    } else {
        printf("=================No memory access this cycle.=================\n");
    }
    if (state_file) dump_state(state_file, cycle-1, if_id, id_ex, ex_mem, mem_wb, R, pc, memory);

    // Cycle 17 (odd)
    printf("\n----------------------------------Cycle %d----------------------------------\n", cycle);
    cycle++;
    if(hasWbStarted){
        printf("=================Writing Back Instruction %d=================\n", mem_wb->instructionNumber);
        printf("- WB Stage Input: dest_reg=R%d, data=%d, reg_write=%s\n", mem_wb->dest_reg, mem_wb->data, mem_wb->reg_write ? "true" : "false");
        WB_execute(mem_wb, R);
        dequeueInUse();
        // hasWbStarted = false;
    }
    if(hasExStarted){
        printf("=================Executing Instruction %d=================\n", id_ex->instructionNumber);
        printf("- EX Stage Input: op=%d, dest_reg=R%d, regA_value=%d, regB_value=%d, immediate=%d, address=%d, SHAMT=%d\n", id_ex->op, id_ex->dest_reg, id_ex->regA_value, id_ex->regB_value, id_ex->immediate, id_ex->address, id_ex->SHAMT);
        EX_execute(id_ex, ex_mem, omar_yahia_var_inbtw_mres, pc);
        hasMemStarted = true;
        if(ex_mem->flush){
            hasIdStarted = false;
            skipInstructionFetchOnce = true;
        }
        hasExStarted = false;
        } else {
            prevAluResultValid = false;
    }
    if(hasIdStarted){
        printf("=================Decoding Instruction %d=================\n", if_id->instructionNumber);
        printf("- ID Stage Input: instruction=0x%08X\n", if_id->instruction);
        ID_execute(if_id, id_ex, R);
        hasExStarted = true;
        hasIdStarted = false;
    }

    // Cycle 18 (even)
    printf("\n----------------------------------Cycle %d----------------------------------\n", cycle);
    cycle++;
    if(hasMemStarted){
        printf("=================Memory Access for Instruction %d=================\n", ex_mem->instructionNumber);
        printf("- MEM Stage Input: alu_result=%d, dest_reg=R%d, mem_read=%d, mem_write=%d, reg_write=%s\n", ex_mem->alu_result, ex_mem->dest_reg, ex_mem->mem_read, ex_mem->mem_write, ex_mem->reg_write ? "true" : "false");
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
        printf("- WB Stage Input: dest_reg=R%d, data=%d, reg_write=%s\n", mem_wb->dest_reg, mem_wb->data, mem_wb->reg_write ? "true" : "false");
        WB_execute(mem_wb, R);
        dequeueInUse();
        hasWbStarted = false;
    }
    if (state_file) dump_state(state_file, cycle-1, if_id, id_ex, ex_mem, mem_wb, R, pc, memory);

    // printf("\n--- Final Register States (c) ---\n");
    // for (int i = 0; i < 32; i++) {
    //     printf("R%d: %d\n", i, R[i].data);
    // }
    // printf("PC: %d\n", pc->data);
    
    // printf("\n--- Final Memory States ---\n");
    // printMemory(memory, 2048);

    return 0;

}
