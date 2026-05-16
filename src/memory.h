#include <stdint.h>
#include <stdbool.h>
#ifndef MEMORY_H
#define MEMORY_H

typedef struct{
    uint32_t data;
    bool isValid;
} memory;

memory* createMemory(void);

void loadProgramFileToMemory(memory* mem);

bool arrayContainsElement(char* array[], int arrayLength, char* element);

int regNameToNumber(char* reg);

int opcodeStringToInt(char* opcode_str);

int encodeR (int opcode, int reg1, int reg2, int reg3, int shamt);

int encodeI(int opcode, int reg1, int reg2, int imm);

int encodeJ(int opcode, int address);

#endif