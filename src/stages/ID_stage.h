#ifndef ID_STAGE_H
#define ID_STAGE_H

#include "pipeline.h"
#include "registerFile.h"

void ID_execute(IF_ID_Buffer* in, ID_EX_Buffer* out, gpr* R);
void dequeueInUse();
void enqueueInUse(uint32_t R);
bool inUseContains(uint32_t R);

#endif