#include <stdint.h>
#ifndef MEMORY_H
#define MEMORY_H

typedef struct{
    uint32_t data;
    int valid;
} memory;

memory* createMemory();

#endif