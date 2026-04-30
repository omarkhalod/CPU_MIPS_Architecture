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
int prevAluResult;

void EX_execute(ID_EX_Buffer *self, EX_MEM_Buffer *out, uint32_t* omar_yahia_var_inbtw_mres, gpr* pc)
{
    // clear any previous data from out

    //  check if any of the values for reg are not up to date
    if (self->regA_updated)
    {
        if(prevAluResult == -1){
            self->regA_updated = omar_yahia_var_inbtw_mres;
        } else{
            self->regA_value = prevAluResult;
            self->regA_updated = false;
        }
    }
    else if (self->regB_updated)
    {
        if(prevAluResult == -1){
            self->regB_updated = omar_yahia_var_inbtw_mres;
        } else{
            self->regB_value = prevAluResult;
            self->regB_updated = false;
        }        
    }
    // pass the des_reg through
    out->dest_reg = self->dest_reg;
    out->reg_write = self->reg_write;
    uint32_t memAddress;

    switch (self->op)
    {
    case OP_ADD:
        prevAluResult = out->alu_result = self->regA_value + self->regB_value;
        break;
    case OP_SUB:
        prevAluResult = out->alu_result = self->regA_value - self->regB_value;
        break;
    case OP_MUL:
        prevAluResult = out->alu_result = self->regA_value * self->regB_value;
        break;
    case OP_MOVI:
        // TODO
        break;
    case OP_AND:
        prevAluResult = out->alu_result = self->regA_value & self->regB_value;
        break;
    case OP_ORI:
        // TODO
        break;
    case OP_LSL:
        // TODO
        prevAluResult = out->alu_result = self->regB_value << self->SHAMT;
        break;
    case OP_LSR:
        // TODO
        prevAluResult = out->alu_result = self->regB_value >> self->SHAMT;
        break;
    case OP_MOVR:
        // TODO
        memAddress = self->regB_value + self->immediate;
        out->mem_read = memAddress;
        prevAluResult = -1;
        break;
    case OP_MOVM:
        // TODO
        memAddress = self->regB_value + self->immediate;
        out->mem_write = memAddress;
        break;
    case OP_JEQ:
        if(self->dest_reg == self->regA_value){
            pc->data = pc->data + self->immediate;
            out->flush = true;
        }
        break;
    case OP_JMP:
        pc->data = (pc->data & 0xF0000000) | self->address;
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

    EX_execute(&execute, &output, &s);

    printf("output is %d", output.alu_result);

    return 0;
}
#endif
