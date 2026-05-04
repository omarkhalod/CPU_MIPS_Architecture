#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

int programSize = 0;

memory* createMemory(){
    memory* res = malloc(sizeof(memory) * 2048);
    if (!res) return NULL;

    for (int i = 0; i < 2048; i++) {
        res[i].data = 0;
        res[i].isValid = false; // this will be set to true when reading from disk to memory
    }
    
    return res;
}