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
    // Instruction 1: MOVR R3, R1, 5  (Load memory[R1 + 5] into R3)
    // opcode: 1010 (10)
    // R1 (dest R3): 00011
    // R2 (base R1): 00001
    // imm: 000000000000000101 (5)
    uint32_t instruction1 = 0b10100001100001000000000000000101;

    // Instruction 2: ADD R3 R4 R10 (Add loaded R3 to R4, store in R10)
    // opcode: 0000 (0)
    // R1 (op1 R3):  00011
    // R2 (op2 R4):  00100
    // R3 (dest R10): 01010
    // shamt: 0000000000000 
    uint32_t instruction2 = 0b00000001100100010100000000000000;

    // Pipeline Simulation
    // c1  c2  c3  c4  c5  c6
    // IF1 ID1 EX1 ME1 WB1
    //     IF2 ID2 EX2 ME2 WB2

    IF_ID_Buffer* if_id = calloc(1, sizeof(IF_ID_Buffer));
    ID_EX_Buffer* id_ex = calloc(1, sizeof(ID_EX_Buffer));
    EX_MEM_Buffer* ex_mem = calloc(1, sizeof(EX_MEM_Buffer));
    MEM_WB_Buffer* mem_wb = calloc(1, sizeof(MEM_WB_Buffer));
    
    memory* memory = createMemory();
    gpr* R = createRegisters();
    gpr* pc = createPC();
        
    // Initialize Registers & Memory
    R[1].data = 10;
    R[4].data = 40;

    // Memory value that will be loaded into R3 (Base 10 + Imm 5 = 15)
    memory[15].data = 99;

    id_ex->regA_updated = false;
    id_ex->regB_updated = false;

    ex_mem->alu_result = 0;

    printf("\n=== Cycle 1 ===\n");
    // IF: Fetch Instruction 1
    if_id->instruction = instruction1;

    printf("\n=== Cycle 2 ===\n");
    // ID: Decode Instruction 1
    ID_stage_decode(if_id, id_ex, R);
    // IF: Fetch Instruction 2
    if_id->instruction = instruction2;

    printf("\n=== Cycle 3 ===\n");
    // EX: Execute Instruction 1
    EX_execute(id_ex, ex_mem, mem_wb, R, pc);
    printf("EX: ALU Result for Instruction 1 = %d\n", ex_mem->alu_result);
    // ID: Decode Instruction 2
    ID_stage_decode(if_id, id_ex, R);

    printf("\n=== Cycle 4 ===\n");
    // MEM: Memory Instruction 1
    MEM_execute(ex_mem, mem_wb, memory);
    // EX: Execute Instruction 2
    EX_execute(id_ex, ex_mem, mem_wb, R, pc);
    printf("EX: ALU Result for Instruction 2 = %d\n", ex_mem->alu_result);

    printf("\n=== Cycle 5 ===\n");
    // WB: Write Back Instruction 1
    WB_execute(mem_wb, R);
    dequeueInUse(); // Clear R3 from in-use queue
    // MEM: Memory Instruction 2
    MEM_execute(ex_mem, mem_wb, memory);

    printf("\n=== Cycle 6 ===\n");
    // WB: Write Back Instruction 2
    WB_execute(mem_wb, R);
    dequeueInUse(); // Clear R10 from in-use queue

    printf("\n=== Final Register Values ===\n");
    printf("R3 = %d\n", R[3].data);
    printf("R10 = %d\n", R[10].data);

    return 0;

}
