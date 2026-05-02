#ifndef MEM_STAGE_H
#define MEM_STAGE_H

#include "memory.h"
#include "pipeline.h"

void MEM_execute(EX_MEM_Buffer *in, MEM_WB_Buffer *out, memory *mem);
        
#endif 