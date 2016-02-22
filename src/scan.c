#include "scan.h"
#include "servo.h"
#include "lcd.h"
#include "debug.h"
#include "ir_sensor.h"
#include "ultrasound.h"
#include "timer.h"
#include "utils.h"
signed int scan_direction = 1;
static uint16_t scan_upper_bound = 270;
static uint16_t scan_lower_bound = 0;
static uint16_t scan_speed = 1;
static uint16_t scan_tentative_speed = 1;
static uint16_t scan_tentative_upper_bound = 270;
static uint16_t scan_tentative_lower_bound = 0;
/*
 * Parameter Loops:
 * Each parameter state has a loop function that runs during the
 * main event loop. This allows us to handle input such that user can
 * see what they inputting as the setting.
 * Architecturely there is a nicer way to do this. Instead of having
 * to put the functions in a switch in the loop. We could have something
 * similar to the state transition table, where we just add loop function
 * pointers to a table and get a function in the main loop to multiplex to
 * one based on the current state.
 */
void any_to_scan(){
    lcd_send_line(LINE1, "Scan, # to start");
    lcd_send_line(LINE2, "* for options");
    servo_set_pos(160);
}
void scan_to_scan_do(){
    lcd_send_line(LINE1, "Scanning...");
}
void scan_parameters_to_1(){
    scan_tentative_speed = scan_speed;
    lcd_send_line(LINE1, "Speed %d", scan_tentative_speed);
    lcd_send_line(LINE2, "# to Confirm");
    timer_delay(300); //Prevent button bounce;
}
void scan_parameter_1_loop(int last_key_press){
    utils_process_digit_input(last_key_press, &scan_tentative_speed);
    lcd_send_line(LINE1, "Speed %d", scan_tentative_speed); 
}
void scan_parameter_1_to_scan_parameters(){
    scan_speed = scan_tentative_speed;
    any_to_scan_parameters();
}
void any_to_scan_parameters(void){
    lcd_send_line(LINE1, "Scan Parameters");
}
void scan_parameters_to_2(void){
    scan_tentative_upper_bound = scan_upper_bound;
    lcd_send_line(LINE1, "Left point %d", scan_tentative_upper_bound);
    lcd_send_line(LINE2, "# to confirm");
    timer_delay(300); //Prevent Button Bounce
}
void scan_parameter_2_loop(int last_key_press){
    utils_process_digit_input(last_key_press, &scan_tentative_upper_bound);
    lcd_send_line(LINE1, "Left point %d", scan_tentative_upper_bound);
}

void scan_parameter_2_to_scan_parameters(void){
    scan_upper_bound = scan_tentative_upper_bound;
    any_to_scan_parameters();
}
void scan_parameters_to_3(void){
    scan_tentative_lower_bound = scan_lower_bound;
    lcd_send_line(LINE1, "Right point %d", scan_tentative_lower_bound);
    lcd_send_line(LINE2, "# to confirm");
    timer_delay(300); //Prevent Button Bounce
}
void scan_parameter_3_loop(int last_key_press){
    utils_process_digit_input(last_key_press, &scan_tentative_lower_bound);
    lcd_send_line(LINE1, "Right Point %d", scan_tentative_lower_bound);
}
void scan_parameter_3_to_scan_parameters(void){
    scan_lower_bound = scan_tentative_lower_bound;
    any_to_scan_parameters();
}
void scan_loop(){
    int pos = servo_get_pos();
    if(pos <= scan_lower_bound) scan_direction = 1;
    if(pos >= scan_upper_bound) scan_direction = -1;
    servo_set_pos(pos + (scan_direction * scan_speed));
    timer_delay(35);//Time for it to phusically move
    uint32_t raw = ir_sensor_get_raw_data();
    raw = ir_convert_to_distance(raw);
    uint32_t raw_us = ultrasound_get_distance();
    raw = (raw + raw_us) /2;
    debug_send_arb((char*) &pos, 4);
    debug_send_arb((char*) &raw, 4);
    lcd_send_line(LINE2, "%d", raw);
}
