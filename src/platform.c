#include "platform.h"
#include "lcd.h"
#include "state.h"
#include "stepper.h"

void any_to_platform(){
    lcd_send_line(LINE1, "Platform Mode");
    stepper_init();
}

void platform_loop(){
    stepper_full_clockwise(3);
    timer_delay(5000);
    stepper_full_anticlockwise(3);
    timer_delay(5000);
}
