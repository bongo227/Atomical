#pragma once

#include <assert.h>

#define ASSERT(cond, msg) assert(cond && msg)
#define bool int
#define true 1
#define false 0
#define NULL (void *)0