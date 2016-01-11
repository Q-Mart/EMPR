#include "keypad.h"
#include "lcd.h"
#include "lpc17xx_gpio.h"

int main(void)
{
    lcd_init();
    lcd_clear_display();

    keypad_init();
    keypad_enable_int();

    while (1);
}

void EINT3_IRQHandler(void) 
{
    const int state = 0;

    if (GPIO_GetIntStatus(0, 23, 1))
    { 
        keypad_clear_int();
        
        if (state == 0) {
            lcd_send_str(0x00, "Interrupt!");
            state = 1;
        } else {
            lcd_send_str(0x00, "Reset!");
            state = 1;
        }
    }
}
