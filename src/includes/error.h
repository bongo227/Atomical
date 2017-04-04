#pragma once

#include "all.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void error(char *src, int line, int start, int end, char *msg, ...);