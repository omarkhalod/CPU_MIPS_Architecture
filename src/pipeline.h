#include <stdint.h>
#include <stdbool.h>
typedef struct
{
    uint32_t instruction;
    int pc;
} IF_ID_Register;

typedef struct
{
    uint8_t dest_reg;

    // R - Format
    uint8_t op;
    uint16_t SHAMT;
    uint8_t regA;
    uint8_t regB;

    // I Format imm 18bits
    int32_t immediate;

    // J- Format
    uint32_t address;

    // handeling data hazerd
    bool regA_updated;
    bool regB_updated;
} ID_EX_Register;

typedef struct
{
    int alu_result;
    int dest_reg;
    int mem_read, mem_write;
} EX_MEM_Register;

typedef struct
{
    int data;
    int rd;
    int reg_write;
} MEM_WB_Register;