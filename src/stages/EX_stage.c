//! 1
// TODO: If there is a branch instruction (JMP, JEQ), instructions skipped that are already in the pipeline should
// TODO: be removed from the pipeline(Flush IF and ID) and update the PC in the second cycle

//! 2
// TODO: Have to handle overflow for signed numbers

//! 3
// TODO: and ofc do all the normal execution for the instructions

#include "EX_stage.h"
#include "registerFile.h"
#include <stdio.h>

void EX_execute(ID_EX_Buffer *self, EX_MEM_Buffer *out, MEM_WB_Buffer *mem_wb, gpr* R, gpr* pc)
{
    // clear any previous data from out
    out->mem_read = -1;
    out->mem_write = -1;
    out->flush = false;

    // Forwarding logic for data hazards
    if (self->regA_updated) //I think dequeueing should happen here
    {
        if (mem_wb->reg_write && mem_wb->dest_reg == self->rs1) {
            self->regA_value = mem_wb->data; // Distance 1: value is in MEM_WB
        } else {
            self->regA_value = R[self->rs1].data; // Distance 2: value has been written to register file
        }
    }
    if (self->regB_updated) //And dequeueing also here
    {
        if (mem_wb->reg_write && mem_wb->dest_reg == self->rs2) {
            self->regB_value = mem_wb->data;
        } else {
            self->regB_value = R[self->rs2].data;
        }        
    }
    // pass the des_reg through
    out->dest_reg = self->dest_reg;
    out->reg_write = self->reg_write;
    uint32_t memAddress;

    switch (self->op)
    {
    case OP_ADD:
        out->alu_result = self->regA_value + self->regB_value;
        break;
    case OP_SUB:
        out->alu_result = self->regA_value - self->regB_value;
        break;
    case OP_MUL:
        out->alu_result = self->regA_value * self->regB_value;
        break;
    case OP_MOVI:
        out->alu_result = self->immediate;
        break;
    case OP_AND:
        out->alu_result = self->regA_value & self->regB_value;
        break;
    case OP_ORI:
        out->alu_result = self->regA_value | self->immediate;
        break;
    case OP_LSL:
        // TODO
        out->alu_result = self->regB_value << self->SHAMT;
        break;
    case OP_LSR:
        // TODO
        out->alu_result = self->regB_value >> self->SHAMT;
        break;
    case OP_MOVR:
        memAddress = self->regA_value + self->immediate;
        out->mem_read = memAddress;
        break;
    case OP_MOVM:
        memAddress = self->regA_value + self->immediate;
        out->mem_write = memAddress;
        break;
    case OP_JEQ:
        if(self->dest_reg == self->regA_value){
            // Subtract 1 to account for the extra IF fetch that advanced the global PC
            pc->data = pc->data - 1 + self->immediate;
            out->flush = true;
        }
        break;
    case OP_JMP:
        // Subtract 1 to ensure we use the upper 4 segment bits of the JMP instruction itself
        pc->data = ((pc->data - 1) & 0xF0000000) | self->address;
        out->flush = true;
        break;
    default:
        break;
    }

}

#ifdef EX_STAGE_DEMO_MAIN
int main(int argc, char const *argv[])
{
    ID_EX_Buffer execute;
    execute.address = 0x0;
    execute.dest_reg = 0x0;
    execute.immediate = 0x01;
    execute.op = (uint8_t)OP_ADD;
    execute.regA_value = 0x5;
    execute.regB_value = 0x3;
    execute.regA_updated = false;   //I didn't update this
    execute.regB_updated = false;   //I didn't update this

    EX_MEM_Buffer output;
    MEM_WB_Buffer s;

    EX_execute(&execute, &output, &s, NULL, NULL);

    printf("output is %d", output.alu_result);

    return 0;
}
#endif
