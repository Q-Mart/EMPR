#include "scan.h"
#include "servo.h"
#include "lcd.h"
#include "debug.h"
signed int scan_direction = 1;

void any_to_scan(){
    lcd_send_line(LINE1, "Scan Mode");
}
void scan_loop(){
    //Max 270
    //Min 0
    //Middle 160
    int pos = servo_get_pos();
    if(pos <= 0) scan_direction = 1;
    if(pos >= 270) scan_direction = -1;
    debug_sendf("%d : %d\n\r", pos, scan_direction);
    servo_set_pos(pos + (scan_direction));
    timer_delay(30);
}
