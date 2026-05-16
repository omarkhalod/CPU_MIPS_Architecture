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

#define BYTE_MASK 0xFF
#define CARRY_MASK 0x100
#define SIGN_MASK 0x80

static void set_add_flags(EX_MEM_Buffer *out, uint32_t a, uint32_t b)
{
    uint32_t temp1 = a & BYTE_MASK;
    uint32_t temp2 = b & BYTE_MASK;
    uint32_t result = temp1 + temp2;

    out->carry = (result & CARRY_MASK) == CARRY_MASK;

    int carry_into_sign = ((temp1 & 0x7F) + (temp2 & 0x7F)) & SIGN_MASK;
    int carry_out_sign = result & CARRY_MASK;

    out->overflow = ((carry_into_sign ? 1 : 0) ^ (carry_out_sign ? 1 : 0));
}

static void set_sub_flags(EX_MEM_Buffer *out, uint32_t a, uint32_t b)
{
    uint32_t temp1 = a & BYTE_MASK;
    uint32_t temp2 = b & BYTE_MASK;
    uint32_t result = temp1 - temp2;

    out->carry = (result & CARRY_MASK) == CARRY_MASK;

    uint32_t res8 = result & BYTE_MASK;

    out->overflow =
        (((temp1 ^ temp2) & SIGN_MASK) != 0) &&
        (((temp1 ^ res8) & SIGN_MASK) != 0);
}

void EX_execute(ID_EX_Buffer *self, EX_MEM_Buffer *out, uint32_t *omar_yahia_var_inbtw_mres, gpr *pc)
{
    out->overflow = false;
    out->carry = false;

    if (self->regA_updated)
    {
        if (prevAluResult == -1)
        {
            self->regA_updated = omar_yahia_var_inbtw_mres;
        }
        else
        {
            self->regA_value = prevAluResult;
            self->regA_updated = false;
        }
    }
    else if (self->regB_updated)
    {
        if (prevAluResult == -1)
        {
            self->regB_updated = omar_yahia_var_inbtw_mres;
        }
        else
        {
            self->regB_value = prevAluResult;
            self->regB_updated = false;
        }
    }

    out->dest_reg = self->dest_reg;
    out->reg_write = self->reg_write;
    uint32_t memAddress;

    switch (self->op)
    {
    case OP_ADD:
        prevAluResult = out->alu_result = self->regA_value + self->regB_value;
        set_add_flags(out, self->regA_value, self->regB_value);
        break;

    case OP_SUB:
        prevAluResult = out->alu_result = self->regA_value - self->regB_value;
        set_sub_flags(out, self->regA_value, self->regB_value);
        break;

    case OP_MUL:
        prevAluResult = out->alu_result = self->regA_value * self->regB_value;
        {
            uint32_t temp1 = self->regA_value & BYTE_MASK;
            uint32_t temp2 = self->regB_value & BYTE_MASK;
            uint32_t result = temp1 * temp2;

            out->carry = (result & CARRY_MASK) == CARRY_MASK;

            int signedA = (int8_t)temp1;
            int signedB = (int8_t)temp2;
            int signedResult = signedA * signedB;

            out->overflow = signedResult > 127 || signedResult < -128;
        }
        break;

    case OP_MOVI:
        break;

    case OP_AND:
        prevAluResult = out->alu_result = self->regA_value & self->regB_value;
        break;

    case OP_ORI:
        break;

    case OP_LSL:
        prevAluResult = out->alu_result = self->regB_value << self->SHAMT;
        out->carry = ((self->regB_value << self->SHAMT) & CARRY_MASK) == CARRY_MASK;
        break;

    case OP_LSR:
        prevAluResult = out->alu_result = self->regB_value >> self->SHAMT;
        break;

    case OP_MOVR:
        memAddress = self->regB_value + self->immediate;
        out->mem_read = memAddress;
        prevAluResult = -1;
        break;

    case OP_MOVM:
        memAddress = self->regB_value + self->immediate;
        out->mem_write = memAddress;
        break;

    case OP_JEQ:
        if (self->dest_reg == self->regA_value)
        {
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
    execute.regA_updated = false; // I didn't update this
    execute.regB_updated = false; // I didn't update this

    EX_MEM_Buffer output;
    MEM_WB_Buffer s;

    EX_execute(&execute, &output, &s);

    printf("output is %d", output.alu_result);

    return 0;
}
#endif
