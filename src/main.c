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

typedef void (*side_func)(void);

typedef struct
{
    state_t current;
    char symbol;
    state_t next;
    side_func effect;
} transition_t;

const transition_t lut[] = {
    {calibrate_done, '#', calibrate, null},
    {scan, '#', scan_do, null},
    {measure, '#', measure_do, null},
    {multi, '#', multi_do_stage_1, null},
    /* maybe do this automatically or have a wait? */
    {multi_do_stage_1, '#', multi_do_stage_2, null},
    {multi_do_stage_2, '#', multi_do_stage_3, null},
    {multi_do_stage_3, '#', multi_do_stage_4, null},
};
void state_transition(char key){
    /* global transitions from any state back to top-level ones */
    int i;
	for(i = 0; i < sizeof(lut)/sizeof(lut[0]); i++){
			if (lut[i].current = current_state && lut[i].symbol == key){
					if(lut[i].effect !=null) (*(lut[i].effect))(void);
					current_state = lut[i].next;
			}
	}
}
