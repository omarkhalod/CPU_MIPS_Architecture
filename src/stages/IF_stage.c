#include <stdio.h>
#include <string.h>
#include "../memory.h"
#include "../registerFile.h"
#include "../pipeline.h"
//need to read from txt file the operation by pc
//place the instruction in the IF_ID buffer?

void IF_stage(memory* mem, gpr* pc, IF_ID_Buffer* out) {
  out->instruction = mem[pc->data].data;
  pc->data++;
}