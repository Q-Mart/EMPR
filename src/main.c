#include "empr.h"
#include "debug.h"
#include "keypad.h"
#include "lcd.h"
#include "test.h"
#include "calibration_mode.h"
#include "multi.h"
#include "scan.h"
#include "measure.h"
#include "adc.h"
#include "state.h"
#include "ir_sensor.h"
#include "timer.h"
#include "servo.h"
#include "ultrasound.h"
#include "network.h"

int key_to_int(char key){
    //This was written to allow for numbers to be converted from
    //char to int based on input from the keypad. A signed int is
    //used to make a bit clearer that there is a seperation between
    //what type of keys are pressed.
    //Could be made clearer by replacing with an enum. But this works
    //for the time being.

    //48 is ASCII 0 and 57 is ASCII 9
    if(key >= 48 && key <= 57){
        return key - 48;
    } else {
        switch(key){
        case '*':
            return -1;
        default:
            return -2;//Don't care value
        }
    }
}

static state_t current_state = CALIBRATE;
static int last_key_press = -2;
void state_transition(char key);
void input_poll();
int main(void)
{
    /*
      The order of initialisation matters!
     */
    timer_enable_systick();
    adc_enable();
    debug_init();
    lcd_init();
    lcd_clear_display();
    keypad_init();
    ir_sensor_init();
    ultrasound_initialise_timer_measurement();
    servo_init();
    any_to_calib();

    while (1)
    {
        input_poll();
        switch (current_state) {
            case CALIBRATE:
                break;
            case SCAN_DO:
                scan_loop(last_key_press);
                break;
            case SCAN_PARAMETER_1:
                scan_parameter_1_loop(last_key_press);
                break;
            case SCAN_PARAMETER_2:
                scan_parameter_2_loop(last_key_press);
                break;
            case SCAN_PARAMETER_3:
                scan_parameter_3_loop(last_key_press);
                break;
            case MEASURE_DO:
                measure_loop(last_key_press);
                break;
            case MULTI_SWEEP:
                multi_sweep_loop();
                break;
            case MULTI_WAIT:
                multi_wait_loop();
                break;
            case MULTI_SWEEP_NUMBER:
                multi_sweep_number_loop(last_key_press);
                break;
            case MULTI_MIN_ANGLE:
                multi_min_angle_loop(last_key_press);
                break;
            case MULTI_MAX_ANGLE:
                multi_max_angle_loop(last_key_press);
                break;
            case MULTI_DONE:
                multi_done_loop();
                break;
            default:
                //have a break, have a kit kat
                break;
        }
    }

    debug_send("PROGRAM END\n\r");
}

void input_poll(void){
    char r[16] = {0};
    get_keyboard_presses(r);
    last_key_press = -2; //Set to no last key pressed
    int i;
    for(i = 0; i < 16; ++i){
        if(r[i] == 1) {
            //The order of these matters because otherwise the
            //key pressed for the transition will also register as
            //an input for the corresponding loop function
            last_key_press = key_to_int(KEYS[i]);
            state_transition(KEYS[i]);
            timer_delay(100);//Prevent button bounce
        }
    }
}

/* Transition function */
typedef void (*side_func)(void);

typedef struct
{
    state_t current;
    char symbol;
    state_t next;
    side_func effect;
} transition_t;

const transition_t lut[] = {
    {CALIBRATE_DONE, '#', CALIBRATE, NULL},
    {CALIBRATE, '#', CALIBRATE_NEAR_DONE, &calib_to_near_calib},
    {CALIBRATE_NEAR_DONE, '#', CALIBRATE_DONE, &near_calib_to_done},

    {SCAN, '#', SCAN_DO, &scan_to_scan_do},
    {SCAN_DO, '*', SCAN, &any_to_scan},
    {SCAN, '*', SCAN_PARAMETERS, &any_to_scan_parameters},
    {SCAN_PARAMETERS, '*', SCAN, &any_to_scan},
    {SCAN_PARAMETERS, '1', SCAN_PARAMETER_1, &scan_parameters_to_1},
    {SCAN_PARAMETERS, '#', SCAN, &any_to_scan},
    {SCAN_PARAMETER_1, '#', SCAN_PARAMETERS, &scan_parameter_1_to_scan_parameters},
    {SCAN_PARAMETERS, '2', SCAN_PARAMETER_2, &scan_parameters_to_2},
    {SCAN_PARAMETER_2, '#', SCAN_PARAMETERS, &scan_parameter_2_to_scan_parameters},
    {SCAN_PARAMETERS, '3', SCAN_PARAMETER_3, &scan_parameters_to_3},
    {SCAN_PARAMETER_3, '#', SCAN_PARAMETERS, &scan_parameter_3_to_scan_parameters},


    {MEASURE, '#', MEASURE_DO, &measure_to_measure_do},
    {MEASURE_DO, '*', MEASURE, &any_to_measure},

    {MULTI, '#', MULTI_SWEEP_NUMBER, &multi_to_multi_sweep_number},
    {MULTI_SWEEP_NUMBER, '#', MULTI_MIN_ANGLE, &multi_sweep_number_to_multi_min_angle},
    {MULTI_MIN_ANGLE, '#', MULTI_MAX_ANGLE, &multi_min_angle_to_multi_max_angle},
    {MULTI_MAX_ANGLE, '#', MULTI_SWEEP, NULL},
    {MULTI_WAIT, '#', MULTI_SWEEP, NULL},
    {MULTI_DONE, '#', CALIBRATE, NULL},

    {ANY, 'A', CALIBRATE, &any_to_calib},
    {ANY, 'B', SCAN, &any_to_scan},
    {ANY, 'C', MEASURE, &any_to_measure},
    {ANY, 'D', MULTI, &any_to_multi}
};

void state_transition(char key){
    /* global transitions from any state back to top-level ones */
    int i;
     for(i = 0; i < sizeof(lut)/sizeof(lut[0]); i++){
        if ((lut[i].current == current_state || lut[i].current == ANY) && lut[i].symbol == key){
            if(current_state == lut[i].next) return;
            lcd_clear_display();
            if(lut[i].effect !=NULL) (*(lut[i].effect))();
            current_state = lut[i].next;
            last_key_press = -2;//Reset last key press to stop it being used.
            return;
        }
    }
}

void change_state(state_t state) {
    current_state = state;
}
