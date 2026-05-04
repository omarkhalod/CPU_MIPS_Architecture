#include <stdint.h>
#include <stdbool.h>
#ifndef MEMORY_H
#define MEMORY_H

typedef struct{
    uint32_t data;
    bool isValid;
} memory;

memory* createMemory();

#endif