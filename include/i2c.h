/* enable I2C on mbed */
#define I2C_DEVICE_1 1
#define I2C_DEVICE_2 2

void i2c_enable_mbed(int);
int i2c_send_mbed_polling(int, int, int, char*);
int i2c_recv_mbed_polling(int, int, int, char*);
int i2c_send_recv_mbed_polling(int, int, int, char*, int, char*);
