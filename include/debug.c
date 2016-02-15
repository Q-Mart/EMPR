#include <stdarg.h>
#include <string.h>
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include <stdio.h>

void debug_init(void)
{
    UART_CFG_Type cfg;
    // init cfg with default values.
    UART_ConfigStructInit(&cfg);

    PINSEL_CFG_Type pinCfg;
    pinCfg.Funcnum = PINSEL_FUNC_1;
    pinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
    pinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    pinCfg.Portnum = PINSEL_PORT_0;
    pinCfg.Pinnum = PINSEL_PIN_2;
    PINSEL_ConfigPin(&pinCfg);
    pinCfg.Pinnum = PINSEL_PIN_3;
    PINSEL_ConfigPin(&pinCfg);

    UART_FIFO_CFG_Type fifoCfg;
    UART_FIFOConfigStructInit(&fifoCfg);

    UART_Init(LPC_UART0, &cfg);
    UART_FIFOConfig(LPC_UART0, &fifoCfg);

    UART_TxCmd(LPC_UART0, ENABLE);
}

int debug_send(char* s)
{
    return UART_Send(LPC_UART0, s, strlen(s), BLOCKING);
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
