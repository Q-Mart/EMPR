#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"

#include "debug.h"
#include "network.h"
#include "pinsel.h"

void debug_init(void)
{
    UART_CFG_Type cfg;
    // init cfg with default values.
    UART_ConfigStructInit(&cfg);

    pinsel_enable_pin(PINSEL_PORT_0, PINSEL_PIN_2, PINSEL_FUNC_1);
    pinsel_enable_pin(PINSEL_PORT_0, PINSEL_PIN_3, PINSEL_FUNC_1);

    UART_FIFO_CFG_Type fifoCfg;
    UART_FIFOConfigStructInit(&fifoCfg);

    UART_Init(LPC_UART0, &cfg);
    UART_FIFOConfig(LPC_UART0, &fifoCfg);

    UART_TxCmd(LPC_UART0, ENABLE);
}

int debug_send(char* s)
{
#ifndef RECORD
    return UART_Send(LPC_UART0, s, strlen(s), BLOCKING);
#endif
}

int debug_send_arb(char* s, int len){
    return UART_Send(LPC_UART0, s, len, BLOCKING);
}

int debug_sendfc(char* fmt, char arg)
{
    char buf[strlen(fmt)];
    sprintf(buf, fmt, arg);
    debug_send(buf);
}

/*
 * send formatted string
 *  analogous to printf/sprintf
 */
void debug_sendf(char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

   char buf[strlen(fmt)];
   vsprintf(buf, fmt, ap);
   debug_send(buf);
   va_end(ap);
}
