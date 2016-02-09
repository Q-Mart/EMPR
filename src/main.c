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

/* Instructions to use ultrasound module:
    Connect Echo pin of the sensor to P29 on IO Board;
    Connect Trigger pin of the sensor to P8 on IO Board;
    Call initialise_timer_measurement() at start;
    Now calibrate the sensor, calculated the speed of sound
     as it appears with the calibration object, in m/s, save
     in calibrated_gradient;
    Call send_test_pulse() to send a pulse, then immediately:
    call process_ultrasound_value(calibrated_gradient,\
        (int)ultrasound_valid_response_time) 
     to obtain object distance in cm.
*/
int tracker_full_scan(int, int, int, uint32_t*);
void tracker_narrow_sweep();
int tracker_find_smallest_index(uint32_t*, int, int);
void tracker_set_bound();
int tracker_compare_indices(int, int);

int tracker_upper_bound = 0;
int tracker_lower_bound = 0;
int tracker_current_center = 0;

int main(void)
{
    debug_init();

    lcd_init();
    lcd_clear_display();
    
    ultrasound_initialise_timer_measurement();
    servo_init();

    uint32_t tracker_ultrasound_range_table[32], tracker_ir_range_table[32];
    int tracker_ultrasound_range_index, tracker_ir_range_index;

    tracker_ultrasound_range_index = tracker_full_scan(0, 250, 10, tracker_ultrasound_range_table);
    tracker_current_center = tracker_find_smallest_index(tracker_ultrasound_range_table, tracker_ultrasound_range_index, 2) * 10;
    servo_set_pos(tracker_current_center);
    tracker_set_bound();

/*
    char print_distance[80];
    int ii;
    for (ii = 0; ii < 25; ii++) {
        debug_sendfc("%d :", ii);
        sprintf(print_distance, "Value: %d \r\n", tracker_ultrasound_range_table[ii]);
        debug_send(print_distance);
    }
    debug_sendfc("Center: %d\r\n", current_center); */

    while (1)
    {
        tracker_narrow_sweep();
        tracker_set_bound();
        timer_delay(125);
    }
}

int tracker_full_scan(int start, int end, int increment, uint32_t * tracker_ultrasound_range_table)
{
    int sensor_position;
    int range_index = 0;
    static uint32_t measured_distance;
    char print_distance[64];
    debug_send("===Beginning full scan.===\r\n");
    for (sensor_position = start; sensor_position <= end; sensor_position += increment)
    {
        servo_set_pos(sensor_position);
        timer_delay(375);
        ultrasound_send_test_pulse();
        measured_distance = ultrasound_process_value(340, 0, ultrasound_valid_response_time);
        sprintf(print_distance, "Distance: %lu um\r\n\r\n", measured_distance);
        debug_send(print_distance);
        tracker_ultrasound_range_table[range_index] = measured_distance;
        range_index++;
    }
    debug_send("===Full scan complete.===\r\n");
    return range_index;
}

/* Narrow sweep within a cone of +-20 degrees, returns the new center */
void tracker_narrow_sweep()
{
    int range_index, new_index, scan_start, scan_end, sensor_position;

    static uint32_t measured_distance;
    uint32_t tracker_ultrasound_range_table[15];
    int tracker_ultrasound_index_table[15];

    if (abs(tracker_lower_bound - servo_get_pos()) < abs(tracker_upper_bound - servo_get_pos()))
    {
        scan_start = tracker_lower_bound;
        scan_end = tracker_upper_bound;
    }
    else {
        scan_start = tracker_upper_bound;
        scan_end = tracker_lower_bound;
    }

    if (scan_start <= scan_end) {
        range_index = 0;
        for (sensor_position = scan_start; sensor_position <= scan_end; sensor_position += 5) {
            servo_set_pos(sensor_position);
            timer_delay(75);
            ultrasound_send_test_pulse();
            measured_distance = ultrasound_process_value(340, 0, ultrasound_valid_response_time);
            tracker_ultrasound_range_table[range_index] = measured_distance;
            tracker_ultrasound_index_table[range_index] = range_index - 6;
            range_index++;
        }
    }
    else {
        range_index = 0;
        for (sensor_position = scan_start; sensor_position >= scan_end; sensor_position -= 5) {
            servo_set_pos(sensor_position);
            timer_delay(75);
            ultrasound_send_test_pulse();
            measured_distance = ultrasound_process_value(340, 0, ultrasound_valid_response_time);
            tracker_ultrasound_range_table[range_index] = measured_distance;
            tracker_ultrasound_index_table[range_index] = -1 * (range_index - 6);
            range_index++;
        }
    }
    
    new_index = tracker_find_smallest_index(tracker_ultrasound_range_table, range_index, 0);
    tracker_current_center = (tracker_upper_bound + tracker_lower_bound) / 2 + 5 * tracker_ultrasound_index_table[new_index];
    debug_sendf("The sensor thinks that the object is at %d degrees.\r\n", tracker_current_center);
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
        if ((darray[i] < smallest) && (darray[i] > 20000)) {
            /* Ignore values smaller than 2, due to unreliable values under the measurable range.*/
            smallest_index = i;
            smallest = darray[i];
        }
    }
    return smallest_index;
}