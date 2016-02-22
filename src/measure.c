#include <stdlib.h>
#include "measure.h"
#include "lcd.h"
#include "ir_sensor.h"
#include "debug.h"
#include "state.h"
#include "network.h"

static uint32_t measure_point = 0; //The desired measure point
static uint8_t measure_alarm_enabled = 0; //0: Alarm off, 1: Alarm on
void any_to_measure() {
    lcd_send_line(LINE1, "Measure Mode # to start");
    lcd_send_line(LINE2, "* for options");
}

void measure_to_measure_do() {
    lcd_send_line(LINE1, "Measuring...");
}

void measure_loop() {
    uint32_t raw_ir = ir_sensor_get_raw_data();
    ultrasound_send_test_pulse();
    uint32_t dist_ir = ir_convert_to_distance(raw_ir);
    uint32_t dist_us = ultrasound_get_distance(); 
    uint32_t dist = (dist_ir + dist_us)/2;
    if(measure_alarm_enabled){
        //Peform update of alarm.

    }
    network_send(MEASURE_DO, (uint8_t *)&dist, 4, NULL);
    lcd_send_line(LINE2, "%d", dist);
}
