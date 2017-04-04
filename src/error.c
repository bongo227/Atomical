#include "includes/error.h"

void verror(char *src, int line, int start, int end, char *msg, va_list args) {
    ASSERT(start >= 0, "start underflows the line");
    
    // print message
    printf("\e[31m\e[1mERROR:\e[0m "); 
    vprintf(msg, args);
    printf("\n\n");

    // get the position of line withing the source code
    char *src_ptr = src;
    int line_end = 0;
    int line_start = 0;
    int current_line = 0;
    while(true) {
        if(*src_ptr == '\n') {
            line_end = src_ptr - src;
            if(current_line != line - 1) {
                line_start = src_ptr - src;
            } else {
                break;
            }
            current_line++;
        }
        src_ptr++;
    }
    int line_length = line_end - line_start;
    
    // print line number
    printf("\e[2m%5d|\e[0m ", line);
    
    // print source code
    char *src_line = malloc(line_length);
    memcpy(src_line, src + line_start, line_length * sizeof(char));
    if(src_line[0] == '\n') {
        // remove leading newline
        src_line++;
        line_length--;
    }
    while(src_line[0] == '\t') {
        // print tabs as 4 spaces (since we dont start on a column boundry)
        printf("    ");
        src_line++;
        line_length += 4;
    }
    printf("%s\n", src_line);

    // print error underlining
    char *buff = malloc(line_length * sizeof(char));
    for (int i = 0; i < line_length; i++) {
        if (i >= start - 1 && i < end) {
            buff[i] = '^';
        } else {
            buff[i] = ' ';
        }
    }
    buff[line_length-1] = '\0';
    printf("       \e[91m\e[1m%s\e[0m\n\n", buff);
}

void error(char *src, int line, int start, int end, char *msg, ...) {
    va_list args;
    va_start(args, msg);
    verror(src, line, start, end, msg, args);
    va_end(args);
}