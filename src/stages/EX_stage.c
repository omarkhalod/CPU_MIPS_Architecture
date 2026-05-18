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

#define BYTE_MASK 0xFF
#define CARRY_MASK 0x100
#define SIGN_MASK 0x80

int prevAluResult;
bool prevAluResultValid = false;

static void set_add_flags(EX_MEM_Buffer *out, uint32_t a, uint32_t b)
{
    uint32_t temp1 = a & BYTE_MASK;
    uint32_t temp2 = b & BYTE_MASK;
    uint32_t result = temp1 + temp2;

    out->carry = (result & CARRY_MASK) == CARRY_MASK;

    int carry_into_sign = (((temp1 & 0x7F) + (temp2 & 0x7F)) & SIGN_MASK) != 0;
    int carry_out_sign = (result & CARRY_MASK) != 0;

    out->overflow = carry_into_sign ^ carry_out_sign;
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
    out->instructionNumber = self->instructionNumber;

    out->mem_read = -1;
    out->mem_write = -1;

    out->overflow = false;
    out->carry = false;

    if (self->regA_updated)
    {
        printf("EX: *** HAZARD DETECTED on RegA! ***\n");
        printf("EX: Resolving RegA hazard...\n");
        if (!prevAluResultValid)
        {
            printf("EX: RegA hazard waiting on memory load...\n");
            self->regA_value = *omar_yahia_var_inbtw_mres;
            prevAluResult = self->regA_value;
            prevAluResultValid = true;
            self->regA_updated = false;
            printf("EX: *** HAZARD RESOLVED! *** RegA value updated from memory to %d\n", self->regA_value);
        }
        else
        {
            self->regA_value = prevAluResult;
            self->regA_updated = false;
            printf("EX: *** HAZARD RESOLVED! *** RegA value updated from ALU to %d\n", self->regA_value);
        }
    }

    if (self->regB_updated)
    {
        printf("EX: *** HAZARD DETECTED on RegB! ***\n");
        printf("EX: Resolving RegB hazard...\n");
        if (!prevAluResultValid)
        {
            printf("EX: RegB hazard waiting on memory load...\n");
            self->regB_value = *omar_yahia_var_inbtw_mres;
            prevAluResult = self->regB_value;
            prevAluResultValid = true;
            self->regB_updated = false;
            printf("EX: *** HAZARD RESOLVED! *** RegB value updated from memory to %d\n", self->regB_value);
        }
        else
        {
            self->regB_value = prevAluResult;
            self->regB_updated = false;
            printf("EX: *** HAZARD RESOLVED! *** RegB value updated from ALU to %d\n", self->regB_value);
        }
    }

    out->dest_reg = self->dest_reg;
    out->reg_write = self->reg_write;
    uint32_t memAddress;

    switch (self->op)
    {
    case OP_ADD:
        prevAluResult = out->alu_result = self->regA_value + self->regB_value;
        prevAluResultValid = true;
        set_add_flags(out, self->regA_value, self->regB_value);
        printf("EX: ADD %d + %d = %d | Carry = %d | Overflow = %d\n",
               self->regA_value, self->regB_value, out->alu_result, out->carry, out->overflow);
        break;

    case OP_SUB:
        prevAluResult = out->alu_result = self->regA_value - self->regB_value;
        prevAluResultValid = true;
        set_sub_flags(out, self->regA_value, self->regB_value);
        printf("EX: SUB %d - %d = %d | Carry = %d | Overflow = %d\n",
               self->regA_value, self->regB_value, out->alu_result, out->carry, out->overflow);
        break;

    case OP_MUL:
        prevAluResult = out->alu_result = self->regA_value * self->regB_value;
        prevAluResultValid = true;
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
        printf("EX: MUL %d * %d = %d | Carry = %d | Overflow = %d\n",
               self->regA_value, self->regB_value, out->alu_result, out->carry, out->overflow);
        break;

    case OP_MOVI:
        prevAluResult = out->alu_result = self->immediate;
        prevAluResultValid = true;
        printf("EX: MOVI setting result to %d\n", out->alu_result);
        break;

    case OP_AND:
        prevAluResult = out->alu_result = self->regA_value & self->regB_value;
        prevAluResultValid = true;
        printf("EX: AND %d & %d = %d\n", self->regA_value, self->regB_value, out->alu_result);
        break;

    case OP_ORI:
        prevAluResult = out->alu_result = self->regA_value | self->immediate;
        prevAluResultValid = true;
        printf("EX: ORI %d | %d = %d\n", self->regA_value, self->immediate, out->alu_result);
        break;

    case OP_LSL:
        prevAluResult = out->alu_result = self->regA_value << self->SHAMT;
        prevAluResultValid = true;
        out->carry = ((self->regA_value << self->SHAMT) & CARRY_MASK) == CARRY_MASK;
        printf("EX: LSL %d << %d = %d | Carry = %d | Overflow = %d\n",
               self->regA_value, self->SHAMT, out->alu_result, out->carry, out->overflow);
        break;

    case OP_LSR:
        prevAluResult = out->alu_result = self->regA_value >> self->SHAMT;
        prevAluResultValid = true;
        printf("EX: LSR %d >> %d = %d\n", self->regA_value, self->SHAMT, out->alu_result);
        break;

    case OP_MOVR:
        memAddress = self->regA_value + self->immediate;
        out->mem_read = memAddress;
        prevAluResultValid = false;
        printf("EX: MOVR calculated address = %d\n", memAddress);
        break;

    case OP_MOVM:
        memAddress = self->regA_value + self->immediate;
        out->mem_write = memAddress;
        out->alu_result = self->regB_value; // Forward value to MEM stage for writing
        prevAluResult = out->alu_result;
        prevAluResultValid = true;
        printf("EX: MOVM calculated address = %d, data to write = %d\n", memAddress, out->alu_result);
        break;

    case OP_JEQ:
        printf("EX: JEQ comparing %d and %d\n", self->regA_value, self->regB_value);
        if (self->regA_value == self->regB_value)
        {
            pc->data = pc->data - 1 + self->immediate;
            out->flush = true;
            printf("EX: JEQ condition met, branch taken to %d\n", pc->data);
        }
        break;

    case OP_JMP:
        pc->data = (pc->data & 0xF0000000) | self->address;
        out->flush = true;
        printf("EX: JMP jumping to %d\n", pc->data);
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
