#include <stdio.h>
#include <stdlib.h>
#include "lpc17xx_timer.h"
#include "keypad.h"
#include "lcd.h"
#include "debug.h"
#include "gpio.h"
#include "timer.h"
#include "ultrasound.h"
#include "servo.h"
#include "ir_sensor.h"
#include "tracker.h"

int tracker_upper_bound = 0;
int tracker_lower_bound = 0;
int tracker_current_center = 0;

void tracker(void)
{
    debug_init();
    
    ultrasound_initialise_timer_measurement();
    servo_init();

    uint32_t tracker_ultrasound_range_table[32], tracker_ir_range_table[32];
    int tracker_range_index;

    tracker_range_index = tracker_full_scan(0, 250, 10, tracker_ultrasound_range_table, tracker_ir_range_table);
    tracker_current_center = tracker_compare_indices(
        tracker_find_smallest_index(
            tracker_ultrasound_range_table, 
            tracker_range_index, 
            2),
        tracker_find_smallest_index(
            tracker_ir_range_table, 
            tracker_range_index,
            2)
        ) * 10;

    servo_set_pos(tracker_current_center);
    tracker_set_bound();

    while (1)
    {
        tracker_narrow_sweep();
        tracker_set_bound();
        timer_delay(125);
    }
}

int tracker_full_scan(int start, int end, int increment, uint32_t * tracker_ultrasound_range_table, uint32_t * tracker_ir_range_table)
{
    int sensor_position;
    int range_index = 0;
    static uint32_t ultrasound_measured_distance;
    static uint32_t ir_measured_distance;
    char print_distance[64];
    debug_send("===Beginning full scan.===\r\n");
    for (sensor_position = start; sensor_position <= end; sensor_position += increment)
    {
        servo_set_pos(sensor_position);
        timer_delay(275);
        ultrasound_send_test_pulse();
        ultrasound_measured_distance = ultrasound_get_distance();
        ir_measured_distance = ir_sensor_get_distance();
        debug_sendf("ultrasound_valid_response_time: %lu \r\n", ultrasound_valid_response_time);
        debug_sendf("ir_sensor_get_raw_data(): %lu \r\n", ir_sensor_get_raw_data());
        tracker_ultrasound_range_table[range_index] = ultrasound_measured_distance;
        tracker_ir_range_table[range_index] = ir_measured_distance;
        range_index++;
    }
    debug_send("===Full scan complete.===\r\n");
    return range_index;
}

/* Narrow sweep within a cone of +-30 degrees, returns the new center */
void tracker_narrow_sweep()
{
    int range_index, new_index, scan_start, scan_end, sensor_position, direction;

    static uint32_t ultrasound_measured_distance, ir_measured_distance;
    static uint32_t tracker_ultrasound_range_table[15];
    static uint32_t tracker_ir_range_table[15];
    static int tracker_index_table[15];

    if (abs(tracker_lower_bound - servo_get_pos()) < abs(tracker_upper_bound - servo_get_pos()))
    {
        scan_start = tracker_lower_bound;
        scan_end = tracker_upper_bound;
    }
    else {
        scan_start = tracker_upper_bound;
        scan_end = tracker_lower_bound;
    }

    debug_sendf("Starting at %d, Finishing at %d \r\n", scan_start, scan_end);
    if (scan_start <= scan_end) {
        range_index = 0;
        for (sensor_position = scan_start; sensor_position <= scan_end; sensor_position += 5) {
            servo_set_pos(sensor_position);
            timer_delay(100);
            ir_measured_distance = ir_sensor_get_distance();
            ultrasound_send_test_pulse();
            timer_delay(50);          
            ultrasound_measured_distance = ultrasound_get_distance();
            tracker_ultrasound_range_table[range_index] = ultrasound_measured_distance;
            tracker_ir_range_table[range_index] = ir_measured_distance;
            tracker_index_table[range_index] = range_index - 6;
            range_index++;
        }
    }
    else {
        range_index = 0;
        for (sensor_position = scan_start; sensor_position >= scan_end; sensor_position -= 5) {
            servo_set_pos(sensor_position);
            timer_delay(100);
            ir_measured_distance = ir_sensor_get_distance();
            ultrasound_send_test_pulse();
            timer_delay(50);
            ultrasound_measured_distance = ultrasound_get_distance();
            tracker_ultrasound_range_table[range_index] = ultrasound_measured_distance;
            tracker_ir_range_table[range_index] = ir_measured_distance;
            tracker_index_table[range_index] = range_index - 6;
            range_index++;
        }
    }
    
    new_index = tracker_compare_indices(
        tracker_find_smallest_index(tracker_ultrasound_range_table, range_index, 0),
        tracker_find_smallest_index(tracker_ir_range_table, range_index, 0));
    tracker_current_center = (tracker_upper_bound + tracker_lower_bound) / 2 + 5 * tracker_index_table[new_index];
    debug_sendf("Tracker: the object is at %d degrees.\r\n", tracker_current_center);
    debug_sendf("Ultrasound: value: %d\r\n", tracker_ultrasound_range_table[new_index]);
    debug_sendf("IR: value: %d\r\n\r\n", tracker_ir_range_table[new_index]);

    int x;
    for (x = 0; x < 15; x++) {
        tracker_ultrasound_range_table[x] = 0;
        tracker_ir_range_table[x] = 0;
    }
}

void tracker_set_bound()
{
    if (tracker_current_center >= 240)
    {
        tracker_upper_bound = 270;
        tracker_lower_bound = 210;
    }
    else if (tracker_current_center <= 30) {
        tracker_lower_bound = 0;
        tracker_upper_bound = 60;
    }
    else {
        tracker_lower_bound = tracker_current_center - 30;
        tracker_upper_bound = tracker_current_center + 30;
    }
}

int tracker_compare_indices(int ultrasound_index, int ir_index)
{
    debug_sendf("U: %d \r\n", ultrasound_index);
    debug_sendf("I: %d \r\n", ir_index);
    if (abs(ultrasound_index - ir_index) >= 3)
    {
        return ir_index;
    }
    else {
        return ((ultrasound_index + ir_index) / 2);
    }
}

int tracker_find_smallest_index(uint32_t darray[], int length, int initial)
{
    /* Optionally ignore first initial number of values, due to motor still moving at the times of measurements. */
    int smallest_index = initial;
    uint32_t smallest = darray[initial];
    int i;
    for (i = (initial+1); i < length; i++) {
        if ((darray[i] < smallest) && (darray[i] > 2)) {
            /* Ignore values smaller than 2, due to unreliable values under the measurable range.*/
            smallest_index = i;
            smallest = darray[i];
        }
    }
    return smallest_index;
}

void any_to_track(){
    lcd_send_line(LINE1,"Narrow Sweep");
    lcd_send_line(LINE2, "Tracking Object");
}
