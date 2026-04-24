#ifndef ID_STAGE_H
#define ID_STAGE_H

#include "pipeline.h"
#include "registerFile.h"

void ID_stage_decode(IF_ID_Buffer* in, ID_EX_Buffer* out, gpr* R);

#endif