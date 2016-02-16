#include <stdlib.h>
#include "measure.h"
#include "lcd.h"
#include "ir_sensor.h"
#include "debug.h"
#include "state.h"
#include "network.h"

void any_to_measure() {
    lcd_send_line(LINE1, "Measure Mode");
}

void measure_loop() {
    uint32_t raw_ir = ir_sensor_get_raw_data();
    ultrasound_send_test_pulse();
    uint32_t dist_ir = ir_convert_to_distance(raw_ir);
    uint32_t dist_us = ultrasound_get_distance(); 
    uint32_t dist = (dist_ir + dist_us)/2;
    network_send(MEASURE_DO, (uint8_t *)&dist, 4, NULL);
    lcd_send_line(LINE2, "%d", dist);
}
