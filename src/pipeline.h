#ifndef PIPELINE_H
#define PIPELINE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint32_t instruction;
    int pc;
} IF_ID_Buffer;

typedef struct
{
    uint8_t dest_reg;

    // R - Format
    uint8_t op;
    uint16_t SHAMT;
    uint32_t regA_value;
    uint32_t regB_value;

    // I Format imm 18bits
    int32_t immediate;

    // J- Format
    uint32_t address;

    // handeling data hazards
    bool regA_updated;
    bool regB_updated;
} ID_EX_Buffer;

typedef struct
{
    int alu_result;
    int dest_reg;
    int mem_read, mem_write;
} EX_MEM_Buffer;

typedef struct
{
    int data;
    int dest_reg;
} MEM_WB_Buffer;

#endif