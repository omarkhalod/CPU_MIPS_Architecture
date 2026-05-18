#if !defined(EXECUTE_TEMPLATE)
#define EXECUTE_TEMPLATE

#include <stdint.h>
#include "registerFile.h"
#include "pipeline.h"
typedef enum
{
    // 0X00 is HEXADECIMAL
    OP_ADD = 0x0,
    OP_SUB = 0x1,
    OP_MUL = 0x2,
    OP_MOVI = 0x3,
    OP_JEQ = 0x4,
    OP_AND = 0x5,
    OP_ORI = 0x6,
    OP_JMP = 0x7,
    OP_LSL = 0x8,
    OP_LSR = 0x9,
    OP_MOVR = 0xA,
    OP_MOVM = 0xB

} OpCode;

void EX_execute(ID_EX_Buffer *self, EX_MEM_Buffer *out, uint32_t *omar_yahia_var_inbtw_mres, gpr *pc);

extern int prevAluResult;
extern bool prevAluResultValid;

#endif // EXECUTE_TEMPLATE