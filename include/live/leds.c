#include "lpc17xx_gpio.h"
#include "leds.h"

void delay1(void)
{
    int i;
    int max = 1 << 23;
    for (i = 0; i < max; ++i); // delay loop
}

void init_gpio(int led)
{
    GPIO_SetDir(LED_PORT, led, 1);
}

void set_gpio(int led, int value) 
{
    if (value == 1)
    {
        GPIO_SetValue(LED_PORT, led);
    } else {
        GPIO_ClearValue(LED_PORT, led);
    }
}
