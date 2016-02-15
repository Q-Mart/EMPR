#include "platform.h"
#include "lcd.h"
#include "state.h"
#include "stepper.h"

void any_to_platform(){
    lcd_send_line(LINE1, "Platform Mode");
    stepper_init();
}

void platform_loop(){
    int i;
    for(i=0;i<16;i++){
        stepper_send_nibble(i);
        timer_delay(500);
    }
}
