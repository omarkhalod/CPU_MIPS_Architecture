#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

memory* createMemory(){
    memory* res = malloc(sizeof(memory) * 2048);
    if (!res) return NULL;

    for (int i = 0; i < 2048; i++) {
        res[i].data = 0;
    }
    
    return res;
}