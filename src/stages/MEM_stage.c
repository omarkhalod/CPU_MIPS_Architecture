#include <stdio.h>
#include "MEM_stage.h"
#include "memory.h"

void MEM_execute(EX_MEM_Buffer *in, MEM_WB_Buffer *out, memory *mem, uint32_t* omar_yahia_var_inbtw_mres)
{

    out->dest_reg = in->dest_reg; // forward to WB to know which register to write back to
    out->reg_write = in->reg_write; // forward the reg_write signal to WB
    if (in->mem_read != -1)
    {
        // LOAD (MOVR): R1 = MEM[R2 + IMM]
        int address = in->mem_read;

        out->data = mem[address].data; // goes to MEM_WB_Buffer
        (*omar_yahia_var_inbtw_mres) = out->data;

        printf("MEM: LOAD - Read value %d from address %d\n", out->data, address);  
    }
    else if (in->mem_write != -1)
    {
        // STORE (MOVM): MEM[R2 + IMM] = R1
        int address = in->mem_write;

        mem[address].data = in->alu_result;
        out->data = in->alu_result;
        printf("MEM: STORE - Wrote value %d to address %d\n", in->alu_result, address);
    }
    else
    {
        // Non-memory instruction: forward ALU result to WB
        out->data = in->alu_result;
        printf("MEM: No memory op - Forwarding ALU result %d\n", in->alu_result);
    }
}