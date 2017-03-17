#pragma once

#include "all.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void Error(char *src, int line, int start, int end, char *msg, ...);