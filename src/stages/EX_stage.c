//! 1
// TODO:EX should take 2 cyclyes and

//! 2
// TODO: If there is a branch instruction (JMP, JEQ), instructions skipped that are already in the pipeline should
// TODO: be removed from the pipeline(Flush IF and ID) and update the PC in the second cycle

//! 3
// TODO: Have to handle overflow for signed numbers

//! 4
// TODO: and ofc do all the normal execution for the instructions

#include "EX_stage.h"
#include <stdio.h>

void EX_execute(EX_Stage *self, EX_MEM_Register *out)
{
    // pass the des_reg thorugh
    out->dest_reg = self->dest_reg;
    switch (self->op)
    {
    case OP_ADD:
        out->alu_result = self->reg_a + self->reg_b;
        break;
    case OP_SUB:
        out->alu_result = self->reg_a - self->reg_b;
        break;
    case OP_MUL:
        out->alu_result = self->reg_a * self->reg_b;
        break;
    case OP_MOVI:
        // TODO
        break;
    case OP_AND:
        out->alu_result = self->reg_a & self->reg_b;
        break;
    case OP_ORI:
        // TODO
        break;
    case OP_LSL:
        // TODO
        out->alu_result = self->reg_b << self->SHAMT;
        break;
    case OP_LSR:
        // TODO
        out->alu_result = self->reg_b >> self->SHAMT;
        break;
    case OP_MOVR:
        // TODO
        uint32_t memAddress = self->reg_b + self->immediate;
        out->mem_read = memAddress;
        break;
    case OP_MOVM:
        // TODO
        uint32_t memAddress = self->reg_b + self->immediate;
        out->mem_write = memAddress;
        break;
    default:
        break;
    }
}

int main(int argc, char const *argv[])
{
    EX_Stage execute;
    execute.address = 0x0;
    execute.dest_reg = 0x0;
    execute.immediate = 0x01;
    execute.op = (uint8_t)OP_ADD;
    execute.reg_a = 0x5;
    execute.reg_b = 0x3;

    EX_MEM_Register output;

    EX_execute(&execute, &output);

    printf("output is %d", output.alu_result);

    return 0;
}
