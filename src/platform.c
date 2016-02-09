#include "platform.h"
#include "lcd.h"
#include "state.h"

void any_to_platform(){
    lcd_send_line(LINE1, "Platform Mode");
}

void platform_loop(){
}
