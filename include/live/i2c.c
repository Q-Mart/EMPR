#include <stdlib.h>
#include <stdio.h>

#include "lpc17xx_i2c.h"
#include "lpc_types.h"

#include "i2c.h"
#include "pinsel.h"

LPC_I2C_TypeDef* get_i2c_ptr(int lpc) {
    if (lpc == I2C_DEVICE_1)
        return LPC_I2C1;
    return LPC_I2C2;
}

void i2c_enable_mbed(int lpc)
{
    // set lpc pin to output in func3
    if (lpc == I2C_DEVICE_1)
    {
        pinsel_enable_pin(PINSEL_PORT_0, PINSEL_PIN_0, PINSEL_FUNC_3);
        pinsel_enable_pin(PINSEL_PORT_0, PINSEL_PIN_1, PINSEL_FUNC_3);
    } else if (lpc == I2C_DEVICE_2) {
        pinsel_enable_pin(PINSEL_PORT_0, PINSEL_PIN_10, PINSEL_FUNC_2);
        pinsel_enable_pin(PINSEL_PORT_0, PINSEL_PIN_11, PINSEL_FUNC_2);
    }

    /* init i2c with clock and enable i2c bus */
    I2C_Init(get_i2c_ptr(lpc), 10e3);
    I2C_Cmd(get_i2c_ptr(lpc), ENABLE);
}

int i2c_send_mbed_polling(int lpc, int addr, int len, char* p_data)
{
    I2C_M_SETUP_Type cfg;
    cfg.tx_data = p_data;
    cfg.tx_length = len;
    cfg.rx_data = NULL;
    cfg.rx_length = 0;
    cfg.retransmissions_max = 3;
    cfg.sl_addr7bit = addr;

    Status s = I2C_MasterTransferData(get_i2c_ptr(lpc), &cfg, I2C_TRANSFER_POLLING);

    if (s == SUCCESS)
        return 1;

    return 0;
}

int i2c_recv_mbed_polling(int lpc, int addr, int len, char* buf)
{
    I2C_M_SETUP_Type cfg;
    cfg.tx_data = NULL;
    cfg.tx_length = 0;
    cfg.rx_data = buf;
    cfg.rx_length = len;
    cfg.retransmissions_max = 3;
    cfg.sl_addr7bit = addr;

    Status s = I2C_MasterTransferData(get_i2c_ptr(lpc), &cfg, I2C_TRANSFER_POLLING);

    if (s == SUCCESS)
        return 1;

    return 0;
}

int i2c_send_recv_mbed_polling(int lpc, 
        int addr, 
        int tx_len, 
        char* tx_buf,
        int rx_len,
        char* rx_buf)
{
    I2C_M_SETUP_Type cfg;
    cfg.tx_data = tx_buf;
    cfg.tx_length = tx_len;
    cfg.rx_data = rx_buf;
    cfg.rx_length = rx_len;
    cfg.retransmissions_max = 3;
    cfg.sl_addr7bit = addr;

    Status s = I2C_MasterTransferData(get_i2c_ptr(lpc), &cfg, I2C_TRANSFER_POLLING);

    if (s == SUCCESS)
        return 1;

    return 0;
}

