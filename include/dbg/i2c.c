#include <stdlib.h>
#include <stdio.h>

#include "i2c.h"
#include "dbg.h"

void i2c_enable_mbed(int lpc) {}
int i2c_send_mbed_polling(int lpc, int addr, int len, char* p_data) {}
int i2c_recv_mbed_polling(int lpc, int addr, int len, char* buf)
{
    return 1;
}

int i2c_send_recv_mbed_polling(int lpc, 
        int addr, 
        int tx_len, 
        char* tx_buf,
        int rx_len,
        char* rx_buf)
{
    return 1;
}

