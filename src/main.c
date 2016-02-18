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

int key_to_int(char key){
    //This was written to allow for numbers to be converted from
    //char to int based on input from the keypad. A signed int is
    //used to make a bit clearer that there is a seperation between
    //what type of keys are pressed.
    //Could be made clearer by replacing with an enum. But this works
    //for the time being.
    switch(key){
        case '0':
            return 0;
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case '*':
            return -1;
        default:
            return -2;//Don't care value
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
                scan_loop();
                break;
            case SCAN_PARAMETERS_1:
                scan_parameters_1_loop(last_key_press);
                break;
            case MEASURE_DO:
                measure_loop();
                break;
            case MULTI:
                break;
            default:
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
            state_transition(KEYS[i]);
            last_key_press = key_to_int(KEYS[i]);
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
    {SCAN, '*', SCAN_PARAMETERS, NULL},
    {SCAN_PARAMETERS, '1', SCAN_PARAMETERS_1, &scan_parameters_to_1},
    {SCAN_PARAMETERS_1, '#', SCAN_PARAMETERS, &scan_parameters_1_to_scan_parameters},


    {MEASURE, '#', MEASURE_DO, NULL},
    {MULTI, '#', MULTI_DO_STAGE_1, NULL},
    /* MAYBE DO THIS AUTOMATICALLY OR HAVE A WAIT? */
    {MULTI_DO_STAGE_1, '#', MULTI_DO_STAGE_2, NULL},
    {MULTI_DO_STAGE_2, '#', MULTI_DO_STAGE_3, NULL},
    {MULTI_DO_STAGE_3, '#', MULTI_DO_STAGE_4, NULL},

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
            return;
        }
    }
}
