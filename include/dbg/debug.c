#include "empr.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"

void debug_init(void) {}

int debug_send(char* s)
{
#ifndef DEBUG
    printf(s);
    return 1;
#endif
}

int debug_send_arb(char* s, int len){
#ifndef DEBUG
    char cpy[strlen(s)];
    strncpy(cpy, s, len);
    printf(cpy);
    return 1;
#endif
}

int debug_sendfc(char* fmt, char arg)
{
#ifndef DEBUG
    printf(fmt, arg);
    return 1;
#endif
}

/*
 * send formatted string
 *  analogous to printf/sprintf
 */
void debug_sendf(char* fmt, ...)
{
#ifndef DEBUG
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
#endif
}
