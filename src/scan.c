#include "scan.h"
#include "servo.h"
#include "lcd.h"
#include "debug.h"
#include "ir_sensor.h"
#include "ultrasound.h"
#include "timer.h"
#include "network.h"
#include "utils.h"
#include "empr.h"
#define NO_OF_STATES 4
static enum _scan_state{DISTANCE, ANGLE, AVG_DISTANCE, NO_SAMPLES} scan_state = DISTANCE;
signed int scan_direction = 1;
static uint32_t scan_upper_bound = 270;
static uint32_t scan_lower_bound = 0;
static uint32_t scan_speed = 1;
static uint32_t scan_tentative_speed = 1;
static uint32_t scan_tentative_upper_bound = 270;
static uint32_t scan_tentative_lower_bound = 0;
static uint64_t scan_total_distance = 0;
static uint32_t scan_count = 0;
static uint32_t scan_servo_pos = 0;
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
    servo_set_pos(0);
    network_send(SCAN, NULL);
}
void scan_to_scan_do(){
    lcd_send_line(LINE1, "Scanning...");
    servo_set_pos(scan_lower_bound);
    scan_servo_pos = scan_lower_bound;
}
void scan_parameters_to_1(){
    scan_tentative_speed = scan_speed;
    lcd_send_line(LINE1, "Speed %u", scan_tentative_speed);
    lcd_send_line(LINE2, "# to Confirm");
    timer_delay(300); //Prevent button bounce;
}
void scan_parameter_1_loop(int last_key_press){
    utils_process_digit_input(last_key_press, &scan_tentative_speed);
    lcd_send_line(LINE1, "Speed %u", scan_tentative_speed);
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
    lcd_send_line(LINE1, "Left point %u", scan_tentative_upper_bound);
    lcd_send_line(LINE2, "# to confirm");
    timer_delay(300); //Prevent Button Bounce
}
void scan_parameter_2_loop(int last_key_press){
    utils_process_digit_input(last_key_press, &scan_tentative_upper_bound);
    lcd_send_line(LINE1, "Left point %u", scan_tentative_upper_bound);
}

void scan_parameter_2_to_scan_parameters(void){
    scan_upper_bound = scan_tentative_upper_bound;
    any_to_scan_parameters();
}
void scan_parameters_to_3(void){
    scan_tentative_lower_bound = scan_lower_bound;
    lcd_send_line(LINE1, "Right point %u", scan_tentative_lower_bound);
    lcd_send_line(LINE2, "# to confirm");
    timer_delay(300); //Prevent Button Bounce
}
void scan_parameter_3_loop(int last_key_press){
    utils_process_digit_input(last_key_press, &scan_tentative_lower_bound);
    lcd_send_line(LINE1, "Right Point %u", scan_tentative_lower_bound);
}
void scan_parameter_3_to_scan_parameters(void){
    scan_lower_bound = scan_tentative_lower_bound;
    any_to_scan_parameters();
}
void scan_loop(int last_key_press){
    if(last_key_press == 3)
        scan_state = (scan_state + 1) % NO_OF_STATES;
    if(last_key_press == 1)
        scan_state = (scan_state - 1 + NO_OF_STATES) % NO_OF_STATES;
    //int pos = servo_get_pos();
    if(scan_servo_pos <= scan_lower_bound) scan_direction = 1;
    if(scan_servo_pos >= scan_upper_bound) scan_direction = -1;
    scan_servo_pos = pos + (scan_direction * scan_speed);
    servo_set_pos(scan_servo_pos);
    timer_delayc(1, &input_poll);//Time for it to physically move
    uint32_t raw = utils_get_ir_and_ultrasound_median_distance();
    network_send(SCAN_DO, (uint8_t *)&pos, 4, (uint8_t* )&raw, 4, NULL);
    scan_count++;
    scan_total_distance += raw;
    switch(scan_state){
        case DISTANCE:
            lcd_send_line(LINE1, "Distance");
            lcd_send_line(LINE2, "%u", raw);
            break;
        case AVG_DISTANCE:
            lcd_send_line(LINE1, "Average Distance");
            lcd_send_line(LINE2, "%u", scan_total_distance/scan_count);
            break;
        case ANGLE:
            lcd_send_line(LINE1, "ANGLE");
            lcd_send_line(LINE2, "%u", pos);
            break;
        case NO_SAMPLES:
            lcd_send_line(LINE1, "No of Samples");
            lcd_send_line(LINE2, "%u"
                    ,(scan_upper_bound - scan_lower_bound) / scan_speed);
            break;
    }
}
