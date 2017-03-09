#pragma once

#include "all.h"

typedef struct {
    LLVMModuleRef module,
    LLVMBuilderRef builder,
} Irgen;