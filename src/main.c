#include "keypad.h"
#include "lcd.h"
#include "lpc17xx_gpio.h"
#include "test.h"

typedef enum {
    CALIBRATE,
    SCAN,
    MEASURE,
    MULTI,

    CALIBRATE_NEAR_DONE,
    CALIBRATE_DONE,

    SCAN_PARAMETERS,
    SCAN_PARAMATER_1,
    SCAN_PARAMATER_2,
    SCAN_PARAMATER_3,
    SCAN_DO,

    MEASURE_PARAMETERS,
    MEASURE_PARAMATER_1,
    MEASURE_PARAMATER_2,
    MEASURE_PARAMATER_3,
    MEASURE_DO,

    MULTI_PARAMETERS,
    MULTI_DO_STAGE_1,
    MULTI_DO_STAGE_2,
    MULTI_DO_STAGE_3,
    MULTI_DO_STAGE_4,
    MULTI_DONE
} state_t;

static int current_state_input = 0;
static state_t current_state = CALIBRATE;

int main(void)
{
    lcd_init();
    lcd_clear_display();
    lcd_send_str(0x00, "state 0");
    keypad_init();
    keypad_enable_int();


    while (1)
    {
        switch (current_state) {
            case CALIBRATE:
                lcd_send_str(0x00, "Calibrating");
                break;
            case SCAN:
                lcd_send_str(0x00, "Scanning");
                break;
            case MEASURE:
                lcd_send_str(0x00, "Measuring");
                break;
            case MULTI:
                lcd_send_str(0x00, "Multi-scan");
                break;

            default:
                lcd_send_strf(0x00, "Unknown Mode, %d", current_state);
        }
    }
}

void EINT3_IRQHandler(void) 
{
    if (GPIO_GetIntStatus(0, 23, 1))
    { 
        keypad_clear_int();

        char r[16] = {0};
        get_keyboard_presses(r);

        int i;
        for(i = 0; i < 16; ++i){
            if(r[i] == 1) {
                state_transition(KEYS[i]);
            }
        }
    }
}

/* Transition function */
typedef struct
{
    state_t current;
    char symbol;
    state_t next;
} transition_t;

void state_transition(char key) {

    /* global transitions from any state back to top-level ones */
    switch (key)
    {
        case 'A':
            current_state = CALIBRATE; break;
        case 'B':
            current_state = SCAN; break;
        case 'C':
            current_state = MEASURE; break;
        case 'D':
            current_state = MULTI; break;
    }

    /* special data-input states that require building some int */
    if (current_state == CALIBRATE && key == '#') {
        dist_1 = get_distance_somehow();
    } else if (current_state == SCAN_PARAMATER_1)
    {
        /* example */
        if (key >= '0' && key <= '9') {
            scan_param_1 *= 10;
            scan_param_1 += (key - '0');
        }
    }

    transition_t lut[] = {
        {CALIBRATE_DONE, '#', CALIBRATE},

        {SCAN, '#', SCAN_DO},
        {MEASURE, '#', MEASURE_DO},

        {MULTI, '#', MULTI_DO_STAGE_1},
        /* maybe do this automatically or have a wait? */
        {MULTI_DO_STAGE_1, '#', MULTI_DO_STAGE_2},
        {MULTI_DO_STAGE_2, '#', MULTI_DO_STAGE_3},
        {MULTI_DO_STAGE_3, '#', MULTI_DO_STAGE_4},
    };


    /* look up in transitions? */
}
