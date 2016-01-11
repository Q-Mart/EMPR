#include "keypad.h"
#include "i2c.h"
#include "lpc17xx_gpio.h"
#include "pinsel.h"

/* init mbed i2c 
 * todo: don't make these overlap*/
void keypad_init(void)
{
    i2c_enable_mbed(LPC_I2C1);
}

void keypad_enable_int(void)
{
    // ensure GPIO functionality is set on that pin.
    pinsel_enable_pin(PINSEL_PORT_0, PINSEL_PIN_23, PINSEL_FUNC_0);

    /* enable interrupts by sending 1's to quasi-bidirectional pins */
    char clear = 0x0f;
    i2c_send_mbed_polling(LPC_I2C1, KEYPAD_ADDR, 1, &clear);

    // Enable GPIO interrupts on P0.23
    // on falling edge
    GPIO_IntCmd(0, 1 << 23, 1);

    // Enable the EINT3 Handler
    NVIC_EnableIRQ(EINT3_IRQn);
}

void keypad_clear_int(void)
{
    GPIO_ClearInt(0, 1 << 23);
}

/* given a 1x16 array, it places 1 in the positions where
 * the key is pressed
 * where r[row*4 + col] = bool */
void get_keyboard_presses(char * r) 
{
    char cols[] = {COLUMN1, COLUMN2, COLUMN3, COLUMN4};

    /* zero the array */
    int i;
    for (i = 0; i < 16; ++i) {
        r[i] = 0; 
    }

    int col;
    for (col = 0; col < 4; ++col) {
        char j = poll_keyboard(cols[col]);

        if (j)
        {
            int row;
            for (row = 0; row < 4; ++row) {
                if (((j >> row) & 0x01) == 1)
                    r[(3 - row)*4 + col] = 1;
            }
        }
    }
}

char poll_keyboard(char column)
{
    char data = column;
    i2c_send_mbed_polling(LPC_I2C1, KEYPAD_ADDR, 1, &data);

    data = 0;
    i2c_recv_mbed_polling(LPC_I2C1, KEYPAD_ADDR, 1, &data);

    
    char clear = 0x0f;
    i2c_send_mbed_polling(LPC_I2C1, KEYPAD_ADDR, 1, &clear);

    data = ~data;
    data = data & 0x0f;
    return data;
}
