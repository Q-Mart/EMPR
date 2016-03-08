#include <stdlib.h>
#include "measure.h"
#include "lcd.h"
#include "ir_sensor.h"
#include "debug.h"
#include "state.h"
#include "network.h"
#include "ultrasound.h"
#include "utils.h"
//These keep track of what data the measure mode will
//display on the IO board lcd.
#define NO_OF_STATES 4
static enum _measure_state{DISTANCE, AVG_DISTANCE, IR_DISTANCE, US_DISTANCE} measure_state = DISTANCE;

static uint64_t measure_total_distance = 0;
static uint32_t measure_count = 0;


void any_to_measure() {
    servo_set_pos(160);
    lcd_send_line(LINE1, "Measure Mode");
    lcd_send_line(LINE2, "# to start");
    network_send(MEASURE, NULL);
}

void measure_to_measure_do() {
    servo_set_pos(160);
    lcd_send_line(LINE1, "Distance");
    measure_count = 0;
    measure_total_distance = 0;
    //We reset the mean distance
}

void measure_loop(int last_key_press) {
    //Uses the last key press the change what data is displayed
    //on the lcd.
    //This should not be too vulnerable to button bounce as there
    //is a sleep during the process that records the data from
    //the ultrasound later in this function.
    if(last_key_press == 3)
        measure_state = (measure_state + 1) % NO_OF_STATES;
    if(last_key_press == 1)
        measure_state = (measure_state - 1 + NO_OF_STATES) % NO_OF_STATES;

    uint32_t dist = utils_get_ir_and_ultrasound_median_distance();
    network_send(MEASURE_DO, (uint8_t *)&dist, 4, NULL);

    //Calculate the running mean. If the mean is 0 then we set the
    //mean to be the last value as 0 is our reset value. In practice it
    //can never actually have a distance of 0.
    measure_total_distance += dist;
    measure_count++;
    switch(measure_state){
        case DISTANCE:
            lcd_send_line(LINE1, "Distance");
            lcd_send_line(LINE2, "%d", dist);
            break;
        case AVG_DISTANCE:
            lcd_send_line(LINE1, "Average Distance");
            lcd_send_line(LINE2, "%d", measure_total_distance/measure_count);
            break;
        case IR_DISTANCE:
            lcd_send_line(LINE1, "IR Distance");
            lcd_send_line(LINE2, "%d", utils_get_last_ir_reading());
            break;
        case US_DISTANCE:
            lcd_send_line(LINE1, "US Distance");
            lcd_send_line(LINE2, "%d", utils_get_last_us_reading());
        default:
            break;
    }
}

