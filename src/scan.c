#include "scan.h"
#include "servo.h"
#include "lcd.h"
#include "debug.h"
#include "ir_sensor.h"
#include "ultrasound.h"
signed int scan_direction = 1;
static uint16_t scan_upper_bound = 270;
static uint16_t scan_lower_bound = 0;
static uint16_t scan_speed = 1;
static int scan_tentative_speed = 0;

void any_to_scan(){
    lcd_send_line(LINE1, "Scan Mode");
    lcd_send_line(LINE2, "Press * for options");
    servo_set_pos(0);
}
void scan_to_scan_do(){
    lcd_send_linel(LINE1, "Scanning...");
}
void scan_parameters_to_1(){
    lcd_send_line(LINE1, "Enter speed: %d", scan_speed);
    lcd_send_line(LINE2, "# : Confirm");
}
void scan_parameters_1_loop(int last_key_press){
    if (last_key_press >= 0){
        scan_tentative_speed = (scan_tentative_speed * 10) + last_key_press;
    } else if (last_key_press == -1){
        //remove least significant digit
        //somewhat hacky, works because C truncates towards 0.
        scan_tentative_speed /= 10;
    }

    lcd_send_line(LINE1, "Enter speed: %d", scan_tentative_speed); 
}
void scan_parameters_1_to_scan_parameters(){
    scan_speed = scan_tentative_speed;
}
void scan_loop(){
    //Max 270
    //Min 0
    //Middle 160
    int pos = servo_get_pos();
    if(pos <= scan_lower_bound) scan_direction = 1;
    if(pos >= scan_upper_bound) scan_direction = -1;
    servo_set_pos(pos + (scan_direction * scan_speed));
    timer_delay(35);
    uint32_t raw = ir_sensor_get_raw_data();
    uint32_t raw_us = ultrasound_get_distance();
    raw = (raw + raw_us) /2;
    debug_send_arb((char*) &pos, 4);
    debug_send_arb((char*) &raw, 4);
    lcd_send_line(LINE2, "%d", ir_convert_to_distance(raw));
}
