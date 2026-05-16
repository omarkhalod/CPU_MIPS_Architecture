#ifndef IF_STAGE_H
#define IF_STAGE_H

#include "../memory.h"
#include "../registerFile.h"
#include "../pipeline.h"

void IF_stage(memory* mem, gpr* pc, IF_ID_Buffer* out);

#endif
