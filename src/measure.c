#include <stdlib.h>
#include "measure.h"
#include "lcd.h"
#include "ir_sensor.h"
#include "debug.h"
#include "state.h"
#include "network.h"

//These keep track of what data the measure mode will
//display on the IO board lcd.
#define NO_OF_STATES 2
static enum measure_state {DISTANCE, AVG_DISTANCE};


static uint32_t measure_mean_distance = 0;


void any_to_measure() {
    lcd_send_line(LINE1, "Measure Mode");
    lcd_send_line(LINE2, "# to start");
}

void measure_to_measure_do() {
    lcd_send_line(LINE1, "Distance");
    measure_mean_distance = 0;
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

    uint32_t raw_ir = ir_sensor_get_raw_data();
    ultrasound_send_test_pulse();
    uint32_t dist_ir = ir_convert_to_distance(raw_ir);
    uint32_t dist_us = ultrasound_get_distance(); 
    uint32_t dist = (dist_ir + dist_us)/2;
    network_send(MEASURE_DO, (uint8_t *)&dist, 4, NULL);

    //Calculate the running mean. If the mean is 0 then we set the
    //mean to be the last value as 0 is our reset value. In practice it
    //can never actually have a distance of 0.
    if(measure_mean_distance == 0)
        measure_mean_distance = dist;
    else
        measure_mean_distance = (measure_mean_distance + dist) / 2;

    switch(measure_state){
        case DISTANCE:
            lcd_send_line(LINE1, "Distance");
            lcd_send_line(LINE2, "%d", dist);
            break;
        case AVG_DISTANCE:
            lcd_send_line(LINE1, "Average Distance");
            lcd_send_line(LINE2, "%d", measure_mean_dist);
            break;
        default:
            break;
            
    }
}
