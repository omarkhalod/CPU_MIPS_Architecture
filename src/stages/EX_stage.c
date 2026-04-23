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

void EX_execute(ID_EX_Register *self, EX_MEM_Register *out, MEM_WB_Register *wb)
{
    // clear any previous data from out

    //  check if any of the values for reg are not up to date
    if (self->regA_updated)
    {
        self->regA_updated = wb->data;
    }
    else if (self->regB_updated)
    {
        self->regB_updated = wb->data;
    }
    // pass the des_reg thorugh
    out->dest_reg = self->dest_reg;
    uint32_t memAddress;

    switch (self->op)
    {
    case OP_ADD:
        out->alu_result = self->regA + self->regB;
        break;
    case OP_SUB:
        out->alu_result = self->regA - self->regB;
        break;
    case OP_MUL:
        out->alu_result = self->regA * self->regB;
        break;
    case OP_MOVI:
        // TODO
        break;
    case OP_AND:
        out->alu_result = self->regA & self->regB;
        break;
    case OP_ORI:
        // TODO
        break;
    case OP_LSL:
        // TODO
        out->alu_result = self->regB << self->SHAMT;
        break;
    case OP_LSR:
        // TODO
        out->alu_result = self->regB >> self->SHAMT;
        break;
    case OP_MOVR:
        // TODO
        memAddress = self->regB + self->immediate;
        out->mem_read = memAddress;
        break;
    case OP_MOVM:
        // TODO
        memAddress = self->regB + self->immediate;
        out->mem_write = memAddress;
        break;
    default:
        break;
    }
}

int main(int argc, char const *argv[])
{
    ID_EX_Register execute;
    execute.address = 0x0;
    execute.dest_reg = 0x0;
    execute.immediate = 0x01;
    execute.op = (uint8_t)OP_ADD;
    execute.regA = 0x5;
    execute.regB = 0x3;
    execute.regA_updated = false;
    execute.regB_updated = false;

    EX_MEM_Register output;
    MEM_WB_Register s;

    EX_execute(&execute, &output, &s);

    printf("output is %d", output.alu_result);

    return 0;
}
