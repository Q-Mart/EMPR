#include "lpc17xx_i2c.h"

/* enable I2C on mbed */
void i2c_enable_mbed(LPC_I2C_TypeDef*);
int i2c_send_mbed_polling(LPC_I2C_TypeDef*, int, int, char*);
int i2c_recv_mbed_polling(LPC_I2C_TypeDef*, int, int, char*);
int i2c_send_recv_mbed_polling(LPC_I2C_TypeDef*, int, int, char*, int, char*);
