#include "tracker.h"

int tracker_upper_bound = 0;
int tracker_lower_bound = 0;
int tracker_current_center = 0;
int tracker_current_bias = 0;
int tracker_current_measurements[TRACKER_MEASUREMENT_AMOUNT];
int tracker_averaged_values_bearings[TRACKER_MEASUREMENT_AMOUNT-1] = {-25, -15, -5, 5, 15, 25};

void tracker(void)
{

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
    tracker_current_center = tracker_set_bound(tracker_current_center);

    while (1)
    {
        tracker_narrow_sweep();
        tracker_current_center = tracker_set_bound(tracker_current_center);
        timer_delay(35);
    }
}

/* Perform a full scan to find the initial object. */
int tracker_full_scan(int start, int end, int increment, uint32_t * tracker_ultrasound_range_table, uint32_t * tracker_ir_range_table)
{
    int sensor_position;
    int range_index = 0;
    static uint32_t ultrasound_measured_distance;
    static uint32_t ir_measured_distance;

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
    int tracker_previous_center = tracker_current_center;
    int range_index, new_index, sensor_position, x;
    char lcd_string[20];

    static uint32_t tracker_ultrasound_range_table[7];
    static uint32_t tracker_ir_range_table[7];

    //Move servo to the current object direction.
    servo_set_pos(tracker_current_center);

    //Perform narrow sweep.
    debug_sendf("Starting at %d, Finishing at %d \r\n", tracker_lower_bound, tracker_upper_bound);
    range_index = 0;
    for (x = 0; x < TRACKER_MEASUREMENT_AMOUNT; x++) {
        sensor_position = tracker_current_measurements[x];
        tracker_narrow_sweep_scan(range_index, sensor_position, tracker_ultrasound_range_table, tracker_ir_range_table);
        range_index++;
    }
    
    //Find the angle with the smallest distance.
    new_index = tracker_compare_indices(
        tracker_process_range_table(tracker_ultrasound_range_table, range_index),
        tracker_process_range_table(tracker_ir_range_table, range_index) 
        );
    debug_sendf("tracker_averaged_values_bearings[new_index] = %d\r\n", tracker_averaged_values_bearings[new_index]);
    tracker_current_center += tracker_averaged_values_bearings[new_index];

    if (tracker_current_center > tracker_previous_center) {
        tracker_current_bias = 1;
    } else if (tracker_current_center == tracker_previous_center) {
        tracker_current_bias = 0;
    } else {
        tracker_current_bias = -1;
    }

    debug_sendf("New bias: %d \r\n", tracker_current_bias);

    sprintf(lcd_string, "At %d degrees", tracker_current_center);
    lcd_send_line(LINE2, lcd_string);

    //Clear the range tables.
    for (x = 0; x < 7; x++) {
        tracker_ultrasound_range_table[x] = 0;
        tracker_ir_range_table[x] = 0;
    }
}

/* Peform one pass of the narrow scan on both sensors. */
void tracker_narrow_sweep_scan(int range_index, int sensor_position, uint32_t * tracker_ultrasound_range_table, uint32_t * tracker_ir_range_table)
{
    
    static uint32_t ultrasound_measured_distance, ir_measured_distance;

    servo_set_pos(sensor_position);
    timer_delay(35);
    ir_measured_distance = ir_sensor_get_distance();
    ultrasound_send_test_pulse();
    timer_delay(50);
    ultrasound_measured_distance = ultrasound_get_distance();
    tracker_ultrasound_range_table[range_index] = ultrasound_measured_distance;
    tracker_ir_range_table[range_index] = ir_measured_distance;

}

/* Average two neighbouring values in the result. */
int tracker_process_range_table(uint32_t * range_table, int range_table_size) {

    int i;                                
    uint32_t averaged_table[range_table_size - 1];

    for (i = 0; i < (range_table_size - 1); i++) {
        averaged_table[i] = (range_table[i] + range_table[i+1]) / 2;
        debug_sendf("%d: %lu, ", i, averaged_table[i]);
    }
    debug_sendf("\r\n");
    return tracker_find_smallest_index(averaged_table, (range_table_size - 1), 0);
}

/* Set scan boundaries. */
int tracker_set_bound(current_center)
{
    int i;

    tracker_lower_bound = current_center - 30 + tracker_current_bias * 15;
    tracker_upper_bound = current_center + 30 + tracker_current_bias * 15;

    if ((tracker_upper_bound - tracker_lower_bound) != 60) {
        debug_sendf("upper_bound, lower_bound error!\r\n");
    }

    if (tracker_lower_bound <= 15) {
        tracker_upper_bound += abs(15 - tracker_lower_bound);
        tracker_lower_bound = 15;
    }

    if (tracker_upper_bound >= 255) {
        tracker_lower_bound -= abs(tracker_upper_bound - 255);
        tracker_upper_bound = 255;
    }

    for (i = 0; i < TRACKER_MEASUREMENT_AMOUNT; i++) {
        tracker_current_measurements[i] = tracker_lower_bound + 10 * i;
    }

    for (i = 0; i < (TRACKER_MEASUREMENT_AMOUNT - 1); i++) {
        tracker_averaged_values_bearings[i] = -25 + 10 * i + 15 * tracker_current_bias;
    }

    current_center = (tracker_upper_bound + tracker_lower_bound) / 2;
    
    debug_sendf("tracker_set_bound: current_center = %d\r\n", current_center);
    
    return current_center;

}

/* Take the average of two sensors' indices. */
int tracker_compare_indices(int ultrasound_index, int ir_index)
{
    debug_sendf("U: %d \r\n", ultrasound_index);
    debug_sendf("I: %d \r\n", ir_index);
    
    if (abs(ultrasound_index - ir_index)> 4) {
        return ir_index;
    }

    return ((ultrasound_index + ir_index) / 2);
}

/* Find the index of the smallest element in an array. */
int tracker_find_smallest_index(uint32_t darray[], int length, int initial)
{
    /* Optionally ignore first initial number of values, due to motor still moving at the times of measurements. */
    int smallest_index = initial;
    uint32_t smallest = darray[initial];
    int i;
    for (i = (initial+1); i < length; i++) {
        if ((darray[i] < smallest) && (darray[i] > 10)) {
            /* Ignore unreliable values.*/
            smallest_index = i;
            smallest = darray[i];
        }
    }
    return smallest_index;
}

/* For LCD display. */
void any_to_track(){
    lcd_send_line(LINE1, "Narrow Sweep");
    lcd_send_line(LINE2, "Tracking Object");
}
