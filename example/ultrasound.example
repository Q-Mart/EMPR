#include <stdio.h>
#include "lpc17xx_timer.h"
#include "keypad.h"
#include "lcd.h"
#include "debug.h"
#include "gpio.h"
#include "timer.h"
#include "ultrasound.h"

/* Instructions to use ultrasound module:
    Connect Echo pin of the sensor to P29 on IO Board;
    Connect Trigger pin of the sensor to P8 on IO Board;
    Call initialise_timer_measurement() at start;
    Now calibrate the sensor, calculated the speed of sound
     as it appears with the calibration object, in m/s, save
     in calibrated_gradient;
    Call send_test_pulse() to send a pulse, then immediately:
    call process_ultrasound_value(calibrated_gradient,\
        (int)ultrasound_valid_response_time) 
     to obtain object distance in cm.
*/


int main(void)
{
    debug_init();

    lcd_init();
    lcd_clear_display();

    //keypad_init();
    //keypad_enable_int();
    initialise_timer_measurement();

    //Not calibrated in example.
    double measured_distance = 0;
    char print_distance[64];
    while (1) {
        send_test_pulse();
        measured_distance = process_ultrasound_value(340, (int)ultrasound_valid_response_time);
        sprintf(print_distance, "Distance: %0.7f cm\r\n\r\n", measured_distance);
        debug_send(print_distance);
    }
}


void EINT3_IRQHandler(void)
{

    debug_send("EINT 3\r\n");
    static int state = 0;
    keypad_clear_int();

    /*
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
    }*/
}




