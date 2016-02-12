#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "debug.h"

void debug_init(void) {}

int debug_send(char* s)
{
    printf(s);
    return 1;
}

int debug_send_arb(char* s, int len){
    char cpy[strlen(s)];
    strncpy(cpy, s, len);
    printf(cpy);
    return 1;
}

int debug_sendfc(char* fmt, char arg)
{
    printf(fmt, arg);
    return 1;
}

/*
 * send formatted string
 *  analogous to printf/sprintf
 */
void debug_sendf(char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}
