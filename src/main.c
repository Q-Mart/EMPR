#include "debug.h"
#include "keypad.h"
#include "lcd.h"
#include "lpc17xx_gpio.h"
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
#include "sound.h"

static state_t current_state = CALIBRATE;
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
    servo_init();
    any_to_calib();
    sound_init();

    while (1)
    {
        input_poll();
        switch (current_state) {
            case CALIBRATE:
                break;
            case SCAN:
                scan_loop();
                break;
            case MEASURE:
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
    int i;
    for(i = 0; i < 16; ++i){
        if(r[i] == 1) {
            state_transition(KEYS[i]);
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
    {SCAN, '#', SCAN_DO, NULL},
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
