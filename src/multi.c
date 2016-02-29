#include "multi.h"
#include "lcd.h"
#include "utils.h"
#include "timer.h"
#include "keypad.h"
#include "state.h"
#include "servo.h"

static uint16_t max_angle = 270;
static uint16_t min_angle = 0;
static uint16_t number_of_sweeps = 4;
static uint16_t current_sweep;
static int scan_direction;

void any_to_multi() {
    lcd_send_line(LINE1, "Multi-View mode");
    lcd_send_line(LINE2, "Press # for settings");
}

void multi_to_multi_sweep_number() {
    lcd_send_line(LINE1, "Number of sweeps");
}

void multi_sweep_number_to_multi_min_angle() {
    lcd_send_line(LINE1, "Minimum angle");
}

void multi_min_angle_to_multi_max_angle() {
    lcd_send_line(LINE1, "Maximum angle");
}

void multi_sweep() {
    //Clamp the angles if they are out of range
    if (max_angle>270) max_angle = 270;
    if (min_angle>270) min_angle = 0;
    servo_set_pos(min_angle);
    //Wait to move
    timer_delay(500);
}

void multi_sweep_number_loop(int last_key_press) {
    utils_process_digit_input(last_key_press, &number_of_sweeps);
    lcd_send_line(LINE2, "%d", number_of_sweeps);
}

void multi_min_angle_loop(int last_key_press) {
    utils_process_digit_input(last_key_press, &min_angle);
    lcd_send_line(LINE2, "%d", min_angle);
}

void multi_max_angle_loop(int last_key_press) {
    utils_process_digit_input(last_key_press, &max_angle);
    lcd_send_line(LINE2, "%d", max_angle);
}

void multi_sweep_loop() {
    int i;
    int angle_range = max_angle-min_angle;
    int pos = servo_get_pos();

    lcd_send_line(LINE1, "Sweep %d of %d", current_sweep+1, number_of_sweeps);

    if (pos<=min_angle) scan_direction = 1;
    if (pos>=max_angle) scan_direction = -1;

    for(i=0;i<angle_range;i++) {
        pos = pos + scan_direction;
        servo_set_pos(pos);
        timer_delay(35);
        uint32_t raw = utils_get_ir_and_ultrasound_distance();
        //***DATA SHOULD BE SENT HERE***
    }

    current_sweep++;

    if (current_sweep < number_of_sweeps) {
        change_state(MULTI_WAIT);
    } else {
        change_state(MULTI_DONE);
    }
}

void multi_wait_loop() {
    lcd_send_line(LINE1, "Please rotate");
    lcd_send_line(LINE2, "Then press #");
}

void multi_done_loop() {
    servo_set_pos(160);
    current_sweep = 0;
    lcd_send_line(LINE1, "Finished!");
    lcd_send_line(LINE2, "Please press #");
}
