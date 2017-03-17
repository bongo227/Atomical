#include "includes/error.h"

void verror(char *src, int line, int start, int end, char *msg, va_list args) {
    ASSERT(strlen(src) >= end, "end overflows the line");
    ASSERT(start >= 0, "start underflows the line");
    
    printf("\e[31mERROR:\e[0m "); // "ERROR: " 
    vprintf(msg, args); // error message
    printf("\n\n");

    printf("\e[2m%5d|\e[0m %s\n", line, src); // source code
    int srcLen = strlen(src);
    char buff[10];
    for (int i = 0; i < 10; i++) {
        if (i >= start - 1 && i < end) {
            buff[i] = '~';
        } else {
            buff[i] = ' ';
        }
    }
    // buff[srcLen] = '\0';
    buff[9] = '\0';

    printf("\e[2m%5d|\e[0m ", line + 1);
    printf("\e[91m%s\e[0m\n\n", buff);
}

void Error(char *src, int line, int start, int end, char *msg, ...) {
    va_list args;
    va_start(args, msg);
    verror(src, line, start, end, msg, args);
    va_end(args);
    exit(1);
}