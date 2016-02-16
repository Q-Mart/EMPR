#include "platform.h"
#include "lcd.h"
#include "state.h"
#include "stepper.h"

void any_to_platform(){
    lcd_send_line(LINE1, "Platform Mode");
    stepper_init();
}

void platform_loop(){
    stepper_send_nibble(6);
    timer_delay(500);
    stepper_send_nibble(3);
    timer_delay(500);
    stepper_send_nibble(9);
    timer_delay(500);
    stepper_send_nibble(12);
    timer_delay(500);
}
