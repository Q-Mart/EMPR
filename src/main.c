#include <stdio.h>
#include "lpc17xx_timer.h"
#include "keypad.h"
#include "lcd.h"
#include "debug.h"
#include "gpio.h"
#include "timer.h"
#include "ultrasound.h"

static uint32_t current_timer_diff = 0; //Use this value for current interval.
static uint32_t previous_timer_value = 0;

int main(void)
{
    debug_init();

    lcd_init();
    lcd_clear_display();

    keypad_init();
    keypad_enable_int();

    initialise_timer_measurement();

    init_general_gpio(HCSR_SIGNAL_PORT, HCSR_SIGNAL_PIN, GPIO_OUTPUT);
    set_general_gpio(HCSR_SIGNAL_PORT, HCSR_SIGNAL_PIN, 0);
    int n = 0;
    while (1) {
        debug_sendfc("Send pulse... %d\r\n", n++);
        set_general_gpio(HCSR_SIGNAL_PORT, HCSR_SIGNAL_PIN, 1);
        timer_delay(1);
        set_general_gpio(HCSR_SIGNAL_PORT, HCSR_SIGNAL_PIN, 0);
        timer_delay(1000);
    }
}

/*
void EINT3_IRQHandler(void) 
{
    static int state = 0;

    if (GPIO_GetIntStatus(0, 23, 1))
    { 
        keypad_clear_int();
        
        if (state == 0) {
            lcd_send_str(0x00, "Interrupt!");
            state = 1;
        } else {
            lcd_send_str(0x00, "Reset!");
            state = 0;
        }
    }
}
*/

/*Using General Purpose Timer 2.0 to capture 
rising and falling edges on ultrasound output. */
void TIMER2_IRQHandler(void)
{
    static uint32_t timer_value;
    static char debug_string[80];
   
    if (TIM_GetIntCaptureStatus(LPC_TIM2, 0))
    {
        TIM_ClearIntCapturePending(LPC_TIM2, 0);
        timer_value = TIM_GetCaptureValue(LPC_TIM2, 0);
        
        //overflow?
        if (timer_value < previous_timer_value) {
            debug_send("Error: timer value inconsistent. Resetting timer.");
            current_timer_diff = 0;
            previous_timer_value = 0;
        } else {
            current_timer_diff = timer_value - previous_timer_value;
            previous_timer_value = timer_value;
        }
        sprintf(debug_string, "Timer Value: %lu \r\nTimer duration: %lu\r\n\r\n", (unsigned long)previous_timer_value, (unsigned long)current_timer_diff);
        debug_send(debug_string);
    }
}

