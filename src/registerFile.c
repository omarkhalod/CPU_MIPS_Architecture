#include <stdio.h>
#include <stdlib.h>
#include "registerFile.h"

gpr* createRegisters(void){
    gpr* regs = malloc(sizeof(gpr) * 32);
    if (!regs) return NULL;

    for (int i = 0; i < 32; i++) {
        regs[i].data = 0;
    }
    
    return regs;
}

gpr* createPC(void){
    gpr* pc =  malloc(sizeof(gpr));
    if(!pc) return NULL;
    
    pc->data = 0;
    return pc;
}