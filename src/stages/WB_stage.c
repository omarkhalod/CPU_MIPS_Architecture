#include <stdio.h>
#include "pipeline.h"
#include "registerFile.h"


void WB_execute(MEM_WB_Buffer *in, gpr *registers){
    if(in->reg_write && in->dest_reg != 0){
        registers[in->dest_reg].data = in->data;
        printf("WB: R%d updated to %d\n", in->dest_reg, in->data);
    }else{
        printf("No write back needed for this instruction.\n");
    }
}