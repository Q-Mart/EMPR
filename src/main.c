#include <stdio.h>
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

int tracker_find_smallest_index(double darray[], int length)
{
    /* Ignore first two values, due to motor still moving at the times of measurements. */
    int smallest_index = 2;
    double smallest = darray[0];
    int i = 2;
    for (i = 2; i < length; i++) {
        if ((darray[i] < smallest) && (darray[i] < 5)) {
            /* Ignore values smaller than 5, due to unreliable values under the measurable range.*/
            smallest_index = i;
            smallest = darray[i];
        }
    }
    return smallest_index;
}

int main(void)
{
    debug_init();

    lcd_init();
    lcd_clear_display();
    
    ultrasound_initialise_timer_measurement();
    servo_init();

    //Not calibrated in example.
    double measured_distance = 0;
    char print_distance[64];
    double range_table[28];
    int range_index = 0;
    int sensor_position = 0;
    servo_set_pos(sensor_position);
    timer_delay(1000);
    for (sensor_position = 0; sensor_position <= 250; sensor_position += 10)
    {
        servo_set_pos(sensor_position);
        timer_delay(375);
        ultrasound_send_test_pulse();
        measured_distance = ultrasound_process_value(340, (int)ultrasound_valid_response_time);
        sprintf(print_distance, "Distance: %0.7f cm\r\n\r\n", measured_distance);
        debug_send(print_distance);
        range_table[range_index] = measured_distance;
        range_index++;
    }
    int closest_index = 0;
    closest_index = tracker_find_smallest_index(range_table, range_index);
    debug_sendfc("Closest Index: %d\r\n", closest_index);
    servo_set_pos(closest_index * 10);

}
