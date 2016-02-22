#include "lpc17xx_gpio.h"
#include "gpio.h"

void init_general_gpio(int port, int pin, int direction)
{
    GPIO_SetDir(port, pin, direction);
}

void set_general_gpio(int port, int pin, int value) 
{
    if (value == 1)
    {   
        GPIO_SetValue(port, pin);
    } else {
        GPIO_ClearValue(port, pin);
    }
}
