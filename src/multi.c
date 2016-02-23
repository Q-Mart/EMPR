#include "multi.h"
#include "lcd.h"
#include "utils.h"
#include <stdio.h>
#include "timer.h"
#include "keypad.h"
#include "state.h"
#include "servo.h"
#include "debug.h"

static uint16_t number_of_sweeps;
static uint16_t current_sweep;
static int scan_direction;

void any_to_multi() {
    lcd_send_line(LINE1, "Multi-View mode");
    lcd_send_line(LINE2, "Press # for settings");
}

void multi_to_multi_settings() {
    lcd_send_line(LINE1, "Multi-settings");
}

void multi_settings_to_multi_sweep() {
    lcd_send_line(LINE1, "Multi-sweep state");
    servo_set_pos(0);
    //Wait to move
    timer_delay(500);
}

void multi_settings_loop(int last_key_press) {
    utils_process_digit_input(last_key_press, &number_of_sweeps);
    lcd_send_line(LINE2, "%d", number_of_sweeps);
}

void multi_sweep_loop() {
    int i;
    int pos = servo_get_pos();

    lcd_send_line(LINE2, "Sweep %d of %d", current_sweep+1, number_of_sweeps);

    if (pos<=0) scan_direction = 1;
    if (pos>=270) scan_direction = -1;

    for(i=0;i<270;i++) {
        pos = pos + scan_direction;
        servo_set_pos(pos + scan_direction);
        timer_delay(35);
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
    lcd_send_line(LINE1, "Finished!");
    lcd_send_line(LINE2, "Please press #");
}
