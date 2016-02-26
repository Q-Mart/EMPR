#include "platform.h"
#include "lcd.h"
#include "utils.h"
#include "timer.h"
#include "keypad.h"
#include "state.h"
#include "servo.h"

static uint16_t number_of_sweeps = 50;
static uint16_t current_sweep;
static int scan_direction;

void any_to_platform() {
    lcd_send_line(LINE1, "Platform mode");
    lcd_send_line(LINE2, "Press # to start");
    servo_set_pos(160);
}

void platform_loop(){}

void platform_to_platform_sweep() {
    lcd_send_line(LINE1, "Sweeping");
}

void platform_scan_loop() {
    int i;
    lcd_send_line(LINE1, "Sweep %d of %d", current_sweep+1, number_of_sweeps);
    timer_delay(35);
    /* uint32_t raw = utils_get_ir_and_ultrasound_distance(); */
    //***DATA SHOULD BE SENT HERE***

    current_sweep++;

    if (current_sweep == number_of_sweeps) {
        change_state(PLATFORM_DONE);
    }
}

void platform_done_loop() {
    servo_set_pos(160);
    current_sweep = 0;
    lcd_send_line(LINE1, "Finished!");
    lcd_send_line(LINE2, "Please press #");
}
