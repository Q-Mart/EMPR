#include "scan.h"
#include "servo.h"
#include "lcd.h"
#include "debug.h"
#include "ir_sensor.h"
#include "ultrasound.h"
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
    servo_set_pos(pos + (scan_direction));
    timer_delay(35);
    uint32_t raw = ir_sensor_get_raw_data();
    uint32_t raw_us = ultrasound_get_distance();
    raw = (raw + raw_us) /2;
    debug_send_arb((char*) &pos, 4);
    debug_send_arb((char*) &raw, 4);
    lcd_send_line(LINE2, "%d", ir_convert_to_distance(raw));
}
