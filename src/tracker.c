#include "tracker.h"

int tracker_upper_bound = 0;
int tracker_lower_bound = 0;
int tracker_current_center = 0;
const int averaged_values_bearings[] = {-25, -15, -5, 5, 15, 25};

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
    tracker_set_bound();

    while (1)
    {
        tracker_narrow_sweep();
        tracker_set_bound();
        timer_delay(35);
    }
}

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
    int range_index, new_index, scan_start, scan_end, sensor_position, x;
    char lcd_string[20];

    static uint32_t tracker_ultrasound_range_table[7];
    static uint32_t tracker_ir_range_table[7];

    //Set boundaries for sweep.
    if (abs(tracker_lower_bound - servo_get_pos()) < abs(tracker_upper_bound - servo_get_pos()))
    {
        scan_start = tracker_lower_bound;
        scan_end = tracker_upper_bound;
    }
    else {
        scan_start = tracker_upper_bound;
        scan_end = tracker_lower_bound;
    }
    tracker_current_center = (tracker_upper_bound + tracker_lower_bound) / 2;


    //Move servo to the current object direction.
    servo_set_pos(tracker_current_center);

    //Perform narrow sweep.
    debug_sendf("Starting at %d, Finishing at %d \r\n", scan_start, scan_end);
    range_index = 0;
    if (scan_start <= scan_end) {
        for (sensor_position = scan_start; sensor_position <= scan_end; sensor_position += 10) {
            tracker_narrow_sweep_scan(range_index, sensor_position, tracker_ultrasound_range_table, tracker_ir_range_table);
            range_index++;
        }
    }
    else {
        for (sensor_position = scan_start; sensor_position >= scan_end; sensor_position -= 10) {
            tracker_narrow_sweep_scan(range_index, sensor_position, tracker_ultrasound_range_table, tracker_ir_range_table);
            range_index++;
        }
    }
    
    //Find the angle with the smallest distance.
    new_index = tracker_compare_indices(
        tracker_process_range_table(tracker_ultrasound_range_table, range_index),
        tracker_process_range_table(tracker_ir_range_table, range_index) 
        );
    tracker_current_center += averaged_values_bearings[new_index];

    sprintf(lcd_string, "At %d degrees", tracker_current_center);
    lcd_send_line(LINE2, lcd_string);

    //Clear the range tables.
    for (x = 0; x < 7; x++) {
        tracker_ultrasound_range_table[x] = 0;
        tracker_ir_range_table[x] = 0;
    }
}

/* Perform one pass of the narrow sweep, median of 3 measurements. */
void tracker_narrow_sweep_scan(int range_index, int sensor_position, uint32_t * tracker_ultrasound_range_table, uint32_t * tracker_ir_range_table)
{
    int i = 0;
    static uint32_t ultrasound_measurements[3], ir_measurements[3];
    servo_set_pos(sensor_position);
    timer_delay(35);

    for (i = 0; i < 3; i++) {
        ultrasound_send_test_pulse();
        ir_measurements[i] = ir_sensor_get_distance();
        timer_delay(30);
        ultrasound_measurements[i] = ultrasound_get_distance();
    }

    tracker_ultrasound_range_table[range_index] = ultrasound_get_sample_median(ultrasound_measurements);
    tracker_ir_range_table[range_index] = ultrasound_get_sample_median(ir_measurements);

    debug_sendf("tracker_ultrasound_range_table[range_index] = %lu \r\n", tracker_ultrasound_range_table[range_index]);
    debug_sendf("tracker_ir_range_table[range_index] = %lu \r\n", tracker_ir_range_table[range_index]);

}

/* Average out values between two. */
int tracker_process_range_table(uint32_t * range_table, int range_table_size) {

    int i;                                
    uint32_t averaged_table[range_table_size - 1];
    /* range_table at   -30 -20 -10  0  10  20  30 degrees 
       averaged_table at -25, -15, -5, 5, 15, 25 degrees, taking 2 closest values in range_table
    */
    for (i = 0; i < (range_table_size - 1); i++) {
        averaged_table[i] = (range_table[i] + range_table[i+1]) / 2;
        debug_sendf("%d: %lu, ", i, averaged_table[i]);
    }
    debug_sendf("\r\n");
    return tracker_find_smallest_index(averaged_table, (range_table_size - 1), 0);
}

/* Configure the bound for the tracker */
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

/* Compare the indices of two sets of measurements, favour IR accuracy */
int tracker_compare_indices(int ultrasound_index, int ir_index)
{
    debug_sendf("U: %d \r\n", ultrasound_index);
    debug_sendf("I: %d \r\n", ir_index);

    if (abs(ultrasound_index - ir_index) > 6)
        return ir_index;
        
    return ((ultrasound_index + ir_index) / 2);
}

/* Pick out the index of the smallest element in an array */
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

/* LCD text display */
void any_to_track(){
    lcd_send_line(LINE1, "Narrow Sweep");
    lcd_send_line(LINE2, "Tracking Object");
}
