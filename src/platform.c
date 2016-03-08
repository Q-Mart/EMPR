#include "platform.h"
#include "lcd.h"
#include "utils.h"
#include "timer.h"
#include "keypad.h"
#include "state.h"
#include "stepper.h"
#include "network.h"

static uint16_t number_of_sweeps = 50;
static uint16_t current_sweep;

void any_to_platform() {
    lcd_send_line(LINE1, "Platform mode");
    lcd_send_line(LINE2, "# to start");
    network_send(PLATFORM, NULL);
}

void platform_loop(){}

void platform_to_platform_sweep() {
    lcd_send_line(LINE1, "Sweeping");
}

void platform_scan_loop() {
    lcd_send_line(LINE1, "Sweep %d of %d", current_sweep+1, number_of_sweeps);
    uint32_t raw = utils_get_ir_and_ultrasound_median_distance();
    lcd_send_line(LINE2, "%d", raw);
    //***DATA SHOULD BE SENT HERE***
    network_send(PLATFORM_SCAN, &current_sweep, 4, &raw, 4, NULL);

    stepper_step_clockwise();
    current_sweep++;

    if (current_sweep == number_of_sweeps) {
        change_state(PLATFORM_DONE);
        network_send(PLATFORM_DONE, NULL);
    }
}

void platform_done_loop() {
    current_sweep = 0;
    lcd_send_line(LINE1, "Finished!");
    lcd_send_line(LINE2, "Please press #");
}
