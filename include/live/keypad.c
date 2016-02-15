#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

#include "keypad.h"
#include "i2c.h"
#include "pinsel.h"
#include "network.h"

#define DEVICE I2C_DEVICE_1

#define EDGE 1
static const char CLEAR = 0xf0;

/* init mbed i2c
 * todo: don't make these overlap*/
void keypad_init(void)
{
    i2c_enable_mbed(DEVICE);
}

void keypad_enable_int(void)
{
    // ensure GPIO functionality is set on that pin.
    pinsel_enable_pin(PINSEL_PORT_0, PINSEL_PIN_23, PINSEL_FUNC_0);

    /* enable interrupts by sending 1's to quasi-bidirectional pins */
    i2c_send_mbed_polling(DEVICE, KEYPAD_ADDR, 1, &CLEAR);

    // Enable GPIO interrupts on P0.23
    // on falling edge
    GPIO_IntCmd(0, 1 << 23, EDGE);

    // Enable the EINT3 Handler
    NVIC_EnableIRQ(EINT3_IRQn);
}

void keypad_clear_int(void)
{
    GPIO_ClearInt(0, 1 << 23);
}

int keypad_get_int(void) {
    return GPIO_GetIntStatus(0, 23, EDGE);
}

/* given a 1x16 array, it places 1 in the positions where
 * the key is pressed
 * where r[row*4 + col] = bool */
void get_keyboard_presses(char * r)
{
    char cols[] = {COLUMN1, COLUMN2, COLUMN3, COLUMN4};

    int col;
    for (col = 0; col < 4; ++col) {
        char j = poll_keyboard(cols[col]);

        if (j)
        {
            int row;
            for (row = 0; row < 4; ++row) {
                if (((j >> row) & 0x01) == 1)
                    r[(3 - row)*4 + col] = 1;
                else
                    r[(3 - row)*4 + col] = 0;
            }
        }
    }

#ifdef RECORD
	network_send(KEYPAD_HEADER, r, 16, NULL);
#endif
}

char poll_keyboard(char column)
{
    char data = column;
    i2c_send_mbed_polling(DEVICE, KEYPAD_ADDR, 1, &data);

    data = 0;
    i2c_recv_mbed_polling(DEVICE, KEYPAD_ADDR, 1, &data);
    i2c_send_mbed_polling(DEVICE, KEYPAD_ADDR, 1, &CLEAR);

    data = ~data;
    data = data & 0x0f;
    return data;
}

/* 00001111 */
