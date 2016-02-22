#include "multi.h"
#include "lcd.h"
#include "utils.h"
#include <stdio.h>
#include "timer.h"
#include "keypad.h"
#include "state.h"

static uint16_t number_of_sweeps;
static uint16_t current_sweep;

void any_to_multi() {
    lcd_send_line(LINE1, "Multi-View mode");
}

void multi_to_multi_settings() {
    lcd_send_line(LINE1, "Multi-settings");
}

void multi_settings_to_multi_sweep() {
    lcd_send_line(LINE1, "Multi-sweep state");
}

void multi_wait_to_multi_sweep() {
}


void multi_settings_loop(int last_key_press) {
    utils_process_digit_input(last_key_press, &number_of_sweeps);
    lcd_send_line(LINE2, "%d", number_of_sweeps);
}

void multi_sweep_loop() {
    lcd_send_line(LINE2, "Sweep %d of %d", current_sweep+1, number_of_sweeps);
    timer_delay(1000);
    current_sweep++;

    if (current_sweep < number_of_sweeps) {
        change_state(MULTI_WAIT);
    } else {
        change_state(MULTI_DONE);
    }
}

void multi_wait_loop() {
    lcd_send_line(LINE1, "Waiting...");
}

void multi_done_loop() {
    lcd_send_line(LINE1, "Finished!");
    lcd_send_line(LINE2, "Please press #");
}
